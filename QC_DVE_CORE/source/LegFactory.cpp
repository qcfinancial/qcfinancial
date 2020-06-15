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


#include "QCInterestRatePeriodsFactory.h"

namespace QCode
{
	namespace Financial
	{
		LegFactory::LegFactory()
		{
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
			bool forBonds)
		{
			auto settlementPeriodicityString = Tenor(settlementPeriodicity).getString();
			// Make all the holidays in the calendar into a shared_ptr.
			auto settCal = std::make_shared<DateList>(settlementCalendar.getHolidays());

			// Minus sign is set if cashflows are paid.
			int sign;
			if (recPay == Receive)
			{
				sign = 1;
			}
			else
			{
				sign = -1;
			}

			// Instantiate factory and build the corresponding periods.
			QCInterestRatePeriodsFactory pf{ startDate, endDate, endDateAdjustment,
				settlementPeriodicityString, settlementStubPeriod, settCal, settlementLag,
				// The next parameters are useful only for IborLegs. Arbitrary values
				// are given to them in this case.
				settlementPeriodicityString, settlementStubPeriod, settCal, 0, 0, settlementPeriodicityString };
			auto periods = pf.getPeriods();

			// Load the periods into the structure of FixedRateCashflow and contruct the Leg.
			Leg fixedRateleg;
			size_t numPeriods = periods.size();
			fixedRateleg.resize(numPeriods);
			size_t i = 0;
			for (const auto& period : periods)
			{
				QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
				QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
				QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
				// For the correct calculation of present values using market yields according
				// to the usual conventions in fixed income markets.
				if (forBonds) settlementDate = thisEndDate; 
				double amort = 0.0;
				if (i == numPeriods - 1)
				{
					amort = sign * notional;
				}
				FixedRateCashflow frc{ thisStartDate, thisEndDate, settlementDate,
					sign * notional, amort, doesAmortize, rate, currency };
				fixedRateleg.setCashflowAt(std::make_shared<FixedRateCashflow>(frc), i);
				++i;
			}

			return fixedRateleg;
		}
		
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
			bool forBonds)
		{
			auto settlementPeriodicityString = Tenor(settlementPeriodicity).getString();
			// Make all the holidays in the calendar into a shared_ptr.
			auto settCal = std::make_shared<DateList>(settlementCalendar.getHolidays());

			// Minus sign is set if cashflows are paid.
			int sign;
			if (recPay == Receive)
			{
				sign = 1;
			}
			else
			{
				sign = -1;
			}

			// Instantiate factory and build the corresponding periods.
			QCInterestRatePeriodsFactory pf{ startDate, endDate, endDateAdjustment,
				settlementPeriodicityString, settlementStubPeriod, settCal, settlementLag,
				// The next parameters are useful only for IborLegs. Arbitrary values
				// are given to them in this case.
				settlementPeriodicityString, settlementStubPeriod, settCal, 0, 0, settlementPeriodicityString };
			auto periods = pf.getPeriods();

			// Load the periods into the structure of FixedRateCashflow and construct the Leg.
			Leg fixedRateleg;
			size_t numPeriods = periods.size();
			fixedRateleg.resize(numPeriods);
			size_t i = 0;
			for (const auto& period : periods)
			{
				QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
				QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
				QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
				// For the correct calculation of present values using market yields according
				// to the usual conventions in fixed income markets.
				if (forBonds) settlementDate = thisEndDate;
				double amort = 0.0;
				if (i == numPeriods - 1)
				{
					amort = sign * notional;
				}
				FixedRateCashflow2 frc{ thisStartDate, thisEndDate, settlementDate,
					sign * notional, amort, doesAmortize, rate, currency };
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
                bool forBonds)
        {
            auto settlementPeriodicityString = Tenor(settlementPeriodicity).getString();
            // Make all the holidays in the calendar into a shared_ptr.
            auto settCal = std::make_shared<DateList>(settlementCalendar.getHolidays());

            // Minus sign is set if cashflows are paid.
            int sign;
            if (recPay == Receive)
            {
                sign = 1;
            }
            else
            {
                sign = -1;
            }

            // Instantiate factory and build the corresponding periods.
            QCInterestRatePeriodsFactory pf{ startDate, endDate, endDateAdjustment,
                                             settlementPeriodicityString, settlementStubPeriod,
                                             settCal, settlementLag,
                    // The next parameters are useful only for IborLegs. Arbitrary values
                    // are given to them in this case.
                                             settlementPeriodicityString, settlementStubPeriod,
                                             settCal, 0, 0, settlementPeriodicityString };
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
            std::cout << "rate description: " << rate.description() << std::endl;
            for (const auto& period : periods)
            {
                QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
                QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
                QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
                // For the correct calculation of present values using market yields according
                // to the usual conventions in fixed income markets.
                if (forBonds) settlementDate = thisEndDate;
                double amort = cuota - tempNotional * r;
                FixedRateCashflow2 frc{ thisStartDate, thisEndDate, settlementDate,
                                        sign * tempNotional, amort, doesAmortize, rate, currency };
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
                std::string fechaInicio)
        {
            // Se da de alta los parámetros requeridos
            auto rp = RecPay::Receive;
            auto qcFechaInicio = QCDate {fechaInicio};
            auto qcFechaFinal =  qcFechaInicio.addMonths(plazo * 12);
            auto busAdjmntRule = QCDate::QCBusDayAdjRules::qcNo;
            auto periodicidad = Tenor {"1M"};

            // Con período irregular las cuotas no quedan iguales
            auto periodoIrregular = QCInterestRateLeg::QCStubPeriod::qcNoStubPeriod;

            auto calendario = QCBusinessCalendar {qcFechaInicio, 20};
            calendario.addHolyday(QCDate(31, 12, 2020));
            unsigned int lagPago = 0;
            auto nominal = monto;
            auto amortEsFlujo = true;

            // Si la tasa es Act/360 o Act/365 las cuotas no van a quedar iguales
            QCYrFrctnShrdPtr yf = std::make_shared<QC30360>(QC30360());
            QCWlthFctrShrdPtr wf = std::make_shared<QCLinearWf>(QCLinearWf());
            QCInterestRate tasaCupon {tasa, yf, wf};

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
                const std::vector<double>& probabilidades)
        {
            auto girosAdicionales = girosTotales - ultimoGiro;
            std::vector<std::tuple<double, Leg>> creditos;
            auto fechaHoy = QCDate(fechaCalculo);
            auto qcFechaProximoGiro = QCDate(fechaProximoGiro);
            double probabilidadAcumulada {0.0};
            for (size_t i = 0; i < girosAdicionales + 1; ++i)
            {
                saldo *= 1 + tasa * fechaHoy.dayDiff(qcFechaProximoGiro) / 360.0;
                auto qcFechaInicio = qcFechaProximoGiro.addMonths(24);
                auto monto = saldo * (1 + tasa * qcFechaProximoGiro.dayDiff(qcFechaInicio) / 360.0);
                if (i < girosAdicionales)
                {
                    creditos.push_back(
                            std::make_tuple(probabilidades[ultimoGiro + 1],
                                    makeLoan(monto, plazoCredito, tasa, qcFechaInicio.description(false))
                            )
                    );
                    probabilidadAcumulada += probabilidades.at(ultimoGiro + 1);
                }
                else
                {
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

        manyCae LegFactory::buildBulkCae(const dataManyCae& data,
                const std::vector<double>& probabilidades)
        {
		    manyCae result;
		    for (const auto& dato : data)
            {
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
			bool forBonds)
		{
			auto settlementPeriodicityString = Tenor(settlementPeriodicity).getString();
			// Make all the holidays in the calendar into a shared_ptr.
			auto settCal = std::make_shared<DateList>(settlementCalendar.getHolidays());

			// Minus sign is set if cashflows are paid.
			int sign;
			if (recPay == Receive)
			{
				sign = 1;
			}
			else
			{
				sign = -1;
			}

			// Instantiate factory and build the corresponding periods.
			QCInterestRatePeriodsFactory pf{ startDate, endDate, endDateAdjustment,
				settlementPeriodicityString, settlementStubPeriod, settCal, settlementLag,
				// The next parameters are useful only for IborLegs. Arbitrary values
				// are given to them in this case.
				settlementPeriodicityString, settlementStubPeriod, settCal, 0, 0, settlementPeriodicityString };
			auto periods = pf.getPeriods();

			// Load the periods into the structure of FixedRateCashflow and contruct the Leg.
			Leg fixedRateMultiCurrencyLeg;
			size_t numPeriods = periods.size();
			fixedRateMultiCurrencyLeg.resize(numPeriods);
			size_t i = 0;
			for (const auto& period : periods)
			{
				QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
				QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
				QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
				QCDate fxRateIndexFixingDate = fxRateIndex->getCalendar().shift(settlementDate, fxRateIndexFixingLag);
				// For the correct calculation of present values using market yields according
				// to the usual conventions in fixed income markets.
				if (forBonds) settlementDate = thisEndDate;
				double amort = 0.0;
				if (i == numPeriods - 1)
				{
					amort = sign * notional;
				}
				FixedRateMultiCurrencyCashflow frmcc{ thisStartDate, thisEndDate, settlementDate,
					sign * notional, amort, doesAmortize, rate, notionalCurrency, fxRateIndexFixingDate, settlementCurrency,
				    fxRateIndex, DEFAULT_FX_RATE_INDEX_VALUE};
				fixedRateMultiCurrencyLeg.setCashflowAt(std::make_shared<FixedRateMultiCurrencyCashflow>(frmcc), i);
				++i;
			}

			return fixedRateMultiCurrencyLeg;
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
			std::shared_ptr<QCCurrency> currency)
		{
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
				currency);

			customizeAmortization(recPay, fixedRateLeg, notionalAndAmort, LegFactory::fixedRateCashflow);
			return fixedRateLeg;
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
			std::shared_ptr<QCCurrency> currency)
		{
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
			double gearing)
		{
			// Make all the holidays in the calendar into a shared_ptr.
			auto settCal = std::make_shared<DateList>(settlementCalendar.getHolidays());
			auto fixCal = std::make_shared<DateList>(fixingCalendar.getHolidays());

			// Minus sign is set if cashflows are paid.
			int sign;
			if (recPay == Receive)
			{
				sign = 1;
			}
			else
			{
				sign = -1;
			}

			//Se da de alta la fabrica de periods
			QCInterestRatePeriodsFactory factory{ startDate, endDate,
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
				index->getTenor().getString() };

			//Se generan los periodos
			auto periods = factory.getPeriods();

			// Load the periods into the structure of FixedRateCashflow and contruct the Leg.
			Leg iborLeg;
			size_t numPeriods = periods.size();
			iborLeg.resize(numPeriods);
			size_t i = 0;
			for (const auto& period : periods)
			{
				QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
				QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
				QCDate thisFixingDate = get<QCInterestRateLeg::intRtPrdElmntFxngDate>(period);
				QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
				double amort = 0.0;
				if (i == numPeriods - 1)
				{
					amort = sign * notional;
				}
				IborCashflow frc{ index, thisStartDate, thisEndDate, thisFixingDate, settlementDate, 
					sign * notional, amort, doesAmortize, currency, spread, gearing };
				iborLeg.setCashflowAt(std::make_shared<IborCashflow>(frc), i);
				++i;
			}

			return iborLeg;

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
                double gearing)
        {
            // Make all the holidays in the calendar into a shared_ptr.
            auto settCal = std::make_shared<DateList>(settlementCalendar.getHolidays());
            auto fixCal = std::make_shared<DateList>(fixingCalendar.getHolidays());

            // Minus sign is set if cashflows are paid.
            int sign;
            if (recPay == Receive)
            {
                sign = 1;
            }
            else
            {
                sign = -1;
            }

            //Se da de alta la fabrica de periods
            QCInterestRatePeriodsFactory factory{ startDate, endDate,
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
                                                  index->getTenor().getString() };

            //Se generan los periodos
            auto periods = factory.getPeriods();

            // Load the periods into the structure of FixedRateCashflow and contruct the Leg.
            Leg iborLeg;
            size_t numPeriods = periods.size();
            iborLeg.resize(numPeriods);
            size_t i = 0;
            for (const auto& period : periods)
            {
                QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
                QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
                QCDate thisFixingDate = get<QCInterestRateLeg::intRtPrdElmntFxngDate>(period);
                QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
                double amort = 0.0;
                if (i == numPeriods - 1)
                {
                    amort = sign * notional;
                }
                IborCashflow2 frc{ index, thisStartDate, thisEndDate, thisFixingDate, settlementDate,
                                  sign * notional, amort, doesAmortize, currency, spread, gearing };
                iborLeg.setCashflowAt(std::make_shared<IborCashflow2>(frc), i);
                ++i;
            }

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
				unsigned int fxRateIndexFixingLag
				)
			{
				// Make all the holidays in the calendar into a shared_ptr.
				auto settCal = std::make_shared<DateList>(settlementCalendar.getHolidays());
				auto fixCal = std::make_shared<DateList>(fixingCalendar.getHolidays());

				// Minus sign is set if cashflows are paid.
				int sign;
				if (recPay == Receive)
				{
					sign = 1;
				}
				else
				{
					sign = -1;
				}

				//Se da de alta la fabrica de periods
				QCInterestRatePeriodsFactory factory{ startDate, endDate,
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
					index->getTenor().getString() };

				//Se generan los periodos
				auto periods = factory.getPeriods();

				// Load the periods into the structure of IborMultiCurrencyLeg and contruct the Leg.
				Leg iborMultiCurrencyLeg;
				size_t numPeriods = periods.size();
				iborMultiCurrencyLeg.resize(numPeriods);
				size_t i = 0;
				for (const auto& period : periods)
				{
					QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
					QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
					QCDate thisFixingDate = get<QCInterestRateLeg::intRtPrdElmntFxngDate>(period);
					QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
					QCDate fxRateIndexFixingDate = fxRateIndex->getCalendar().shift(settlementDate, fxRateIndexFixingLag);

					double amort = 0.0;
					if (i == numPeriods - 1)
					{
						amort = sign * notional;
					}
					IborMultiCurrencyCashflow imccy{ index, thisStartDate, thisEndDate, thisFixingDate, settlementDate,
						sign * notional, amort, doesAmortize, notionalCurrency, spread, gearing, fxRateIndexFixingDate,
						settlementCurrency, fxRateIndex };
					iborMultiCurrencyLeg.setCashflowAt(std::make_shared<IborMultiCurrencyCashflow>(imccy), i);
					++i;
				}

				return iborMultiCurrencyLeg;

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
			double gearing)
		{
			// Primero construir una pata bullet
			Leg iborLeg = buildBulletIborLeg(recPay,
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


			std::cout << "custom amort ibor leg: done bullet" << std::endl;
			customizeAmortization(recPay, iborLeg, notionalAndAmort, LegFactory::iborCashflow);
			return iborLeg;
		}

		void LegFactory::customizeAmortization(RecPay recPay,
											   Leg& leg,
											   CustomNotionalAmort notionalAndAmort,
											   TypeOfCashflow typeOfCashflow)
		{
			// Ahora modificar nominal y amortizaci�n en el objeto Leg reci�n construido.
			size_t notionalAndAmortSize = notionalAndAmort.getSize();
			size_t legSize = leg.size();
			if (legSize == 0)
			{
				throw invalid_argument("LegFactory::customizeAmortization. Leg has 0 cashflows");
			}
			size_t minSize = std::min(notionalAndAmortSize, legSize);
			// std::cout << "legSize: " << legSize << std::endl;
			// std::cout << "notionalAndAmortSize: " << notionalAndAmortSize << std::endl;

			int sign;
			if (recPay == Receive)
			{
				sign = 1;
			}
			else
			{
				sign = -1;
			}

			if (typeOfCashflow == LegFactory::fixedRateCashflow)
			{
				for (size_t i = 0; i < minSize; ++i)
				{
					std::dynamic_pointer_cast<FixedRateCashflow>(leg.getCashflowAt(legSize - 1 - i))
						->setNominal(sign * std::get<0>(notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
					std::dynamic_pointer_cast<FixedRateCashflow>(leg.getCashflowAt(legSize - 1 - i))
						->setAmortization(sign * std::get<1>(notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
				}
			}

			if (typeOfCashflow == LegFactory::fixedRateCashflow2)
			{
				for (size_t i = 0; i < minSize; ++i)
				{
					auto c = std::dynamic_pointer_cast<FixedRateCashflow2>(leg.getCashflowAt(legSize - 1 - i));
					auto nominal = sign * std::get<0>(notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]);
					auto amort = sign * std::get<1>(notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]);
					auto newC = FixedRateCashflow2(c->getStartDate(), c->getEndDate(), c->getSettlementDate(), nominal, amort,
						c->doesAmortize(), std::get<8>(*(c->wrap())), c->ccy());
						leg.setCashflowAt(std::make_shared<FixedRateCashflow2>(newC), legSize - 1 - i);
				}
			}

			if (typeOfCashflow == LegFactory::iborCashflow)
			{
				for (size_t i = 0; i < minSize; ++i)
				{
					std::dynamic_pointer_cast<IborCashflow>(leg.getCashflowAt(legSize - 1 - i))
						->setNominal(sign * std::get<0>(notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
					std::dynamic_pointer_cast<IborCashflow>(leg.getCashflowAt(legSize - 1 - i))
						->setAmortization(sign * std::get<1>(notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
				}
			}

			if (typeOfCashflow == LegFactory::icpClpCashflow)
			{
				for (size_t i = 0; i < minSize; ++i)
				{
					std::dynamic_pointer_cast<IcpClpCashflow>(leg.getCashflowAt(legSize - 1 - i))
						->setNominal(sign * std::get<0>(notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
					std::dynamic_pointer_cast<IcpClpCashflow>(leg.getCashflowAt(legSize - 1 - i))
						->setAmortization(sign * std::get<1>(notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
				}
			}

			if (typeOfCashflow == LegFactory::icpClfCashflow)
			{
				for (size_t i = 0; i < minSize; ++i)
				{
					std::dynamic_pointer_cast<IcpClfCashflow>(leg.getCashflowAt(legSize - 1 - i))
						->setNominal(sign * std::get<0>(notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
					std::dynamic_pointer_cast<IcpClfCashflow>(leg.getCashflowAt(legSize - 1 - i))
						->setAmortization(sign * std::get<1>(notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
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
			double gearing)
		{
			// Make all the holidays in the calendar into a shared_ptr.
			auto settCal = std::make_shared<DateList>(settlementCalendar.getHolidays());

			// Minus sign is set if cashflows are paid.
			int sign;
			if (recPay == Receive)
			{
				sign = 1;
			}
			else
			{
				sign = -1;
			}

			// Instantiate factory and build the corresponding periods.
			QCInterestRatePeriodsFactory pf{ startDate, endDate, endDateAdjustment,
				settlementPeriodicity.getString(), settlementStubPeriod, settCal, settlementLag,
				// The next parameters are useful only for IborLegs. Arbitrary values
				// are given to them in this case.
				settlementPeriodicity.getString(), settlementStubPeriod, settCal, 0, 0,
				settlementPeriodicity.getString() };
			auto periods = pf.getPeriods();

			// Load the periods into the structure of FixedRateCashflow and construct the Leg.
			Leg icpClpLeg;
			size_t numPeriods = periods.size();
			icpClpLeg.resize(numPeriods);
			size_t i = 0;
			for (const auto& period : periods)
			{
				QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
				QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
				QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
				double amort = 0.0;
				if (i == numPeriods - 1)
				{
					amort = sign * notional;
				}
				IcpClpCashflow icpclpc{ thisStartDate, thisEndDate, settlementDate,
					sign * notional, amort, doesAmortize, spread, gearing, DEFAULT_ICP, DEFAULT_ICP };
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
                bool isAct360)
        {
            // Make all the holidays in the calendar into a shared_ptr.
            auto settCal = std::make_shared<DateList>(settlementCalendar.getHolidays());

            // Minus sign is set if cashflows are paid.
            int sign;
            if (recPay == Receive)
            {
                sign = 1;
            }
            else
            {
                sign = -1;
            }

            // Instantiate factory and build the corresponding periods.
            QCInterestRatePeriodsFactory pf{ startDate, endDate, endDateAdjustment,
                                             settlementPeriodicity.getString(), settlementStubPeriod, settCal, settlementLag,
                    // The next parameters are useful only for IborLegs. Arbitrary values
                    // are given to them in this case.
                                             settlementPeriodicity.getString(), settlementStubPeriod, settCal, 0, 0,
                                             settlementPeriodicity.getString() };
            auto periods = pf.getPeriods();

            // Load the periods into the structure of FixedRateCashflow and construct the Leg.
            Leg icpClpLeg;
            size_t numPeriods = periods.size();
            icpClpLeg.resize(numPeriods);
            size_t i = 0;
            for (const auto& period : periods)
            {
                QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
                QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
                QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
                double amort = 0.0;
                if (i == numPeriods - 1)
                {
                    amort = sign * notional;
                }
                // This is the only difference with respect to the method that returns
                // a Leg with cashflows of type IcpClpCashflow
                IcpClpCashflow2 icpclpc{ thisStartDate, thisEndDate, settlementDate,
                                        sign * notional, amort, doesAmortize, spread, gearing,
                                         isAct360, DEFAULT_ICP, DEFAULT_ICP };
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
			double gearing)
		{
			Leg icpClpLeg = buildBulletIcpClpLeg(recPay,
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

			std::cout << "custom amort icp clp leg: done bullet" << std::endl;
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
                bool isAct360)
        {
            Leg icpClpLeg = buildBulletIcpClp2Leg(recPay,
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

            std::cout << "custom amort icp clp leg: done bullet" << std::endl;
            customizeAmortization(recPay, icpClpLeg, notionalAndAmort, LegFactory::icpClpCashflow);
            return icpClpLeg;

        }


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
			double gearing)
		{
			// Make all the holidays in the calendar into a shared_ptr.
			auto settCal = std::make_shared<DateList>(settlementCalendar.getHolidays());

			// Minus sign is set if cashflows are paid.
			int sign;
			if (recPay == Receive)
			{
				sign = 1;
			}
			else
			{
				sign = -1;
			}

			// Instantiate factory and build the corresponding periods.
			QCInterestRatePeriodsFactory pf{ startDate, endDate, endDateAdjustment,
				settlementPeriodicity.getString(), settlementStubPeriod, settCal, settlementLag,
				// The next parameters are useful only for IborLegs. Arbitrary values
				// are given to them in this case.
				settlementPeriodicity.getString(), settlementStubPeriod, settCal, 0, 0, settlementPeriodicity.getString() };
			auto periods = pf.getPeriods();

			// Load the periods into the structure of FixedRateCashflow and contruct the Leg.
			Leg icpClfLeg;
			size_t numPeriods = periods.size();
			icpClfLeg.resize(numPeriods);
			vector<double> defaults{ DEFAULT_ICP, DEFAULT_ICP, DEFAULT_UF, DEFAULT_UF };
			size_t i = 0;
			for (const auto& period : periods)
			{
				QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
				QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
				QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
				double amort = 0.0;
				if (i == numPeriods - 1)
				{
					amort = sign * notional;
				}
				IcpClfCashflow icpclfc{ thisStartDate, thisEndDate, settlementDate,
					sign * notional, amort, doesAmortize, spread, gearing, defaults };
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
			double gearing)
		{
			Leg icpClfLeg = buildBulletIcpClfLeg(recPay,
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

			std::cout << "custom amort icp clf leg: done bullet" << std::endl;
			customizeAmortization(recPay, icpClfLeg, notionalAndAmort, LegFactory::icpClfCashflow);
			return icpClfLeg;

		}

		LegFactory::~LegFactory()
		{
		}

	}
}
