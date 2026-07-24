#include <algorithm>
#include <cstdint>
#include <limits>
#include <thread>

#include "portfolio/Portfolio.h"

namespace
{
	// Splits [0, n) into contiguous chunks and runs work(start, end) on each,
	// one std::thread per chunk. Deterministic: output element i depends only
	// on input element i, so the chunking never changes results.
	template<typename Work>
	void runChunks(size_t n, unsigned int numThreads, const Work& work)
	{
		if (n == 0)
		{
			return;
		}
		size_t hw = numThreads > 0 ? numThreads : std::thread::hardware_concurrency();
		if (hw == 0)
		{
			hw = 1;
		}
		hw = std::min(hw, n);
		if (hw == 1)
		{
			work(0, n);
			return;
		}
		size_t chunk = (n + hw - 1) / hw;
		std::vector<std::thread> threads;
		threads.reserve(hw);
		for (size_t start = 0; start < n; start += chunk)
		{
			threads.emplace_back(work, start, std::min(n, start + chunk));
		}
		for (auto& th : threads)
		{
			th.join();
		}
	}

	constexpr double nan_ = std::numeric_limits<double>::quiet_NaN();
}

namespace QCode
{
	namespace Financial
	{
		void Portfolio::add(const Operation& operation)
		{
			auto inserted = _operations.emplace(operation.getKey(), operation);
			if (!inserted.second)
			{
				throw std::invalid_argument(
					"Portfolio: operation with key " +
					std::to_string(operation.getKey()) + " already exists.");
			}
			if (operation.getMaxSettlementSerial() > _maxSettlementSerial)
			{
				_maxSettlementSerial = operation.getMaxSettlementSerial();
			}
		}

		void Portfolio::remove(long long key)
		{
			if (_operations.erase(key) == 0)
			{
				throw std::invalid_argument(
					"Portfolio: no operation with key " + std::to_string(key) + ".");
			}
		}

		std::vector<Portfolio::LegRef> Portfolio::_makeSnapshot(std::vector<std::string>& legend)
		{
			std::vector<LegRef> refs;
			std::map<std::string, int32_t> ccyToIndex;
			for (auto& [key, op] : _operations)
			{
				for (size_t i = 0; i < op.numberOfLegs(); ++i)
				{
					auto& leg = op.getLeg(i);
					auto isoCode = leg.getCashflowAt(0)->ccy()->getIsoCode();
					auto found = ccyToIndex.find(isoCode);
					int32_t ccyIndex;
					if (found == ccyToIndex.end())
					{
						ccyIndex = static_cast<int32_t>(legend.size());
						ccyToIndex.emplace(isoCode, ccyIndex);
						legend.push_back(isoCode);
					}
					else
					{
						ccyIndex = found->second;
					}
					refs.push_back(LegRef{key, static_cast<int32_t>(i + 1), &leg, ccyIndex});
				}
			}
			return refs;
		}

