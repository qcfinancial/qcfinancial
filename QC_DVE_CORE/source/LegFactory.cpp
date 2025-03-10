#include<memory>
#include<algorithm>
#include<tuple>
#include <cashflows/IborCashflow2.h>

#include "LegFactory.h"
#include "cashflows/FixedRateCashflow.h"
#include "cashflows/FixedRateCashflow2.h"
#include "cashflows/FixedRateMultiCurrencyCashflow.h"
#include "cashflows/IborCashflow.h"
#include "cashflows/IborMultiCurrencyCashflow.h"
#include "cashflows/IcpClpCashflow.h"
#include "cashflows/IcpClpCashflow2.h"
#include "cashflows/IcpClfCashflow.h"
#include "cashflows/CompoundedOvernightRateCashflow.h"
#include "cashflows/CompoundedOvernightRateCashflow2.h"
#include "cashflows/OvernightIndexCashflow.h"
#include "cashflows/OvernightIndexMultiCurrencyCashflow.h"


#include "QCInterestRatePeriodsFactory.h"

namespace QCode::Financial {
    LegFactory::LegFactory() = default;

    bool LegFactory::isPeriodicityZero(Tenor periodicity) {
        return periodicity.getString() == "0D";
}

    Leg LegFactory::buildBulletFixedRateLeg(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            double notional,
            bool doesAmortize,
            QCInterestRate rate,
            std::shared_ptr<QCCurrency> currency,
            bool forBonds,
            QCDate::QCSettlementLagBehaviour settLagBehaviour) {

        if (isPeriodicityZero(settlementPeriodicity)) {
            throw std::invalid_argument("Settlement periodicity must be different from 0 in at least one dimension");
        }

        auto settlementPeriodicityString = Tenor(settlementPeriodicity).getString();

        // Make all the holidays in the calendar into a shared_ptr.
        auto settCal = std::make_shared<std::vector<QCDate>>(settlementCalendar.getHolidays());

        // Minus sign is set if cashflows are paid.
        int sign;
        if (recPay == Receive) {
            sign = 1;
        } else {
            sign = -1;
        }

        // Instantiate factory and build the corresponding periods.
        QCInterestRatePeriodsFactory pf{
            startDate,
            endDate,
            endDateAdjustment,
            settlementPeriodicityString,
            settlementStubPeriod,
            settCal,
            settlementLag,
            // The next parameters are useful only for IborLegs. Arbitrary values
            // are given to them in this case.
            settlementPeriodicityString,
            settlementStubPeriod,
            settCal,
            0,
            0,
            settlementPeriodicityString,
            // New Parameters (2024)
            settLagBehaviour};

        auto periods = pf.getPeriods();

        // Load the periods into the structure of FixedRateCashflow and construct the Leg.
        Leg fixedRateLeg;
        size_t numPeriods = periods.size();
        fixedRateLeg.resize(numPeriods);
        size_t i = 0;
        for (const auto &period: periods) {
            QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
            QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
            QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
            // For the correct calculation of present values using market yields according
            // to the usual conventions in fixed income markets.
            if (forBonds) settlementDate = thisEndDate;
            double amort = 0.0;
            if (i == numPeriods - 1) {
                amort = sign * notional;
            }
            FixedRateCashflow frc{
                thisStartDate,
                thisEndDate,
                settlementDate,
                sign * notional,
                amort,
                doesAmortize,
                rate,
                currency};
            fixedRateLeg.setCashflowAt(std::make_shared<FixedRateCashflow>(frc), i);
            ++i;
        }

        return fixedRateLeg;
    }

    Leg LegFactory::buildCustomAmortFixedRateLeg(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            CustomNotionalAmort notionalAndAmort,
            bool doesAmortize,
            QCInterestRate rate,
            std::shared_ptr<QCCurrency> currency,
            QCDate::QCSettlementLagBehaviour settLagBehaviour) {

        Leg fixedRateLeg = buildBulletFixedRateLeg(
                recPay,
                startDate,
                endDate,
                endDateAdjustment,
                settlementPeriodicity,
                settlementStubPeriod,
                settlementCalendar,
                settlementLag,
                1.0,
                doesAmortize,
                rate,
                currency,
                false,
                settLagBehaviour);

        customizeAmortization(recPay, fixedRateLeg, notionalAndAmort, LegFactory::fixedRateCashflow);
        return fixedRateLeg;
    }

    Leg LegFactory::buildBulletFixedRateMultiCurrencyLeg(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            double notional,
            bool doesAmortize,
            QCInterestRate rate,
            std::shared_ptr<QCCurrency> notionalCurrency,
            std::shared_ptr<QCCurrency> settlementCurrency,
            std::shared_ptr<FXRateIndex> fxRateIndex,
            unsigned int fxRateIndexFixingLag,
            bool forBonds,
            QCDate::QCSettlementLagBehaviour settLagBehaviour,
            QCDate::QCFxFixingLagPivot fxFixingLagPivot,
            QCDate::QCFxFixingLagAppliesTo fxFixingLagAppliesTo) {

        if (isPeriodicityZero(settlementPeriodicity)) {
            throw std::invalid_argument("Settlement periodicity must be different from 0 in at least one dimension");
        }
        auto settlementPeriodicityString = Tenor(settlementPeriodicity).getString();

        // Make all the holidays in the calendar into a shared_ptr.
        auto settCal = std::make_shared<std::vector<QCDate>>(settlementCalendar.getHolidays());

        // Minus sign is set if cashflows are paid.
        int sign;
        if (recPay == Receive) {
            sign = 1;
        } else {
            sign = -1;
        }

        // Instantiate factory and build the corresponding periods.
        QCInterestRatePeriodsFactory pf{
            startDate,
            endDate,
            endDateAdjustment,
            settlementPeriodicityString,
            settlementStubPeriod,
            settCal,
            settlementLag,
            // The next parameters are useful only for IborLegs. Arbitrary values
            // are given to them in this case.
            settlementPeriodicityString,
            settlementStubPeriod,
            settCal,
            0,
            0,
            settlementPeriodicityString,
            settLagBehaviour};

        auto periods = pf.getPeriods();

        // Load the periods into the structure of FixedRateCashflow and construct the Leg.
        Leg fixedRateMultiCurrencyLeg;
        size_t numPeriods = periods.size();
        fixedRateMultiCurrencyLeg.resize(numPeriods);
        size_t i = 0;
        for (const auto &period: periods) {
            QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
            QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
            QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);

            // Calculation of fx rate fixing date according to selected pivot date.
            QCDate fxRateIndexFixingDate;
            if (fxFixingLagPivot == QCDate::QCFxFixingLagPivot::qcSettlementDate) {
                fxRateIndexFixingDate =settlementCalendar.shift(settlementDate, -int(fxRateIndexFixingLag));
            } else {
                fxRateIndexFixingDate =settlementCalendar.shift(thisEndDate, -int(fxRateIndexFixingLag));
            }
            if (fxFixingLagAppliesTo == QCDate::QCFxFixingLagAppliesTo::qcFixingDate) {
                fxRateIndexFixingDate = fxRateIndex->publishingDate(fxRateIndexFixingDate);
            }

            // For the correct calculation of present values using market yields according
            // to the usual conventions in fixed income markets.
            if (forBonds) settlementDate = thisEndDate;
            double amort = 0.0;
            if (i == numPeriods - 1) {
                amort = sign * notional;
            }
            FixedRateMultiCurrencyCashflow frmcc{
                thisStartDate,
                thisEndDate,
                settlementDate,
                sign * notional,
                amort,
                doesAmortize,
                rate,
                notionalCurrency,
                fxRateIndexFixingDate,
                settlementCurrency,
                fxRateIndex,
                DEFAULT_FX_RATE_INDEX_VALUE};

            fixedRateMultiCurrencyLeg.setCashflowAt(std::make_shared<FixedRateMultiCurrencyCashflow>(frmcc), i);
            ++i;
        }

