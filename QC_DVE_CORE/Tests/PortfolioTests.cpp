//
// Created by Alvaro Diaz Valenzuela on 2026-07-17.
//

#include "catch/catch-2.hpp"

#include <cmath>
#include <memory>

#include "asset_classes/QCAct360.h"
#include "asset_classes/QCCompoundWf.h"
#include "asset_classes/QCCurrency.h"
#include "asset_classes/QCInterestRate.h"
#include "asset_classes/QCLinearWf.h"
#include "asset_classes/ZeroCouponCurve.h"
#include "cashflows/FixedRateCashflow.h"
#include "curves/QCLinearInterpolator.h"
#include "portfolio/Operation.h"
#include "portfolio/Portfolio.h"
#include "time/QCDate.h"

using namespace QCode::Financial;

namespace
{
	QCInterestRate testRate(double value = 0.05)
	{
		return QCInterestRate(value, std::make_shared<QCAct360>(), std::make_shared<QCLinearWf>());
	}

	std::shared_ptr<FixedRateCashflow> makeCashflow(
		const QCDate& start,
		const QCDate& end,
		double nominal,
		double amortization,
		bool doesAmortize,
		double rateValue = 0.05)
	{
		return std::make_shared<FixedRateCashflow>(
			start, end, end, nominal, amortization, doesAmortize,
			testRate(rateValue), std::make_shared<QCCLP>());
	}

	// Two-period leg: [start, mid) then [mid, end), settling at period end.
	Leg makeBulletLeg(double nominal = 1000000.0, double rateValue = 0.05)
	{
		QCDate start{15, 1, 2026};
		QCDate mid{15, 7, 2026};
		QCDate end{15, 1, 2027};
		Leg leg;
		leg.appendCashflow(makeCashflow(start, mid, nominal, 0.0, false, rateValue));
		leg.appendCashflow(makeCashflow(mid, end, nominal, nominal, true, rateValue));
		return leg;
	}

	Leg makeCustomAmortLeg()
	{
		QCDate start{15, 1, 2026};
		QCDate mid{15, 7, 2026};
		QCDate end{15, 1, 2027};
		Leg leg;
		leg.appendCashflow(makeCashflow(start, mid, 1000000.0, 400000.0, true));
		leg.appendCashflow(makeCashflow(mid, end, 600000.0, 600000.0, true));
		return leg;
	}

	Operation makeOperation(long long key, Leg leg)
	{
		return Operation(key, {std::move(leg)}, {RecPay::Receive});
	}

	std::shared_ptr<ZeroCouponCurve> makeFlatCurve(double rateValue = 0.05)
	{
		auto plazos = std::vector<long>{1, 3650};
		auto tasas = std::vector<double>{rateValue, rateValue};
		auto curve = std::make_shared<QCCurve<long>>(plazos, tasas);
		auto interpolator = std::make_shared<QCLinearInterpolator>(curve);
		return std::make_shared<ZeroCouponCurve>(
			interpolator,
			QCInterestRate(0.0, std::make_shared<QCAct360>(), std::make_shared<QCCompoundWf>()));
	}
}

TEST_CASE("Operation construction and validation")
{
	SECTION("valid single leg operation")
	{
		auto op = makeOperation(1001, makeBulletLeg());
		REQUIRE(op.getKey() == 1001);
		REQUIRE(op.numberOfLegs() == 1);
		REQUIRE(op.getRecPay(0) == RecPay::Receive);
	}

	SECTION("zero legs throws")
	{
		REQUIRE_THROWS_AS(Operation(1, {}, {}), std::invalid_argument);
	}

	SECTION("rec_pay length mismatch throws")
	{
		REQUIRE_THROWS_AS(
			Operation(1, {makeBulletLeg()}, {RecPay::Receive, RecPay::Pay}),
			std::invalid_argument);
	}

	SECTION("empty leg throws")
	{
		REQUIRE_THROWS_AS(Operation(1, {Leg()}, {RecPay::Receive}), std::invalid_argument);
	}
}