		StateColumns Portfolio::statesAt(
			const QCDate& t,
			const CurveMap& curves,
			unsigned int numThreads)
		{
			StateColumns result;
			auto refs = _makeSnapshot(result.currencyLegend);
			auto n = refs.size();

			// Per-currency discount factor tables, built serially: the curve query
			// chain (curve, interpolator, QCInterestRate) mutates internal state on
			// every call and must never be touched from worker threads.
			long tSerial = t.excelSerial();
			long maxOffset = _maxSettlementSerial > tSerial ? _maxSettlementSerial - tSerial : 0;
			std::vector<std::vector<double>> dfTables(result.currencyLegend.size());
			for (size_t c = 0; c < result.currencyLegend.size(); ++c)
			{
				auto curve = curves.find(result.currencyLegend[c]);
				if (curve == curves.end())
				{
					continue;
				}
				auto& table = dfTables[c];
				table.resize(maxOffset + 1);
				table[0] = 1.0;
				for (long d = 1; d <= maxOffset; ++d)
				{
					table[d] = curve->second->getDiscountFactorAt(d);
				}
			}

			result.opKey.resize(n);
			result.legNumber.resize(n);
			result.currency.resize(n);
			result.accruedInterest.resize(n);
			result.outstandingNotional.resize(n);
			result.interestSettling.resize(n);
			result.amortizationSettling.resize(n);
			result.totalSettling.resize(n);
			result.nextFlowDate.resize(n);
			result.presentValue.resize(n);

			runChunks(n, numThreads, [&](size_t start, size_t end) {
				for (size_t idx = start; idx < end; ++idx)
				{
					auto& ref = refs[idx];
					const auto& dfTable = dfTables[ref.ccyIndex];
					bool hasCurve = !dfTable.empty();
					bool currentFound = false;
					double accrued = 0.0;
					double nominal = 0.0;
					double interestSettling = 0.0;
					double totalSettling = 0.0;
					double pv = 0.0;
					long long nextFlow = 0;
					for (size_t j = 0; j < ref.leg->size(); ++j)
					{
						auto cf = ref.leg->getCashflowAt(j);
						long serial = cf->date().excelSerial();
						if (!currentFound && cf->startDate() <= t && t < cf->endDate())
						{
							currentFound = true;
							accrued = cf->accruedInterest(t);
							nominal = cf->getNominal();
						}
						if (serial == tSerial)
						{
							// Raw amount(), never settlementAmount(): rounding is the
							// caller's job at the settlement edge (requirement 6.3).
							double total = cf->amount();
							double interest = cf->accruedInterest(cf->endDate());
							totalSettling += total;
							interestSettling += interest;
						}
						else if (serial > tSerial)
						{
							if (nextFlow == 0)
							{
								nextFlow = serial;
							}
							if (hasCurve)
							{
								pv += cf->amount() * dfTable[serial - tSerial];
							}
						}
					}
					result.opKey[idx] = ref.key;
					result.legNumber[idx] = ref.legNumber;
					result.currency[idx] = ref.ccyIndex;
					result.accruedInterest[idx] = accrued;
					result.outstandingNotional[idx] = nominal;
					result.interestSettling[idx] = interestSettling;
					result.amortizationSettling[idx] = totalSettling - interestSettling;
					result.totalSettling[idx] = totalSettling;
					result.nextFlowDate[idx] = nextFlow;
					result.presentValue[idx] = hasCurve ? pv : nan_;
				}
			});

			return result;
		}

		FlowColumns Portfolio::flowsBetween(
			const QCDate& t1,
			const QCDate& t2,
			unsigned int numThreads)
		{
			if (!(t1 < t2))
			{
				throw std::invalid_argument("Portfolio::flowsBetween: t1 must be earlier than t2.");
			}
			FlowColumns result;
			auto refs = _makeSnapshot(result.currencyLegend);
			auto n = refs.size();
			long t1Serial = t1.excelSerial();
			long t2Serial = t2.excelSerial();

			// Count pass: flows per leg in (t1, t2].
			std::vector<size_t> counts(n, 0);
			runChunks(n, numThreads, [&](size_t start, size_t end) {
				for (size_t idx = start; idx < end; ++idx)
				{
					size_t count = 0;
					auto* leg = refs[idx].leg;
					for (size_t j = 0; j < leg->size(); ++j)
					{
						long serial = leg->getCashflowAt(j)->date().excelSerial();
						if (t1Serial < serial && serial <= t2Serial)
						{
							++count;
						}
					}
					counts[idx] = count;
				}
			});

			// Prefix sum -> per-leg output offsets, rows stay in snapshot order.
			std::vector<size_t> offsets(n + 1, 0);
			for (size_t idx = 0; idx < n; ++idx)
			{
				offsets[idx + 1] = offsets[idx] + counts[idx];
			}
			auto totalRows = offsets[n];

			result.opKey.resize(totalRows);
			result.legNumber.resize(totalRows);
			result.settlementDate.resize(totalRows);
			result.interest.resize(totalRows);
			result.amortization.resize(totalRows);
			result.total.resize(totalRows);
			result.currency.resize(totalRows);

			// Fill pass: each leg writes its own disjoint range.
			runChunks(n, numThreads, [&](size_t start, size_t end) {
				for (size_t idx = start; idx < end; ++idx)
				{
					auto& ref = refs[idx];
					size_t row = offsets[idx];
					for (size_t j = 0; j < ref.leg->size(); ++j)
					{
						auto cf = ref.leg->getCashflowAt(j);
						long serial = cf->date().excelSerial();
						if (t1Serial < serial && serial <= t2Serial)
						{
							double total = cf->amount();
							double interest = cf->accruedInterest(cf->endDate());
							result.opKey[row] = ref.key;
							result.legNumber[row] = ref.legNumber;
							result.settlementDate[row] = serial;
							result.interest[row] = interest;
							result.amortization[row] = total - interest;
							result.total[row] = total;
							result.currency[row] = ref.ccyIndex;
							++row;
						}
					}
				}
			});

			return result;
		}
	}
}
