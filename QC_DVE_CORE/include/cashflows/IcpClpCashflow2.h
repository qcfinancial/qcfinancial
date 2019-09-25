#ifndef ICPCLPCASHFLOW2_H
#define ICPCLPCASHFLOW2_H

#include <asset_classes/InterestRateIndex.h>
#include <cashflows/LinearInterestRateCashflow.h>
#include "IcpClpCashflow.h"

namespace QCode
{
    namespace Financial
    {
        // const double DEFAULT_ICP = 10000.00;
        // const unsigned int DEFAULT_TNA_DECIMAL_PLACES = 4;

        typedef std::tuple<
                QCDate,                 /* Start Date */
                QCDate,                 /* End Date */
                QCDate,                 /* Settlement Date */
                double,                 /* Nominal */
                double,                 /* Amortization */
                bool,                   /* Amortization is cashflow */
                shared_ptr<QCCurrency>, /* Nominal Currency (always CLP) */
                double,                 /* Start date ICP value */
                double,                 /* End date ICP value */
                double,                 /* Rate */
                double,                 /* Interest */
                double,                 /* Spread */
                double                  /* Gearing */
        > IcpClpCashflow2Wrapper;

        class IcpClpCashflow2 : public LinearInterestRateCashflow
        {
        public:
            IcpClpCashflow2(const QCDate& startDate,
                           const QCDate& endDate,
                           const QCDate& settlementDate,
                           double nominal,
                           double amortization,
                           bool doesAmortize,
                           double spread,
                           double gearing,
                           double startDateICP = QCode::Financial::DEFAULT_ICP,
                           double endDateICP = QCode::Financial::DEFAULT_ICP);

            // Overriding methods in Cashflow
            virtual double amount() override;

            virtual shared_ptr<QCCurrency> ccy() override;

            virtual QCDate date() override;

            // Overriding methods in LinearInterestRateCashflow
            virtual std::string getType() const override;

            virtual std::shared_ptr<QCCurrency> getInitialCcy() const override;

            virtual const QCDate& getStartDate() const override;

            virtual const QCDate& getEndDate() const override;

            virtual const QCDate& getSettlementDate() const override;

            virtual const DateList& getFixingDates() const override;

            virtual double getNominal() const override;

            virtual double nominal(const QCDate& fecha) const override;

            virtual double getAmortization() const override;

            virtual double amortization() const override;

            virtual double interest() override;

            virtual double interest(const TimeSeries& fixings) override;

            virtual double fixing() override;

            virtual double fixing(const TimeSeries& fixings) override;

            virtual double accruedInterest(const QCDate& fecha) override;

            virtual double accruedInterest(const QCDate& fecha, const TimeSeries& fixings) override;

            virtual double accruedFixing(const QCDate& fecha) override;

            virtual double accruedFixing(const QCDate& fecha, const TimeSeries& fixings) override;

            virtual bool doesAmortize() const override;

            // Methods specific to this class
            void setTnaDecimalPlaces(unsigned int decimalPlaces);

            double getTna(QCDate date, double icpValue);

            void setStartDateICP(double icpValue);

            double getStartDateICP() const;

            double getEndDateICP() const;

            void setEndDateICP(double icpValue);

            void setNominal(double nominal);

            void setAmortization(double amortization);

            shared_ptr<IcpClpCashflowWrapper> wrap();

            double getRateValue();

            std::string getTypeOfRate();

            double getSpread() const;

            double getGearing() const;

            virtual ~IcpClpCashflow2();

        protected:
            /** @brief	The interest rate index. It is always Lin ACT360. */
            QCInterestRate 	_rate = QCInterestRate(0.0,
                                                     std::make_shared<QCAct360>(QCAct360()),
                                                     std::make_shared<QCLinearWf>(QCLinearWf())
            );

            /** @brief	The default amount of decimal places for TNA. */
            unsigned int _tnaDecimalPlaces;

            /** @brief	Calculates interest (final or accrued) given a date and ICP value. */
            double _calculateInterest(QCDate& date, double icpValue);

            /** @brief	ICP at start date */
            double _startDateICP;

            /** @brief	ICP at end date */
            double _endDateICP;

            /** @brief	The start date */
            QCDate _startDate;

            /** @brief	The end date */
            QCDate _endDate;

            /** @brief	The settlement date */
            QCDate _settlementDate;

            /** @brief	List of dates corresponding to fixing dates. In this case only contains start date */
            vector<QCDate> _fixingDates;

            /** @brief	The nominal */
            double _nominal;

            /** @brief	The amortization */
            double _amortization;

            /** @brief	The interest amount calculated */
            double _interest;

            /** @brief	True if amortization is part of the cashflow */
            bool _doesAmortize;

            /** @brief	The nominal currency. It is always CLP. */
            shared_ptr<QCCurrency> _currency;

            /** @brief	The spread */
            double _spread;

            /** @brief	The gearing */
            double _gearing;

            /**
             * @fn	bool IcpClpCashflow::_validate();
             *
             * @brief	Validates that the object is instantiated properly.
             *
             * @author	Alvaro D�az V.
             * @date	24/09/2018
             *
             * @return	True if it succeeds, false if it fails.
             */
            bool _validate();

            /** @brief	Message that describes errors encountered when constructing the object */
            std::string _validateMsg;


        };
    }
}

#endif //ICPCLPCASHFLOW2_H