TEST_CASE("Portfolio add and remove")
{
	Portfolio portfolio;
	portfolio.add(makeOperation(1001, makeBulletLeg()));
	REQUIRE(portfolio.size() == 1);

	SECTION("duplicate key throws and leaves portfolio unchanged")
	{
		REQUIRE_THROWS_AS(portfolio.add(makeOperation(1001, makeBulletLeg())), std::invalid_argument);
		REQUIRE(portfolio.size() == 1);
	}

	SECTION("missing key on remove throws")
	{
		REQUIRE_THROWS_AS(portfolio.remove(9999), std::invalid_argument);
		REQUIRE(portfolio.size() == 1);
	}

	SECTION("remove")
	{
		portfolio.remove(1001);
		REQUIRE(portfolio.size() == 0);
	}
}

TEST_CASE("states_at per-leg state")
{
	Portfolio portfolio;
	portfolio.add(makeOperation(1001, makeBulletLeg()));
	portfolio.add(makeOperation(1002, makeCustomAmortLeg()));

	SECTION("mid-period accrual matches FixedRateCashflow and rows are key ordered")
	{
		QCDate t{20, 3, 2026};
		auto state = portfolio.statesAt(t);
		REQUIRE(state.opKey.size() == 2);
		REQUIRE(state.opKey[0] == 1001);
		REQUIRE(state.opKey[1] == 1002);
		REQUIRE(state.legNumber[0] == 1);

		auto expectedBullet = makeBulletLeg().getCashflowAt(0)->accruedInterest(t);
		auto expectedCustom = makeCustomAmortLeg().getCashflowAt(0)->accruedInterest(t);
		REQUIRE(state.accruedInterest[0] == expectedBullet);
		REQUIRE(state.accruedInterest[1] == expectedCustom);
		REQUIRE(state.outstandingNotional[0] == 1000000.0);
		REQUIRE(state.outstandingNotional[1] == 1000000.0);
		REQUIRE(state.totalSettling[0] == 0.0);
		REQUIRE(state.nextFlowDate[0] == QCDate(15, 7, 2026).excelSerial());
		REQUIRE(std::isnan(state.presentValue[0]));
	}

	SECTION("second period uses reduced nominal for custom amortization")
	{
		QCDate t{20, 9, 2026};
		auto state = portfolio.statesAt(t);
		REQUIRE(state.outstandingNotional[1] == 600000.0);
		auto expected = makeCustomAmortLeg().getCashflowAt(1)->accruedInterest(t);
		REQUIRE(state.accruedInterest[1] == expected);
	}

	SECTION("settling flow split at a settlement date")
	{
		QCDate t{15, 7, 2026};
		auto state = portfolio.statesAt(t);
		auto cf = makeCustomAmortLeg().getCashflowAt(0);
		auto total = cf->amount();
		auto interest = cf->accruedInterest(QCDate(15, 7, 2026));
		REQUIRE(state.totalSettling[1] == total);
		REQUIRE(state.interestSettling[1] == interest);
		REQUIRE(state.amortizationSettling[1] == total - interest);
	}

	SECTION("outside leg life accrual and notional are zero")
	{
		auto before = portfolio.statesAt(QCDate(1, 1, 2026));
		REQUIRE(before.accruedInterest[0] == 0.0);
		REQUIRE(before.outstandingNotional[0] == 0.0);
		REQUIRE(before.nextFlowDate[0] == QCDate(15, 7, 2026).excelSerial());

		auto after = portfolio.statesAt(QCDate(1, 1, 2028));
		REQUIRE(after.accruedInterest[0] == 0.0);
		REQUIRE(after.outstandingNotional[0] == 0.0);
		REQUIRE(after.nextFlowDate[0] == 0);
	}
}

