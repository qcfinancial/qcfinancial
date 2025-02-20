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
            IcpClpCashflow2(
                    const QCDate& startDate,
                    const QCDate& endDate,
                    const QCDate& settlementDate,
                    double nominal,
                    double amortization,
                    bool doesAmortize,
                    double spread,
                    double gearing,
                    bool isAct360,
                    double startDateICP = QCode::Financial::DEFAULT_ICP,
                    double endDateICP = QCode::Financial::DEFAULT_ICP);

            // Overriding methods in Cashflow
            double amount() override;

            shared_ptr<QCCurrency> ccy() override;

            QCDate date() override;

            QCDate endDate() override;

            // Overriding methods in LinearInterestRateCashflow
            std::string getType() const override;

            std::shared_ptr<QCCurrency> getInitialCcy() const override;

            const QCDate& getStartDate() const override;

            const QCDate& getEndDate() const override;

            const QCDate& getSettlementDate() const override;

            const std::vector<QCDate>& getFixingDates() const override;

            double getNominal() const override;

            double nominal(const QCDate& fecha) const override;

            double getAmortization() const override;

            double amortization() const override;

            double interest() override;

            double interest(const TimeSeries& fixings) override;

            double fixing() override;

            double fixing(const TimeSeries& fixings) override;

            double accruedInterest(const QCDate& fecha) override;

            double accruedInterest(const QCDate& fecha, const TimeSeries& fixings) override;

            double accruedFixing(const QCDate& fecha) override;

            double accruedFixing(const QCDate& fecha, const TimeSeries& fixings) override;


            [[nodiscard]] bool doesAmortize() const override;

            // Methods specific to this class
            [[nodiscard]] std::vector<double> getAmountDerivatives() const;

            void setTnaDecimalPlaces(unsigned int decimalPlaces);

            double getTna(QCDate date, double icpValue);

            void setStartDateICP(double icpValue);

            void setStartDateICPDerivatives(std::vector<double> der);

            [[nodiscard]] std::vector<double> getStartDateICPDerivatives() const;

            [[nodiscard]] double getStartDateICP() const;

            [[nodiscard]] double getEndDateICP() const;

            void setEndDateICP(double icpValue);

            void setEndDateICPDerivatives(std::vector<double> der);

            [[nodiscard]] std::vector<double> getEndDateICPDerivatives() const;

            void setNominal(double nominal);

            void setAmortization(double amortization);

            shared_ptr<IcpClpCashflowWrapper> wrap();

            double getRateValue();

            std::string getTypeOfRate();

            [[nodiscard]] double getSpread() const;

            [[nodiscard]] double getGearing() const;

            virtual ~IcpClpCashflow2();

        protected:
            /** @brief	The interest rate index. It is always Lin ACT360. */
            QCInterestRate 	_rate = QCInterestRate(0.0,
                    std::make_shared<QCAct360>(QCAct360()),
                    std::make_shared<QCLinearWf>(QCLinearWf()));;

            /** @brief	The default amount of decimal places for TNA. */
            unsigned int _tnaDecimalPlaces;

            /** @brief	Calculates interest (final or accrued) given a date and ICP value. */
            double _calculateInterest(QCDate& date, double icpValue);

            /** @brief	ICP at start date */
            double _startDateICP;

            /** @brief	Stores de derivatives of ICP at start date with respect to rates belonging to zero
             * coupon curve.
            */
            std::vector<double> _startDateICPDerivatives;


            /** @brief	Stores de derivatives of amount() with respect to rates belonging to zero
            * coupon curve.
            */
            std::vector<double> _amountDerivatives;

            /** @brief	ICP at end date */
            double _endDateICP;

            /** @brief	Stores de derivatives of ICP at end date with respect to rates belonging to zero
            * coupon curve.
            */
            std::vector<double> _endDateICPDerivatives;

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
             * @author	Alvaro Díaz V.
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
