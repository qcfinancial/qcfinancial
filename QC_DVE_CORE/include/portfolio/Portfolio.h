#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "asset_classes/ZeroCouponCurve.h"
#include "portfolio/Operation.h"
#include "time/QCDate.h"

namespace QCode
{
	namespace Financial
	{
		/**
		 * @struct	StateColumns
		 *
		 * @brief	Columnar result of Portfolio::statesAt. All vectors have the same
		 * 			length N (total live legs); element i of every vector refers to the
		 * 			same leg. Rows are ordered by (op_key, leg_number). currency holds an
		 * 			index into currencyLegend.
		 */
		struct StateColumns
		{
			std::vector<long long> opKey;
			std::vector<int32_t> legNumber;
			std::vector<int32_t> currency;
			std::vector<double> accruedInterest;
			std::vector<double> outstandingNotional;
			std::vector<double> interestSettling;
			std::vector<double> amortizationSettling;
			std::vector<double> totalSettling;
			std::vector<long long> nextFlowDate;
			std::vector<double> presentValue;
			std::vector<std::string> currencyLegend;
		};

		/**
		 * @struct	FlowColumns
		 *
		 * @brief	Columnar result of Portfolio::flowsBetween. Rows ordered by
		 * 			(op_key, leg_number, settlement_date).
		 */
		struct FlowColumns
		{
			std::vector<long long> opKey;
			std::vector<int32_t> legNumber;
			std::vector<long long> settlementDate;
			std::vector<double> interest;
			std::vector<double> amortization;
			std::vector<double> total;
			std::vector<int32_t> currency;
			std::vector<std::string> currencyLegend;
		};

		/**
		 * @class	Portfolio
		 *
		 * @brief	Container of Operations keyed by their opaque key, built once and
		 * 			mutated incrementally. Receiver of the batch state queries. Batch
		 * 			queries parallelize across operations with std::thread and are
		 * 			bitwise deterministic regardless of thread count.
		 */
		class Portfolio
		{
		public:
			using CurveMap = std::map<std::string, std::shared_ptr<ZeroCouponCurve>>;

			Portfolio() = default;

			/**
			 * @brief	Adds an operation. Throws std::invalid_argument on duplicate key.
			 */
			void add(const Operation& operation);

			/**
			 * @brief	Removes the operation with the given key. Throws
			 * 			std::invalid_argument if the key is not present.
			 */
			void remove(long long key);

			[[nodiscard]] size_t size() const
			{
				return _operations.size();
			}

			/**
			 * @brief	State of every leg of every operation at date t, columnar.
			 * 			If curves is non-empty, presentValue is computed per leg using the
			 * 			curve matching the leg's currency ISO code (NaN if missing);
			 * 			otherwise presentValue is NaN-filled. numThreads == 0 means
			 * 			hardware concurrency.
			 */
			StateColumns statesAt(
				const QCDate& t,
				const CurveMap& curves = CurveMap(),
				unsigned int numThreads = 0);

			/**
			 * @brief	Every contractual flow with settlement date in (t1, t2],
			 * 			columnar. numThreads == 0 means hardware concurrency.
			 */
			FlowColumns flowsBetween(
				const QCDate& t1,
				const QCDate& t2,
				unsigned int numThreads = 0);

		private:
			struct LegRef
			{
				long long key;
				int32_t legNumber;
				Leg* leg;
				int32_t ccyIndex;
			};

			// Flat (key, leg) snapshot in map order plus the currency legend.
			std::vector<LegRef> _makeSnapshot(std::vector<std::string>& legend);

			std::map<long long, Operation> _operations;

			// ponytail: monotone max over every operation ever added; removals do not
			// shrink it. Only oversizes per-query df tables (bounded by portfolio
			// horizon, ~11k doubles for 30y). Track live max if that ever matters.
			long _maxSettlementSerial = 0;
		};
	}
}

#endif //PORTFOLIO_H