TEST_CASE("states_at present value")
{
	Portfolio portfolio;
	auto leg = makeBulletLeg();
	portfolio.add(makeOperation(1001, leg));
	QCDate t{20, 3, 2026};
	auto curve = makeFlatCurve();

	SECTION("matches per-cashflow discounting")
	{
		auto state = portfolio.statesAt(t, {{"CLP", curve}});
		double expected = 0.0;
		for (size_t i = 0; i < leg.size(); ++i)
		{
			auto cf = leg.getCashflowAt(i);
			auto offset = t.dayDiff(cf->date());
			if (offset > 0)
			{
				expected += cf->amount() * curve->getDiscountFactorAt(offset);
			}
		}
		REQUIRE(state.presentValue[0] == Approx(expected).epsilon(1e-12));
	}

	SECTION("missing currency in curve map yields NaN")
	{
		auto state = portfolio.statesAt(t, {{"USD", curve}});
		REQUIRE(std::isnan(state.presentValue[0]));
		REQUIRE(state.accruedInterest[0] > 0.0);
	}

	SECTION("flow settling exactly at t is excluded from PV")
	{
		QCDate settlement{15, 7, 2026};
		auto state = portfolio.statesAt(settlement, {{"CLP", curve}});
		auto lastCf = leg.getCashflowAt(1);
		auto offset = settlement.dayDiff(lastCf->date());
		auto expected = lastCf->amount() * curve->getDiscountFactorAt(offset);
		REQUIRE(state.presentValue[0] == Approx(expected).epsilon(1e-12));
		REQUIRE(state.totalSettling[0] > 0.0);
	}
}

TEST_CASE("flows_between window")
{
	Portfolio portfolio;
	portfolio.add(makeOperation(1001, makeBulletLeg()));

	SECTION("window is half open (t1, t2]")
	{
		QCDate firstSettlement{15, 7, 2026};
		auto flows = portfolio.flowsBetween(firstSettlement, QCDate(15, 1, 2027));
		// Flow at t1 excluded, flow at t2 included.
		REQUIRE(flows.opKey.size() == 1);
		REQUIRE(flows.settlementDate[0] == QCDate(15, 1, 2027).excelSerial());

		auto cf = makeBulletLeg().getCashflowAt(1);
		REQUIRE(flows.total[0] == cf->amount());
		REQUIRE(flows.interest[0] == cf->accruedInterest(QCDate(15, 1, 2027)));
		REQUIRE(flows.amortization[0] == cf->amount() - cf->accruedInterest(QCDate(15, 1, 2027)));
	}

	SECTION("empty window returns zero-length columns")
	{
		auto flows = portfolio.flowsBetween(QCDate(1, 2, 2027), QCDate(1, 3, 2027));
		REQUIRE(flows.opKey.empty());
	}

	SECTION("t1 not before t2 throws")
	{
		REQUIRE_THROWS_AS(
			portfolio.flowsBetween(QCDate(1, 2, 2027), QCDate(1, 2, 2027)),
			std::invalid_argument);
	}
}

TEST_CASE("thread count does not change results")
{
	Portfolio portfolio;
	for (long long key = 1; key <= 200; ++key)
	{
		portfolio.add(makeOperation(key, makeBulletLeg(1000.0 * key, 0.03 + 0.0001 * key)));
	}
	QCDate t{20, 3, 2026};
	auto curve = makeFlatCurve();

	auto one = portfolio.statesAt(t, {{"CLP", curve}}, 1);
	auto many = portfolio.statesAt(t, {{"CLP", curve}}, 8);
	REQUIRE(one.opKey == many.opKey);
	REQUIRE(one.accruedInterest == many.accruedInterest);
	REQUIRE(one.outstandingNotional == many.outstandingNotional);
	REQUIRE(one.presentValue == many.presentValue);
	REQUIRE(one.totalSettling == many.totalSettling);
	REQUIRE(one.nextFlowDate == many.nextFlowDate);

	auto flowsOne = portfolio.flowsBetween(QCDate(1, 2, 2026), QCDate(1, 2, 2027), 1);
	auto flowsMany = portfolio.flowsBetween(QCDate(1, 2, 2026), QCDate(1, 2, 2027), 8);
	REQUIRE(flowsOne.opKey == flowsMany.opKey);
	REQUIRE(flowsOne.settlementDate == flowsMany.settlementDate);
	REQUIRE(flowsOne.total == flowsMany.total);
}