        return fixedRateMultiCurrencyLeg;
    }

    Leg LegFactory::buildCustomAmortFixedRateMultiCurrencyLeg(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            CustomNotionalAmort notionalAndAmort,
            bool doesAmortize,
            QCInterestRate rate,
            std::shared_ptr<QCCurrency> notionalCurrency,
            std::shared_ptr<QCCurrency> settlementCurrency,
            std::shared_ptr<FXRateIndex> fxRateIndex,
            unsigned int fxRateIndexFixingLag,
            bool forBonds,
            QCDate::QCSettlementLagBehaviour settLagBehaviour,
            QCDate::QCFxFixingLagPivot fxFixingLagPivot,
            QCDate::QCFxFixingLagAppliesTo fxFixingLagAppliesTo) {
        Leg fixedRateMccyLeg = buildBulletFixedRateMultiCurrencyLeg(
                recPay,
                startDate,
                endDate,
                endDateAdjustment,
                settlementPeriodicity,
                settlementStubPeriod,
                settlementCalendar,
                settlementLag,
                1.0,
                doesAmortize,
                rate,
                notionalCurrency,
                settlementCurrency,
                fxRateIndex,
                fxRateIndexFixingLag,
                forBonds,
                settLagBehaviour,
                fxFixingLagPivot,
                fxFixingLagAppliesTo);

        customizeAmortization(recPay, fixedRateMccyLeg, notionalAndAmort,
                              LegFactory::fixedRateMultiCurrencyCashflow);
        return fixedRateMccyLeg;
    }

    Leg LegFactory::buildBulletIborLeg(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            Tenor fixingPeriodicity,
            QCInterestRateLeg::QCStubPeriod fixingStubPeriod,
            QCBusinessCalendar fixingCalendar,
            unsigned int fixingLag,
            std::shared_ptr<InterestRateIndex> index,
            double notional,
            bool doesAmortize,
            std::shared_ptr<QCCurrency> currency,
            double spread,
            double gearing,
            QCDate::QCSettlementLagBehaviour settLagBehaviour) {

        if (isPeriodicityZero(settlementPeriodicity)) {
            throw std::invalid_argument("Settlement periodicity must be different from 0 in at least one dimension");
        }

        if (isPeriodicityZero(fixingPeriodicity)) {
            throw std::invalid_argument("Fixing periodicity must be different from 0 in at least one dimension");
        }

        // Make all the holidays in the calendar into a shared_ptr.
        auto settCal = std::make_shared<std::vector<QCDate>>(settlementCalendar.getHolidays());
        auto fixCal = std::make_shared<std::vector<QCDate>>(fixingCalendar.getHolidays());

        // Minus sign is set if cashflows are paid.
        int sign;
        if (recPay == Receive) {
            sign = 1;
        } else {
            sign = -1;
        }

        //Se da de alta la fabrica de periods
        QCInterestRatePeriodsFactory factory{
            startDate,
            endDate,
            endDateAdjustment,
            settlementPeriodicity.getString(),
            settlementStubPeriod,
            settCal,
            settlementLag,
            fixingPeriodicity.getString(),
            fixingStubPeriod,
            fixCal,
            fixingLag,
            index->getDaysOfStartLag(),
            index->getTenor().getString(),
            settLagBehaviour};

        //Se generan los periodos
        auto periods = factory.getPeriods();

        // Load the periods into the structure of FixedRateCashflow and construct the Leg.
        Leg iborLeg;
        size_t numPeriods = periods.size();
        iborLeg.resize(numPeriods);
        size_t i = 0;
        for (const auto &period: periods) {
            QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
            QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
            QCDate thisFixingDate = get<QCInterestRateLeg::intRtPrdElmntFxngDate>(period);
            QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
            double amort = 0.0;
            if (i == numPeriods - 1) {
                amort = sign * notional;
            }
            IborCashflow frc{
                index,
                thisStartDate,
                thisEndDate,
                thisFixingDate,
                settlementDate,
                sign * notional,
                amort,
                doesAmortize,
                currency,
                spread,
                gearing};

            iborLeg.setCashflowAt(std::make_shared<IborCashflow>(frc), i);
            ++i;
        }

        return iborLeg;

    }

    Leg LegFactory::buildCustomAmortIborLeg(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            CustomNotionalAmort notionalAndAmort,
            Tenor fixingPeriodicity,
            QCInterestRateLeg::QCStubPeriod fixingStubPeriod,
            QCBusinessCalendar fixingCalendar,
            unsigned int fixingLag,
            std::shared_ptr<InterestRateIndex> index,
            bool doesAmortize,
            std::shared_ptr<QCCurrency> currency,
            double spread,
            double gearing,
            QCDate::QCSettlementLagBehaviour settLagBehaviour) {

        // Primero se construye una pata bullet
        Leg iborLeg = buildBulletIborLeg(
                recPay,
                startDate,
                endDate,
                endDateAdjustment,
                settlementPeriodicity,
                settlementStubPeriod,
                settlementCalendar,
                settlementLag,
                fixingPeriodicity,
                fixingStubPeriod,
                fixingCalendar,
                fixingLag,
                index,
                1.0,
                doesAmortize,
                currency,
                spread,
                gearing,
                settLagBehaviour);

        // Luego se customiza la amortización
        customizeAmortization(recPay, iborLeg, notionalAndAmort, LegFactory::iborCashflow);
        return iborLeg;
    }

    Leg LegFactory::buildBulletIborMultiCurrencyLeg(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            Tenor fixingPeriodicity,
            QCInterestRateLeg::QCStubPeriod fixingStubPeriod,
            QCBusinessCalendar fixingCalendar,
            unsigned int fixingLag,
            std::shared_ptr<InterestRateIndex> index,
            double notional,
            bool doesAmortize,
            std::shared_ptr<QCCurrency> notionalCurrency,
            double spread,
            double gearing,
            std::shared_ptr<QCCurrency> settlementCurrency,
            std::shared_ptr<FXRateIndex> fxRateIndex,
            unsigned int fxRateIndexFixingLag,
            QCDate::QCSettlementLagBehaviour settLagBehaviour,
            QCDate::QCFxFixingLagPivot fxFixingLagPivot,
            QCDate::QCFxFixingLagAppliesTo fxFixingLagAppliesTo) {

        if (isPeriodicityZero(settlementPeriodicity)) {
            throw std::invalid_argument("Settlement periodicity must be different from 0 in at least one dimension");
        }

        if (isPeriodicityZero(fixingPeriodicity)) {
            throw std::invalid_argument("Fixing periodicity must be different from 0 in at least one dimension");
        }

        // Make all the holidays in the calendar into a shared_ptr.
        auto settCal = std::make_shared<std::vector<QCDate>>(settlementCalendar.getHolidays());
        auto fixCal = std::make_shared<std::vector<QCDate>>(fixingCalendar.getHolidays());

        // Minus sign is set if cashflows are paid.
        int sign;
        if (recPay == Receive) {
            sign = 1;
        } else {
            sign = -1;
        }

        //Se da de alta la fabrica de periods
        QCInterestRatePeriodsFactory factory{
            startDate,
            endDate,
            endDateAdjustment,
            settlementPeriodicity.getString(),
            settlementStubPeriod,
            settCal,
            settlementLag,
            fixingPeriodicity.getString(),
            fixingStubPeriod,
            fixCal,
            fixingLag,
            index->getDaysOfStartLag(),
            index->getTenor().getString(),
            settLagBehaviour
        };

        //Se construyen los períodos
        auto periods = factory.getPeriods();

        // Load the periods into the structure of IborMultiCurrencyLeg and construct the Leg.
        Leg iborMultiCurrencyLeg;
        size_t numPeriods = periods.size();
        iborMultiCurrencyLeg.resize(numPeriods);
        size_t i = 0;
        for (const auto &period: periods) {
            QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
            QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
            QCDate thisFixingDate = get<QCInterestRateLeg::intRtPrdElmntFxngDate>(period);
            QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
            // Calculation of fx rate fixing date according to selected pivot date.
            QCDate fxRateIndexFixingDate;
            if (fxFixingLagPivot == QCDate::QCFxFixingLagPivot::qcSettlementDate) {
                fxRateIndexFixingDate =settlementCalendar.shift(settlementDate, -int(fxRateIndexFixingLag));
            } else {
                fxRateIndexFixingDate =settlementCalendar.shift(thisEndDate, -int(fxRateIndexFixingLag));
            }
            if (fxFixingLagAppliesTo == QCDate::QCFxFixingLagAppliesTo::qcFixingDate) {
                fxRateIndexFixingDate = fxRateIndex->publishingDate(fxRateIndexFixingDate);
            }

            double amort = 0.0;
            if (i == numPeriods - 1) {
                amort = sign * notional;
            }

            IborMultiCurrencyCashflow imccy{
                index,
                thisStartDate,
                thisEndDate,
                thisFixingDate,
                settlementDate,
                sign * notional,
                amort,
                doesAmortize,
                notionalCurrency,
                spread,
                gearing,
                fxRateIndexFixingDate,
                settlementCurrency, fxRateIndex
            };

            iborMultiCurrencyLeg.setCashflowAt(std::make_shared<IborMultiCurrencyCashflow>(imccy), i);
            ++i;
        }

        return iborMultiCurrencyLeg;

    }

    Leg LegFactory::buildCustomAmortIborMultiCurrencyLeg(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            Tenor fixingPeriodicity,
            QCInterestRateLeg::QCStubPeriod fixingStubPeriod,
            QCBusinessCalendar fixingCalendar,
            unsigned int fixingLag,
            std::shared_ptr<InterestRateIndex> index,
            CustomNotionalAmort notionalAndAmort,
            bool doesAmortize,
            std::shared_ptr<QCCurrency> notionalCurrency,
            double spread,
            double gearing,
            std::shared_ptr<QCCurrency> settlementCurrency,
            std::shared_ptr<FXRateIndex> fxRateIndex,
            unsigned int fxRateIndexFixingLag,
            QCDate::QCSettlementLagBehaviour settLagBehaviour,
            QCDate::QCFxFixingLagPivot fxFixingLagPivot,
            QCDate::QCFxFixingLagAppliesTo fxFixingLagAppliesTo) {

        // Se construye la pata bullet
        auto iborMccyLeg = buildBulletIborMultiCurrencyLeg(
                recPay,
                startDate,
                endDate,
                endDateAdjustment,
                settlementPeriodicity,
                settlementStubPeriod,
                settlementCalendar,
                settlementLag,
                fixingPeriodicity,
                fixingStubPeriod,
                fixingCalendar,
                fixingLag,
                index,
                1.0,
                doesAmortize,
                notionalCurrency,
                spread,
                gearing,
                settlementCurrency,
                fxRateIndex,
                fxRateIndexFixingLag,
                settLagBehaviour,
                fxFixingLagPivot,
                fxFixingLagAppliesTo
        );

        // Se customiza la amortización
        customizeAmortization(
                recPay,
                iborMccyLeg,
                notionalAndAmort,
                LegFactory::iborMultiCurrencyCashflow);

        return iborMccyLeg;
    }

    Leg LegFactory::buildBulletOvernightIndexLeg(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            QCDate::QCBusDayAdjRules indexDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            QCBusinessCalendar fixingCalendar,
            unsigned int settlementLag,
            double notional,
            bool doesAmortize,
            double spread,
            double gearing,
            QCInterestRate rate,
            std::string indexName,
            unsigned int eqRateDecimalPlaces,
            std::shared_ptr<QCCurrency> notionalCurrency,
            DatesForEquivalentRate datesForEquivalentRate,
            QCDate::QCSettlementLagBehaviour settLagBehaviour) {

        if (isPeriodicityZero(settlementPeriodicity)) {
            throw std::invalid_argument("Settlement periodicity must be different from 0 in at least one dimension");
        }

        // Make all the holidays in the calendars into a shared_ptr.
        auto settCal = std::make_shared<std::vector<QCDate>>(
                settlementCalendar.getHolidays());

        auto fixCal = std::make_shared<std::vector<QCDate>>(
                fixingCalendar.getHolidays());


        // Minus sign is set if cashflows are paid.
        int sign;
        if (recPay == Receive) {
            sign = 1;
        } else {
            sign = -1;
        }

        // Instantiate factory and build the corresponding periods.
        QCInterestRatePeriodsFactory pf{
                startDate,
                endDate,
                endDateAdjustment,
                settlementPeriodicity.getString(),
                settlementStubPeriod,
                settCal,
                settlementLag,
                // The next parameters are useful only for IborLegs. Arbitrary values
                // are given to them in this case.
                settlementPeriodicity.getString(),
                settlementStubPeriod,
                settCal,
                0,
                0,
                settlementPeriodicity.getString(),
                settLagBehaviour
        };
        auto periods = pf.getPeriods();

        // Load the periods into the structure of OvernightIndexCashflow and construct the Leg.
        Leg overnightIndexLeg;
        size_t numPeriods = periods.size();
        overnightIndexLeg.resize(numPeriods);
        size_t i = 0;
        for (const auto &period: periods) {
            QCDate accrualStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
            QCDate accrualEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
            QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
            QCDate indexStartDate = accrualStartDate.businessDay(fixCal, indexDateAdjustment);
            QCDate indexEndDate = accrualEndDate.businessDay(fixCal, indexDateAdjustment);
            double amort = 0.0;
            if (i == numPeriods - 1) {
                amort = sign * notional;
            }
            OvernightIndexCashflow overnightIndexCashflow{
                    accrualStartDate,
                    accrualEndDate,
                    indexStartDate,
                    indexEndDate,
                    settlementDate,
                    notionalCurrency,
                    sign * notional,
                    amort,
                    doesAmortize,
                    spread,
                    gearing,
                    rate,
                    indexName,
                    eqRateDecimalPlaces,
                    datesForEquivalentRate
            };
            overnightIndexLeg.setCashflowAt(std::make_shared<OvernightIndexCashflow>(overnightIndexCashflow), i);
            ++i;
        }

        return overnightIndexLeg;


    };

    Leg LegFactory::buildCustomAmortOvernightIndexLeg(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            QCDate::QCBusDayAdjRules indexDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            QCBusinessCalendar fixingCalendar,
            unsigned int settlementLag,
            CustomNotionalAmort notionalAndAmort,
            bool doesAmortize,
            double spread,
            double gearing,
            QCInterestRate rate,
            std::string indexName,
            unsigned int eqRateDecimalPlaces,
            std::shared_ptr<QCCurrency> notionalCurrency,
            DatesForEquivalentRate datesForEquivalentRate,
            QCDate::QCSettlementLagBehaviour settLagBehaviour) {

        // Se construye la pata bullet
        auto bulletLeg = buildBulletOvernightIndexLeg(
                recPay,
                startDate,
                endDate,
                endDateAdjustment,
                indexDateAdjustment,
                settlementPeriodicity,
                settlementStubPeriod,
                settlementCalendar,
                fixingCalendar,
                settlementLag,
                1.0,
                doesAmortize,
                spread,
                gearing,
                rate,
                std::move(indexName), // a ver que pasa
                eqRateDecimalPlaces,
                std::move(notionalCurrency),
                datesForEquivalentRate,
                settLagBehaviour);

        // Se customiza la amortización
        customizeAmortization(
                recPay,
                bulletLeg,
                notionalAndAmort,
                LegFactory::overnightIndexCashflow);

        return bulletLeg;
    };

    Leg LegFactory::buildBulletOvernightIndexMultiCurrencyLeg(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            QCDate::QCBusDayAdjRules indexDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            QCBusinessCalendar fixingCalendar,
            unsigned int settlementLag,
            double notional,
            bool doesAmortize,
            double spread,
            double gearing,
            QCInterestRate rate,
            std::string indexName,
            unsigned int eqRateDecimalPlaces,
            std::shared_ptr<QCCurrency> notionalCurrency,
            DatesForEquivalentRate datesForEquivalentRate,
            std::shared_ptr<QCCurrency> settlementCurrency,
            std::shared_ptr<FXRateIndex> fxRateIndex,
            unsigned int fxRateIndexFixingLag,
            QCDate::QCSettlementLagBehaviour settLagBehaviour,
            QCDate::QCFxFixingLagPivot fxFixingLagPivot,
            QCDate::QCFxFixingLagAppliesTo fxFixingLagAppliesTo) {

        if (isPeriodicityZero(settlementPeriodicity)) {
            throw std::invalid_argument("Settlement periodicity must be different from 0 in at least one dimension");
        }

        // Make all the holidays in the calendars into a shared_ptr.
        auto settCal = std::make_shared<std::vector<QCDate>>(
                settlementCalendar.getHolidays());

        auto fixCal = std::make_shared<std::vector<QCDate>>(
                fixingCalendar.getHolidays());


        // Minus sign is set if cashflows are paid.
        int sign;
        if (recPay == Receive) {
            sign = 1;
        } else {
            sign = -1;
        }

        // Instantiate factory and build the corresponding periods.
        QCInterestRatePeriodsFactory pf{
                startDate,
                endDate,
                endDateAdjustment,
                settlementPeriodicity.getString(),
                settlementStubPeriod,
                settCal,
                settlementLag,
                // The next parameters are useful only for IborLegs. Arbitrary values
                // are given to them in this case.
                settlementPeriodicity.getString(),
                settlementStubPeriod,
                settCal,
                0,
                0,
                settlementPeriodicity.getString(),
                settLagBehaviour
        };

        auto periods = pf.getPeriods();

        // Load the periods into the structure of OvernightIndexCashflow and construct the Leg.
        Leg overnightIndexMccyLeg;
        size_t numPeriods = periods.size();
        overnightIndexMccyLeg.resize(numPeriods);
        size_t i = 0;
        for (const auto &period: periods) {
            QCDate accrualStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
            QCDate accrualEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
            QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
            QCDate indexStartDate = accrualStartDate.businessDay(fixCal, indexDateAdjustment);
            QCDate indexEndDate = accrualEndDate.businessDay(fixCal, indexDateAdjustment);
            QCDate fxRateIndexFixingDate;
            if (fxFixingLagPivot == QCDate::QCFxFixingLagPivot::qcSettlementDate) {
                fxRateIndexFixingDate =settlementCalendar.shift(settlementDate, -int(fxRateIndexFixingLag));
            } else {
                fxRateIndexFixingDate =settlementCalendar.shift(accrualEndDate, -int(fxRateIndexFixingLag));
            }
            if (fxFixingLagAppliesTo == QCDate::QCFxFixingLagAppliesTo::qcFixingDate) {
                fxRateIndexFixingDate = fxRateIndex->publishingDate(fxRateIndexFixingDate);
            }
            double amort = 0.0;
            if (i == numPeriods - 1) {
                amort = sign * notional;
            }
            OvernightIndexMultiCurrencyCashflow overnightIndexMccyCashflow{
                    accrualStartDate,
                    accrualEndDate,
                    indexStartDate,
                    indexEndDate,
                    settlementDate,
                    notionalCurrency,
                    sign * notional,
                    amort,
                    doesAmortize,
                    spread,
                    gearing,
                    rate,
                    indexName,
                    eqRateDecimalPlaces,
                    datesForEquivalentRate,
                    fxRateIndexFixingDate,
                    settlementCurrency,
                    fxRateIndex,
            };
            overnightIndexMccyLeg.setCashflowAt(
                    std::make_shared<OvernightIndexMultiCurrencyCashflow>(overnightIndexMccyCashflow), i);
            ++i;
        }

        return overnightIndexMccyLeg;

    };

    Leg LegFactory::buildCustomAmortOvernightIndexMultiCurrencyLeg(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            QCDate::QCBusDayAdjRules indexDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            QCBusinessCalendar fixingCalendar,
            unsigned int settlementLag,
            CustomNotionalAmort notionalAndAmort,
            bool doesAmortize,
            double spread,
            double gearing,
            QCInterestRate rate,
            std::string indexName,
            unsigned int eqRateDecimalPlaces,
            std::shared_ptr<QCCurrency> notionalCurrency,
            DatesForEquivalentRate datesForEquivalentRate,
            std::shared_ptr<QCCurrency> settlementCurrency,
            std::shared_ptr<FXRateIndex> fxRateIndex,
            unsigned int fxRateIndexFixingLag,
            QCDate::QCSettlementLagBehaviour settLagBehaviour,
            QCDate::QCFxFixingLagPivot fxFixingLagPivot,
            QCDate::QCFxFixingLagAppliesTo fxFixingLagAppliesTo) {

        // Se construye la pata bullet
        auto leg = buildBulletOvernightIndexMultiCurrencyLeg(
                recPay,
                startDate,
                endDate,
                endDateAdjustment,
                indexDateAdjustment,
                settlementPeriodicity,
                settlementStubPeriod,
                settlementCalendar,
                fixingCalendar,
                settlementLag,
                1.0,
                doesAmortize,
                spread,
                gearing,
                rate,
                indexName,
                eqRateDecimalPlaces,
                notionalCurrency,
                datesForEquivalentRate,
                settlementCurrency,
                fxRateIndex,
                fxRateIndexFixingLag,
                settLagBehaviour,
                fxFixingLagPivot,
                fxFixingLagAppliesTo);

        // Se customiza la amortización
        customizeAmortization(
                recPay,
                leg,
                notionalAndAmort,
                LegFactory::overnightIndexMultiCurrencyCashflow);

        return leg;
    }

    Leg LegFactory::buildBulletCompoundedOvernightRateLeg2(
            RecPay recPay,
            const QCDate &startDate,
            const QCDate &endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            QCBusinessCalendar fixingCalendar,
            const shared_ptr<InterestRateIndex> &index,
            double notional,
            bool doesAmortize,
            const shared_ptr<QCCurrency> &currency,
            double spread,
            double gearing,
            const QCInterestRate &interestRate,
            unsigned int eqRateDecimalPlaces,
            unsigned int lookback,
            unsigned int lockout,
            QCDate::QCSettlementLagBehaviour settLagBehaviour) {

        if (isPeriodicityZero(settlementPeriodicity)) {
            throw std::invalid_argument("Settlement periodicity must be different from 0 in at least one dimension");
        }

        // Make all the holidays in the calendar into a shared_ptr.
        auto settCal = std::make_shared<std::vector<QCDate>>(settlementCalendar.getHolidays());
        auto fixCal = std::make_shared<std::vector<QCDate>>(fixingCalendar.getHolidays());

        // Minus sign is set if cashflows are paid.
        int sign;
        if (recPay == Receive) {
            sign = 1;
        } else {
            sign = -1;
        }

        // Se da de alta la fábrica de periods
        QCInterestRatePeriodsFactory factory{
                startDate,
                endDate,
                endDateAdjustment,
                settlementPeriodicity.getString(),
                settlementStubPeriod,
                settCal,
                settlementLag,
                settlementPeriodicity.getString(),
                QCInterestRateLeg::QCStubPeriod::qcShortFront,
                fixCal,
                0,
                index->getDaysOfStartLag(),
                index->getTenor().getString(),
                settLagBehaviour
        };

        //Se generan los periodos
        auto periods = factory.getPeriods();

        // Load the periods into the structure of CompoundOvernightRateCashflow and construct the Leg.
        Leg compoundedOvernightRateLeg2;
        size_t numPeriods = periods.size();
        compoundedOvernightRateLeg2.resize(numPeriods);
        size_t i = 0;
        for (const auto &period: periods) {
            QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
            QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
            QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
            std::vector<QCDate> fixingDates;
            auto fixingDate = thisStartDate;
            while (fixingDate < thisEndDate) {
                fixingDates.emplace_back(fixingDate);
                fixingDate = fixingCalendar.shift(fixingDate, 1);
            }

            /*auto fixingDate = fixingCalendar.shift(thisStartDate, -lookback);
            while (fixingDate < fixingCalendar.shift(thisEndDate, -lookback)) {
                fixingDates.emplace_back(fixingDate);
                fixingDate = fixingCalendar.shift(fixingDate, 1);
            }*/

            double amort = 0.0;
            if (i == numPeriods - 1) {
                amort = sign * notional;
            }
            CompoundedOvernightRateCashflow2 comp{
                    index,
                    thisStartDate,
                    thisEndDate,
                    settlementDate,
                    fixingDates,
                    sign * notional,
                    amort,
                    doesAmortize,
                    currency,
                    spread,
                    gearing,
                    interestRate,
                    eqRateDecimalPlaces,
                    lookback,
                    lockout};

            compoundedOvernightRateLeg2.setCashflowAt(
                    std::make_shared<CompoundedOvernightRateCashflow2>(comp), i);
            ++i;
        }

        return compoundedOvernightRateLeg2;
    }

    Leg LegFactory::buildCustomAmortCompoundedOvernightRateLeg2(
            RecPay recPay,
            const QCDate &startDate,
            const QCDate &endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            QCBusinessCalendar fixingCalendar,
            const shared_ptr<InterestRateIndex> &index,
            CustomNotionalAmort notionalAndAmort,
            bool doesAmortize,
            const shared_ptr<QCCurrency> &currency,
            double spread,
            double gearing,
            const QCInterestRate &interestRate,
            unsigned int eqRateDecimalPlaces,
            unsigned int lookback,
            unsigned int lockout,
            QCDate::QCSettlementLagBehaviour settLagBehaviour) {

        // Se construye la pata bullet
        auto leg = buildBulletCompoundedOvernightRateLeg2(
                recPay,
                startDate,
                endDate,
                endDateAdjustment,
                settlementPeriodicity,
                settlementStubPeriod,
                std::move(settlementCalendar),
                settlementLag,
                std::move(fixingCalendar),
                index,
                1.0,
                doesAmortize,
                currency,
                spread,
                gearing,
                interestRate,
                eqRateDecimalPlaces,
                lookback,
                lockout,
                settLagBehaviour);

        // Se customiza la amortización
        customizeAmortization(
                recPay,
                leg,
                notionalAndAmort,
                LegFactory::compoundedOvernightRateCashflow2);

        return leg;

    }

    Leg LegFactory::buildBulletCompoundedOvernightRateMultiCurrencyLeg2(
            RecPay recPay,
            const QCDate& startDate,
            const QCDate& endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            QCBusinessCalendar fixingCalendar,
            const shared_ptr<InterestRateIndex>& index,
            double notional,
            bool doesAmortize,
            const shared_ptr<QCCurrency>& currency,
            double spread,
            double gearing,
            const QCInterestRate& interestRate,
            unsigned int eqRateDecimalPlaces,
            unsigned int lookback,
            unsigned int lockout,
            unsigned int &fxRateIndexFixingLag,
            std::shared_ptr<QCCurrency> settlementCurrency,
            std::shared_ptr<FXRateIndex> fxRateIndex,
            QCDate::QCSettlementLagBehaviour settLagBehaviour,
            QCDate::QCFxFixingLagPivot fxFixingLagPivot,
            QCDate::QCFxFixingLagAppliesTo fxFixingLagAppliesTo) {

        if (isPeriodicityZero(settlementPeriodicity)) {
            throw std::invalid_argument("Settlement periodicity must be different from 0 in at least one dimension");
        }

        // Make all the holidays in the calendar into a shared_ptr.
        auto settCal = std::make_shared<std::vector<QCDate>>(settlementCalendar.getHolidays());
        auto fixCal = std::make_shared<std::vector<QCDate>>(fixingCalendar.getHolidays());

        // Minus sign is set if cashflows are paid.
        int sign;
        if (recPay == Receive) {
            sign = 1;
        } else {
            sign = -1;
        }

        // Se da de alta la fábrica de periods
        QCInterestRatePeriodsFactory factory{
                startDate,
                endDate,
                endDateAdjustment,
                settlementPeriodicity.getString(),
                settlementStubPeriod,
                settCal,
                settlementLag,
                settlementPeriodicity.getString(),
                QCInterestRateLeg::QCStubPeriod::qcShortFront,
                fixCal,
                0,
                index->getDaysOfStartLag(),
                index->getTenor().getString(),
                settLagBehaviour
        };

        //Se generan los periodos
        auto periods = factory.getPeriods();

        // Load the periods into the structure of CompoundOvernightRateCashflow and construct the Leg.
        Leg compoundedOvernightRateLeg2;
        size_t numPeriods = periods.size();
        compoundedOvernightRateLeg2.resize(numPeriods);
        size_t i = 0;
        for (const auto &period: periods) {
            QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
            QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
            QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
            QCDate fxRateIndexFixingDate;
            if (fxFixingLagPivot == QCDate::QCFxFixingLagPivot::qcSettlementDate) {
                fxRateIndexFixingDate =settlementCalendar.shift(settlementDate, -int(fxRateIndexFixingLag));
            } else {
                fxRateIndexFixingDate =settlementCalendar.shift(thisEndDate, -int(fxRateIndexFixingLag));
            }
            if (fxFixingLagAppliesTo == QCDate::QCFxFixingLagAppliesTo::qcFixingDate) {
                fxRateIndexFixingDate = fxRateIndex->publishingDate(fxRateIndexFixingDate);
            }
            std::vector<QCDate> fixingDates;
            auto fixingDate = thisStartDate;
            while (fixingDate < thisEndDate) {
                fixingDates.emplace_back(fixingDate);
                fixingDate = fixingCalendar.shift(fixingDate, 1);
            }
            double amort = 0.0;
            if (i == numPeriods - 1) {
                amort = sign * notional;
            }
            CompoundedOvernightRateMultiCurrencyCashflow2 comp{
                    index,
                    thisStartDate,
                    thisEndDate,
                    settlementDate,
                    fixingDates,
                    sign * notional,
                    amort,
                    doesAmortize,
                    currency,
                    spread,
                    gearing,
                    interestRate,
                    eqRateDecimalPlaces,
                    lookback,
                    lockout,
                    fxRateIndexFixingDate,
                    settlementCurrency,
                    fxRateIndex};

            compoundedOvernightRateLeg2.setCashflowAt(
                    std::make_shared<CompoundedOvernightRateMultiCurrencyCashflow2>(comp), i);
            ++i;
        }

        return compoundedOvernightRateLeg2;

    }

    Leg LegFactory::buildCustomAmortCompoundedOvernightRateMultiCurrencyLeg2(
            RecPay recPay,
            const QCDate& startDate,
            const QCDate& endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            QCBusinessCalendar fixingCalendar,
            const shared_ptr<InterestRateIndex>& index,
            CustomNotionalAmort notionalAndAmort,
            bool doesAmortize,
            const shared_ptr<QCCurrency>& currency,
            double spread,
            double gearing,
            const QCInterestRate& interestRate,
            unsigned int eqRateDecimalPlaces,
            unsigned int lookback,
            unsigned int lockout,
            unsigned int &fxRateIndexFixingLag,
            std::shared_ptr<QCCurrency> settlementCurrency,
            std::shared_ptr<FXRateIndex> fxRateIndex,
            QCDate::QCSettlementLagBehaviour settLagBehaviour,
            QCDate::QCFxFixingLagPivot fxFixingLagPivot,
            QCDate::QCFxFixingLagAppliesTo fxFixingLagAppliesTo) {

        // Se construye la pata bullet
        auto leg = buildBulletCompoundedOvernightRateMultiCurrencyLeg2(
                recPay,
                startDate,
                endDate,
                endDateAdjustment,
                settlementPeriodicity,
                settlementStubPeriod,
                std::move(settlementCalendar),
                settlementLag,
                std::move(fixingCalendar),
                index,
                1.0,
                doesAmortize,
                currency,
                spread,
                gearing,
                interestRate,
                eqRateDecimalPlaces,
                lookback,
                lockout,
                fxRateIndexFixingLag,
                settlementCurrency,
                fxRateIndex,
                settLagBehaviour,
                fxFixingLagPivot,
                fxFixingLagAppliesTo);

        // Se customiza la amortización
        customizeAmortization(
                recPay,
                leg,
                notionalAndAmort,
                LegFactory::compoundedOvernightRateCashflow2);

        return leg;}

    Leg LegFactory::buildBulletIcpClfLeg(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            double notional,
            bool doesAmortize,
            double spread,
            double gearing) {

        if (isPeriodicityZero(settlementPeriodicity)) {
            throw std::invalid_argument("Settlement periodicity must be different from 0 in at least one dimension");
        }

        // Make all the holidays in the calendar into a shared_ptr.
        auto settCal = std::make_shared<std::vector<QCDate>>(settlementCalendar.getHolidays());

        // Minus sign is set if cashflows are paid.
        int sign;
        if (recPay == Receive) {
            sign = 1;
        } else {
            sign = -1;
        }

        // Instantiate factory and build the corresponding periods.
        QCInterestRatePeriodsFactory pf{
            startDate,
            endDate,
            endDateAdjustment,
            settlementPeriodicity.getString(),
            settlementStubPeriod,
            settCal,
            settlementLag,
            // The next parameters are useful only for IborLegs. Arbitrary values
            // are given to them in this case.
            settlementPeriodicity.getString(),
            settlementStubPeriod,
            settCal,
            0,
            0,
            settlementPeriodicity.getString()
        };

        auto periods = pf.getPeriods();

        // Load the periods into the structure of FixedRateCashflow and contruct the Leg.
        Leg icpClfLeg;
        size_t numPeriods = periods.size();
        icpClfLeg.resize(numPeriods);
        vector<double> defaults{DEFAULT_ICP, DEFAULT_ICP, DEFAULT_UF, DEFAULT_UF};
        size_t i = 0;
        for (const auto &period: periods) {
            QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
            QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
            QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
            double amort = 0.0;
            if (i == numPeriods - 1) {
                amort = sign * notional;
            }
            IcpClfCashflow icpclfc{thisStartDate, thisEndDate, settlementDate,
                                   sign * notional, amort, doesAmortize, spread, gearing, defaults};
            icpClfLeg.setCashflowAt(std::make_shared<IcpClfCashflow>(icpclfc), i);
            ++i;
        }

        return icpClfLeg;
    }

    Leg LegFactory::buildCustomAmortIcpClfLeg(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            CustomNotionalAmort notionalAndAmort,
            bool doesAmortize,
            double spread,
            double gearing) {

        // Se construye la pata bullet
        Leg icpClfLeg = buildBulletIcpClfLeg(
                recPay,
                startDate,
                endDate,
                endDateAdjustment,
                settlementPeriodicity,
                settlementStubPeriod,
                settlementCalendar,
                settlementLag,
                100.0,
                doesAmortize,
                spread,
                gearing);

        // Se customiza la amortización
        customizeAmortization(
                recPay,
                icpClfLeg,
                notionalAndAmort,
                LegFactory::icpClfCashflow);

        return icpClfLeg;
    }

    // #############################################################################
    // De aquí para abajo está la totalidad deprecated
    // #############################################################################

    Leg LegFactory::buildBulletFixedRateLeg2(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            double notional,
            bool doesAmortize,
            QCInterestRate rate,
            std::shared_ptr<QCCurrency> currency,
            bool forBonds) {
        auto settlementPeriodicityString = Tenor(settlementPeriodicity).getString();
        // Make all the holidays in the calendar into a shared_ptr.
        auto settCal = std::make_shared<std::vector<QCDate>>(settlementCalendar.getHolidays());

        // Minus sign is set if cashflows are paid.
        int sign;
        if (recPay == Receive) {
            sign = 1;
        } else {
            sign = -1;
        }

        // Instantiate factory and build the corresponding periods.
        QCInterestRatePeriodsFactory pf{startDate, endDate, endDateAdjustment,
                                        settlementPeriodicityString, settlementStubPeriod, settCal, settlementLag,
                // The next parameters are useful only for IborLegs. Arbitrary values
                // are given to them in this case.
                                        settlementPeriodicityString, settlementStubPeriod, settCal, 0, 0,
                                        settlementPeriodicityString};
        auto periods = pf.getPeriods();

        // Load the periods into the structure of FixedRateCashflow and construct the Leg.
        Leg fixedRateleg;
        size_t numPeriods = periods.size();
        fixedRateleg.resize(numPeriods);
        size_t i = 0;
        for (const auto &period: periods) {
            QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
            QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
            QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
            // For the correct calculation of present values using market yields according
            // to the usual conventions in fixed income markets.
            if (forBonds) settlementDate = thisEndDate;
            double amort = 0.0;
            if (i == numPeriods - 1) {
                amort = sign * notional;
            }
            FixedRateCashflow2 frc{thisStartDate, thisEndDate, settlementDate,
                                   sign * notional, amort, doesAmortize, rate, currency};
            fixedRateleg.setCashflowAt(std::make_shared<FixedRateCashflow2>(frc), i);
            ++i;
        }

        return fixedRateleg;
    }

    Leg LegFactory::buildFrenchFixedRateLeg2(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            double notional,
            bool doesAmortize,
            QCInterestRate rate,
            std::shared_ptr<QCCurrency> currency,
            bool forBonds) {
        auto settlementPeriodicityString = Tenor(settlementPeriodicity).getString();
        // Make all the holidays in the calendar into a shared_ptr.
        auto settCal = std::make_shared<std::vector<QCDate>>(settlementCalendar.getHolidays());

        // Minus sign is set if cashflows are paid.
        int sign;
        if (recPay == Receive) {
            sign = 1;
        } else {
            sign = -1;
        }

        // Instantiate factory and build the corresponding periods.
        QCInterestRatePeriodsFactory pf{startDate, endDate, endDateAdjustment,
                                        settlementPeriodicityString, settlementStubPeriod,
                                        settCal, settlementLag,
                // The next parameters are useful only for IborLegs. Arbitrary values
                // are given to them in this case.
                                        settlementPeriodicityString, settlementStubPeriod,
                                        settCal, 0, 0, settlementPeriodicityString};
        auto periods = pf.getPeriods();

        //Con esas fechas residuales y el notional calcular la cuota
        auto periodsCuota = periods.size();
        double r = 1 / (1 + rate.getValue() / 12.0);
        double formula = r * (1 - pow(r, periodsCuota)) / (1 - r);
        double cuota = notional / formula;

        // Load the periods into the structure of FixedRateCashflow and construct the Leg.
        Leg fixedRateleg;
        size_t numPeriods = periods.size();
        fixedRateleg.resize(numPeriods);
        size_t i = 0;
        auto tempNotional = notional;
        r = rate.getValue() / 12.0;
        // std::cout << "rate description: " << rate.description() << std::endl;
        for (const auto &period: periods) {
            QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
            QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
            QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
            // For the correct calculation of present values using market yields according
            // to the usual conventions in fixed income markets.
            if (forBonds) settlementDate = thisEndDate;
            double amort = cuota - tempNotional * r;
            FixedRateCashflow2 frc{thisStartDate, thisEndDate, settlementDate,
                                   sign * tempNotional, amort, doesAmortize, rate, currency};
            fixedRateleg.setCashflowAt(std::make_shared<FixedRateCashflow2>(frc), i);
            ++i;
            tempNotional -= amort;
        }

        return fixedRateleg;
    }

    Leg LegFactory::makeLoan(
            double monto,
            int plazo,
            double tasa,
            std::string fechaInicio) {
        // Se da de alta los parámetros requeridos
        auto rp = RecPay::Receive;
        auto qcFechaInicio = QCDate{fechaInicio};
        auto qcFechaFinal = qcFechaInicio.addMonths(plazo * 12);
        auto busAdjmntRule = QCDate::QCBusDayAdjRules::qcNo;
        auto periodicidad = Tenor{"1M"};

        // Con período irregular las cuotas no quedan iguales
        auto periodoIrregular = QCInterestRateLeg::QCStubPeriod::qcNoStubPeriod;

        auto calendario = QCBusinessCalendar{qcFechaInicio, 20};
        calendario.addHoliday(QCDate(31, 12, 2020));
        unsigned int lagPago = 0;
        auto nominal = monto;
        auto amortEsFlujo = true;

        // Si la tasa es Act/360 o Act/365 las cuotas no van a quedar iguales
        QCYrFrctnShrdPtr yf = std::make_shared<QC30360>(QC30360());
        QCWlthFctrShrdPtr wf = std::make_shared<QCLinearWf>(QCLinearWf());
        QCInterestRate tasaCupon{tasa, yf, wf};

        auto moneda = std::make_shared<QCCurrency>(QCCLF());
        auto esBono = false;

        // Se da de alta el objeto
        auto frenchFixedRateLeg2 = buildFrenchFixedRateLeg2(
                rp,
                qcFechaInicio,
                qcFechaFinal,
                busAdjmntRule,
                periodicidad,
                periodoIrregular,
                calendario,
                lagPago,
                nominal,
                amortEsFlujo,
                tasaCupon,
                moneda,
                esBono);

        return frenchFixedRateLeg2;
    }

    std::vector<std::tuple<double, Leg>> LegFactory::buildCae(
            std::string fechaCalculo,
            int ultimoGiro,
            double giroPromedio,
            double saldo,
            int girosTotales,
            int plazoCredito,
            std::string fechaProximoGiro,
            double tasa,
            const std::vector<double> &probabilidades) {
        auto girosAdicionales = girosTotales - ultimoGiro;
        std::vector<std::tuple<double, Leg>> creditos;
        auto fechaHoy = QCDate(fechaCalculo);
        auto qcFechaProximoGiro = QCDate(fechaProximoGiro);
        double probabilidadAcumulada{0.0};
        for (size_t i = 0; i < girosAdicionales + 1; ++i) {
            saldo *= 1 + tasa * fechaHoy.dayDiff(qcFechaProximoGiro) / 360.0;
            auto qcFechaInicio = qcFechaProximoGiro.addMonths(24);
            auto monto = saldo * (1 + tasa * qcFechaProximoGiro.dayDiff(qcFechaInicio) / 360.0);
            if (i < girosAdicionales) {
                creditos.push_back(
                        std::make_tuple(probabilidades[ultimoGiro + 1],
                                        makeLoan(monto, plazoCredito, tasa, qcFechaInicio.description(false))
                        )
                );
                probabilidadAcumulada += probabilidades.at(ultimoGiro + 1);
            } else {
                creditos.push_back(
                        std::make_tuple(
                                1 - probabilidadAcumulada,
                                makeLoan(monto, plazoCredito, tasa, qcFechaInicio.description(false))
                        ));
                fechaHoy = qcFechaProximoGiro;
                qcFechaProximoGiro = qcFechaProximoGiro.addMonths(24);
                saldo += giroPromedio;
            }
        }

        return creditos;
    }

    manyCae LegFactory::buildBulkCae(const dataManyCae &data,
                                     const std::vector<double> &probabilidades) {
        manyCae result;
        for (const auto &dato: data) {
            result[std::get<0>(dato)] = buildCae(
                    std::get<1>(dato),
                    std::get<2>(dato),
                    std::get<3>(dato),
                    std::get<4>(dato),
                    std::get<5>(dato),
                    std::get<6>(dato),
                    std::get<7>(dato),
                    std::get<8>(dato),
                    probabilidades);
        }
        return result;
    }


    Leg LegFactory::buildCustomAmortFixedRateLeg2(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            CustomNotionalAmort notionalAndAmort,
            bool doesAmortize,
            QCInterestRate rate,
            std::shared_ptr<QCCurrency> currency) {
        Leg fixedRateLeg = buildBulletFixedRateLeg2(
                recPay,
                startDate,
                endDate,
                endDateAdjustment,
                settlementPeriodicity,
                settlementStubPeriod,
                settlementCalendar,
                settlementLag,
                1.0,
                doesAmortize,
                rate,
                currency);

        customizeAmortization(recPay, fixedRateLeg, notionalAndAmort, LegFactory::fixedRateCashflow2);
        return fixedRateLeg;
    }

    Leg LegFactory::buildBulletIbor2Leg(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            Tenor fixingPeriodicity,
            QCInterestRateLeg::QCStubPeriod fixingStubPeriod,
            QCBusinessCalendar fixingCalendar,
            unsigned int fixingLag,
            std::shared_ptr<InterestRateIndex> index,
            double notional,
            bool doesAmortize,
            std::shared_ptr<QCCurrency> currency,
            double spread,
            double gearing) {
        // Make all the holidays in the calendar into a shared_ptr.
        auto settCal = std::make_shared<std::vector<QCDate>>(settlementCalendar.getHolidays());
        auto fixCal = std::make_shared<std::vector<QCDate>>(fixingCalendar.getHolidays());

        // Minus sign is set if cashflows are paid.
        int sign;
        if (recPay == Receive) {
            sign = 1;
        } else {
            sign = -1;
        }

        //Se da de alta la fabrica de periods
        QCInterestRatePeriodsFactory factory{startDate, endDate,
                                             endDateAdjustment,
                                             settlementPeriodicity.getString(),
                                             settlementStubPeriod,
                                             settCal,
                                             settlementLag,
                                             fixingPeriodicity.getString(),
                                             fixingStubPeriod,
                                             fixCal,
                                             fixingLag,
                                             index->getDaysOfStartLag(),
                                             index->getTenor().getString()};

        //Se generan los periodos
        auto periods = factory.getPeriods();

        // Load the periods into the structure of FixedRateCashflow and contruct the Leg.
        Leg iborLeg;
        size_t numPeriods = periods.size();
        iborLeg.resize(numPeriods);
        size_t i = 0;
        for (const auto &period: periods) {
            QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
            QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
            QCDate thisFixingDate = get<QCInterestRateLeg::intRtPrdElmntFxngDate>(period);
            QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
            double amort = 0.0;
            if (i == numPeriods - 1) {
                amort = sign * notional;
            }
            IborCashflow2 frc{index, thisStartDate, thisEndDate, thisFixingDate, settlementDate,
                              sign * notional, amort, doesAmortize, currency, spread, gearing};
            iborLeg.setCashflowAt(std::make_shared<IborCashflow2>(frc), i);
            ++i;
        }

        return iborLeg;
    }



    Leg LegFactory::buildCustomAmortIbor2Leg(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            CustomNotionalAmort notionalAndAmort,
            Tenor fixingPeriodicity,
            QCInterestRateLeg::QCStubPeriod fixingStubPeriod,
            QCBusinessCalendar fixingCalendar,
            unsigned int fixingLag,
            std::shared_ptr<InterestRateIndex> index,
            bool doesAmortize,
            std::shared_ptr<QCCurrency> currency,
            double spread,
            double gearing) {
        // Primero construir una pata bullet
        Leg iborLeg = buildBulletIbor2Leg(
                recPay,
                startDate,
                endDate,
                endDateAdjustment,
                settlementPeriodicity,
                settlementStubPeriod,
                settlementCalendar,
                settlementLag,
                fixingPeriodicity,
                fixingStubPeriod,
                fixingCalendar,
                fixingLag,
                index,
                1.0,
                doesAmortize,
                currency,
                spread,
                gearing);

        // std::cout << "custom amort ibor leg: done bullet" << std::endl;
        customizeAmortization(
                recPay,
                iborLeg,
                notionalAndAmort,
                LegFactory::iborCashflow2);

        return iborLeg;
    }

    void LegFactory::customizeAmortization(
            RecPay recPay,
            Leg &leg,
            CustomNotionalAmort notionalAndAmort,
            TypeOfCashflow typeOfCashflow) {
        // Ahora modificar nominal y amortizacion en el objeto Leg recién construido.
        size_t notionalAndAmortSize = notionalAndAmort.getSize();
        size_t legSize = leg.size();
        if (legSize == 0) {
            throw invalid_argument("LegFactory::customizeAmortization. Leg has 0 cashflows");
        }
        size_t minSize = std::min(notionalAndAmortSize, legSize);
        // std::cout << "legSize: " << legSize << std::endl;
        // std::cout << "notionalAndAmortSize: " << notionalAndAmortSize << std::endl;

        int sign;
        if (recPay == Receive) {
            sign = 1;
        } else {
            sign = -1;
        }

        if (typeOfCashflow == LegFactory::fixedRateCashflow) {
            for (size_t i = 0; i < minSize; ++i) {
                std::dynamic_pointer_cast<FixedRateCashflow>(leg.getCashflowAt(legSize - 1 - i))
                        ->setNominal(sign * std::get<0>(
                                notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
                std::dynamic_pointer_cast<FixedRateCashflow>(leg.getCashflowAt(legSize - 1 - i))
                        ->setAmortization(sign * std::get<1>(
                                notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
            }
        }

        if (typeOfCashflow == LegFactory::fixedRateMultiCurrencyCashflow) {
            for (size_t i = 0; i < minSize; ++i) {
                std::dynamic_pointer_cast<FixedRateMultiCurrencyCashflow>(leg.getCashflowAt(legSize - 1 - i))
                        ->setNominal(sign * std::get<0>(
                                notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
                std::dynamic_pointer_cast<FixedRateMultiCurrencyCashflow>(leg.getCashflowAt(legSize - 1 - i))
                        ->setAmortization(sign * std::get<1>(
                                notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
            }
        }

        if (typeOfCashflow == LegFactory::fixedRateCashflow2) {
            for (size_t i = 0; i < minSize; ++i) {
                auto c = std::dynamic_pointer_cast<FixedRateCashflow2>(leg.getCashflowAt(legSize - 1 - i));
                auto nominal =
                        sign * std::get<0>(notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]);
                auto amort = sign * std::get<1>(notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]);
                auto newC = FixedRateCashflow2(c->getStartDate(), c->getEndDate(), c->getSettlementDate(), nominal,
                                               amort,
                                               c->doesAmortize(), std::get<8>(*(c->wrap())), c->ccy());
                leg.setCashflowAt(std::make_shared<FixedRateCashflow2>(newC), legSize - 1 - i);
            }
        }

        if (typeOfCashflow == LegFactory::iborCashflow) {
            for (size_t i = 0; i < minSize; ++i) {
                std::dynamic_pointer_cast<IborCashflow>(leg.getCashflowAt(legSize - 1 - i))
                        ->setNominal(sign * std::get<0>(
                                notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
                std::dynamic_pointer_cast<IborCashflow>(leg.getCashflowAt(legSize - 1 - i))
                        ->setAmortization(sign * std::get<1>(
                                notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
            }
        }

        if (typeOfCashflow == LegFactory::iborCashflow2) {
            for (size_t i = 0; i < minSize; ++i) {
                std::dynamic_pointer_cast<IborCashflow2>(leg.getCashflowAt(legSize - 1 - i))
                        ->setNominal(sign * std::get<0>(
                                notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
                std::dynamic_pointer_cast<IborCashflow2>(leg.getCashflowAt(legSize - 1 - i))
                        ->setAmortization(sign * std::get<1>(
                                notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
            }
        }

        if (typeOfCashflow == LegFactory::icpClpCashflow) {
            for (size_t i = 0; i < minSize; ++i) {
                std::dynamic_pointer_cast<IcpClpCashflow>(leg.getCashflowAt(legSize - 1 - i))
                        ->setNominal(sign * std::get<0>(
                                notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
                std::dynamic_pointer_cast<IcpClpCashflow>(leg.getCashflowAt(legSize - 1 - i))
                        ->setAmortization(sign * std::get<1>(
                                notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
            }
        }

        if (typeOfCashflow == LegFactory::icpClpCashflow2) {
            for (size_t i = 0; i < minSize; ++i) {
                std::dynamic_pointer_cast<IcpClpCashflow2>(leg.getCashflowAt(legSize - 1 - i))
                        ->setNominal(sign * std::get<0>(
                                notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
                std::dynamic_pointer_cast<IcpClpCashflow2>(leg.getCashflowAt(legSize - 1 - i))
                        ->setAmortization(sign * std::get<1>(
                                notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
            }
        }

        if (typeOfCashflow == LegFactory::icpClfCashflow) {
            for (size_t i = 0; i < minSize; ++i) {
                std::dynamic_pointer_cast<IcpClfCashflow>(leg.getCashflowAt(legSize - 1 - i))
                        ->setNominal(sign * std::get<0>(
                                notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
                std::dynamic_pointer_cast<IcpClfCashflow>(leg.getCashflowAt(legSize - 1 - i))
                        ->setAmortization(sign * std::get<1>(
                                notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
            }
        }

        if (typeOfCashflow == LegFactory::iborMultiCurrencyCashflow) {
            for (size_t i = 0; i < minSize; ++i) {
                std::dynamic_pointer_cast<IborMultiCurrencyCashflow>(leg.getCashflowAt(legSize - 1 - i))
                        ->setNominal(sign * std::get<0>(
                                notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
                std::dynamic_pointer_cast<IborMultiCurrencyCashflow>(leg.getCashflowAt(legSize - 1 - i))
                        ->setAmortization(sign * std::get<1>(
                                notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
            }
        }

        if (typeOfCashflow == LegFactory::overnightIndexCashflow) {
            for (size_t i = 0; i < minSize; ++i) {
                std::dynamic_pointer_cast<OvernightIndexCashflow>(leg.getCashflowAt(legSize - 1 - i))
                        ->setNotional(sign * std::get<0>(
                                notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
                std::dynamic_pointer_cast<OvernightIndexCashflow>(leg.getCashflowAt(legSize - 1 - i))
                        ->setAmortization(sign * std::get<1>(
                                notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
            }
        }

        if (typeOfCashflow == LegFactory::overnightIndexMultiCurrencyCashflow) {
            for (size_t i = 0; i < minSize; ++i) {
                std::dynamic_pointer_cast<OvernightIndexMultiCurrencyCashflow>(leg.getCashflowAt(legSize - 1 - i))
                        ->setNotional(sign * std::get<0>(
                                notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
                std::dynamic_pointer_cast<OvernightIndexMultiCurrencyCashflow>(leg.getCashflowAt(legSize - 1 - i))
                        ->setAmortization(sign * std::get<1>(
                                notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
            }
        }

        if (typeOfCashflow == LegFactory::compoundedOvernightRateCashflow2) {
            for (size_t i = 0; i < minSize; ++i) {
                std::dynamic_pointer_cast<CompoundedOvernightRateCashflow2>(leg.getCashflowAt(legSize - 1 - i))
                        ->setNotional(sign * std::get<0>(
                                notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
                std::dynamic_pointer_cast<CompoundedOvernightRateCashflow2>(leg.getCashflowAt(legSize - 1 - i))
                        ->setAmortization(sign * std::get<1>(
                                notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
            }
        }
    }

    Leg LegFactory::buildBulletIcpClpLeg(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            double notional,
            bool doesAmortize,
            double spread,
            double gearing) {
        // Make all the holidays in the calendar into a shared_ptr.
        auto settCal = std::make_shared<std::vector<QCDate>>(settlementCalendar.getHolidays());

        // Minus sign is set if cashflows are paid.
        int sign;
        if (recPay == Receive) {
            sign = 1;
        } else {
            sign = -1;
        }

        // Instantiate factory and build the corresponding periods.
        QCInterestRatePeriodsFactory pf{startDate, endDate, endDateAdjustment,
                                        settlementPeriodicity.getString(), settlementStubPeriod, settCal,
                                        settlementLag,
                // The next parameters are useful only for IborLegs. Arbitrary values
                // are given to them in this case.
                                        settlementPeriodicity.getString(), settlementStubPeriod, settCal, 0, 0,
                                        settlementPeriodicity.getString()};
        auto periods = pf.getPeriods();

        // Load the periods into the structure of FixedRateCashflow and construct the Leg.
        Leg icpClpLeg;
        size_t numPeriods = periods.size();
        icpClpLeg.resize(numPeriods);
        size_t i = 0;
        for (const auto &period: periods) {
            QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
            QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
            QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
            double amort = 0.0;
            if (i == numPeriods - 1) {
                amort = sign * notional;
            }
            IcpClpCashflow icpclpc{thisStartDate, thisEndDate, settlementDate,
                                   sign * notional, amort, doesAmortize, spread, gearing, DEFAULT_ICP, DEFAULT_ICP};
            icpClpLeg.setCashflowAt(std::make_shared<IcpClpCashflow>(icpclpc), i);
            ++i;
        }

        return icpClpLeg;
    }


    Leg LegFactory::buildBulletIcpClp2Leg(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            double notional,
            bool doesAmortize,
            double spread,
            double gearing,
            bool isAct360) {
        // Make all the holidays in the calendar into a shared_ptr.
        auto settCal = std::make_shared<std::vector<QCDate>>(settlementCalendar.getHolidays());

        // Minus sign is set if cashflows are paid.
        int sign;
        if (recPay == Receive) {
            sign = 1;
        } else {
            sign = -1;
        }

        // Instantiate factory and build the corresponding periods.
        QCInterestRatePeriodsFactory pf{startDate, endDate, endDateAdjustment,
                                        settlementPeriodicity.getString(), settlementStubPeriod, settCal,
                                        settlementLag,
                // The next parameters are useful only for IborLegs. Arbitrary values
                // are given to them in this case.
                                        settlementPeriodicity.getString(), settlementStubPeriod, settCal, 0, 0,
                                        settlementPeriodicity.getString()};
        auto periods = pf.getPeriods();

        // Load the periods into the structure of FixedRateCashflow and construct the Leg.
        Leg icpClpLeg;
        size_t numPeriods = periods.size();
        icpClpLeg.resize(numPeriods);
        size_t i = 0;
        for (const auto &period: periods) {
            QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
            QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
            QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
            double amort = 0.0;
            if (i == numPeriods - 1) {
                amort = sign * notional;
            }
            // This is the only difference with respect to the method that returns
            // a Leg with cashflows of type IcpClpCashflow
            IcpClpCashflow2 icpclpc{thisStartDate, thisEndDate, settlementDate,
                                    sign * notional, amort, doesAmortize, spread, gearing,
                                    isAct360, DEFAULT_ICP, DEFAULT_ICP};
            icpClpLeg.setCashflowAt(std::make_shared<IcpClpCashflow2>(icpclpc), i);
            ++i;
        }

        return icpClpLeg;
    }


    Leg LegFactory::buildCustomAmortIcpClpLeg(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            CustomNotionalAmort notionalAndAmort,
            bool doesAmortize,
            double spread,
            double gearing) {
        Leg icpClpLeg = buildBulletIcpClpLeg(
                recPay,
                startDate,
                endDate,
                endDateAdjustment,
                settlementPeriodicity,
                settlementStubPeriod,
                settlementCalendar,
                settlementLag,
                100.0,
                doesAmortize,
                spread,
                gearing);

        // std::cout << "custom amort icp clp leg: done bullet" << std::endl;
        customizeAmortization(recPay, icpClpLeg, notionalAndAmort, LegFactory::icpClpCashflow);
        return icpClpLeg;
    }


    Leg LegFactory::buildCustomAmortIcpClp2Leg(
            RecPay recPay,
            QCDate startDate,
            QCDate endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            CustomNotionalAmort notionalAndAmort,
            bool doesAmortize,
            double spread,
            double gearing,
            bool isAct360) {
        Leg icpClpLeg = buildBulletIcpClp2Leg(
                recPay,
                startDate,
                endDate,
                endDateAdjustment,
                settlementPeriodicity,
                settlementStubPeriod,
                settlementCalendar,
                settlementLag,
                100.0,
                doesAmortize,
                spread,
                gearing,
                isAct360);

        // std::cout << "custom amort icp clp leg: done bullet" << std::endl;
        customizeAmortization(recPay, icpClpLeg, notionalAndAmort, LegFactory::icpClpCashflow2);
        return icpClpLeg;

    }

    LegFactory::~LegFactory() {
    }

    Leg LegFactory::buildBulletCompoundedOvernightLeg(
            RecPay recPay,
            const QCDate &startDate,
            const QCDate &endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            QCBusinessCalendar fixingCalendar,
            const shared_ptr<InterestRateIndex> &index,
            double notional,
            bool doesAmortize,
            const shared_ptr<QCCurrency> &currency,
            double spread,
            double gearing,
            bool isAct360,
            unsigned int eqRateDecimalPlaces,
            unsigned int lookback,
            unsigned int lockout) {

        // Make all the holidays in the calendar into a shared_ptr.
        auto settCal = std::make_shared<std::vector<QCDate>>(settlementCalendar.getHolidays());
        auto fixCal = std::make_shared<std::vector<QCDate>>(fixingCalendar.getHolidays());

        // Minus sign is set if cashflows are paid.
        int sign;
        if (recPay == Receive) {
            sign = 1;
        } else {
            sign = -1;
        }

        // Se da de alta la fábrica de periods
        QCInterestRatePeriodsFactory factory{
                startDate,
                endDate,
                endDateAdjustment,
                settlementPeriodicity.getString(),
                settlementStubPeriod,
                settCal,
                settlementLag,
                settlementPeriodicity.getString(),
                QCInterestRateLeg::QCStubPeriod::qcShortFront,
                fixCal,
                0,
                index->getDaysOfStartLag(),
                index->getTenor().getString()};

        //Se generan los periodos
        auto periods = factory.getPeriods();

        // Load the periods into the structure of CompoundOvernightRateCashflow and construct the Leg.
        Leg compoundedOvernightRateLeg;
        size_t numPeriods = periods.size();
        compoundedOvernightRateLeg.resize(numPeriods);
        size_t i = 0;
        for (const auto &period: periods) {
            QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
            QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
            QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
            std::vector<QCDate> fixingDates;
            auto fixingDate = thisStartDate;
            while (fixingDate < thisEndDate) {
                fixingDates.emplace_back(fixingDate);
                fixingDate = fixingCalendar.shift(fixingDate, 1);
            }
            double amort = 0.0;
            if (i == numPeriods - 1) {
                amort = sign * notional;
            }
            CompoundedOvernightRateCashflow comp{
                    index,
                    thisStartDate,
                    thisEndDate,
                    settlementDate,
                    fixingDates,
                    sign * notional,
                    amort,
                    doesAmortize,
                    currency,
                    spread,
                    gearing,
                    isAct360,
                    eqRateDecimalPlaces,
                    lookback,
                    lockout};

            compoundedOvernightRateLeg.setCashflowAt(
                    std::make_shared<CompoundedOvernightRateCashflow>(comp), i);
            ++i;
        }

        return compoundedOvernightRateLeg;
    }

    Leg LegFactory::buildCustomAmortCompoundedOvernightLeg(
            RecPay recPay,
            const QCDate &startDate,
            const QCDate &endDate,
            QCDate::QCBusDayAdjRules endDateAdjustment,
            Tenor settlementPeriodicity,
            QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
            QCBusinessCalendar settlementCalendar,
            unsigned int settlementLag,
            QCBusinessCalendar fixingCalendar,
            const shared_ptr<InterestRateIndex> &index,
            CustomNotionalAmort notionalAndAmort,
            bool doesAmortize,
            const shared_ptr<QCCurrency> &currency,
            double spread,
            double gearing,
            bool isAct360,
            unsigned int eqRateDecimalPlaces,
            unsigned int lookback,
            unsigned int lockout) {

        Leg comOvernightLeg = buildBulletCompoundedOvernightLeg(
                recPay,
                startDate,
                endDate,
                endDateAdjustment,
                settlementPeriodicity,
                settlementStubPeriod,
                settlementCalendar,
                settlementLag,
                fixingCalendar,
                index,
                notionalAndAmort.getNotionalAt(0),
                doesAmortize,
                currency,
                spread,
                gearing,
                isAct360,
                eqRateDecimalPlaces,
                lookback,
                lockout);

        customizeAmortization(recPay, comOvernightLeg, notionalAndAmort,
                              LegFactory::compoundedOvernightRateCashflow);
        return comOvernightLeg;
    }
}

