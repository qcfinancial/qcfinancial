//
// Created by Alvaro Diaz on 2019-07-28.
//

#ifndef IBORCASHFLOW2_H
#define IBORCASHFLOW2_H

#include <asset_classes/InterestRateIndex.h>
#include <cashflows/LinearInterestRateCashflow.h>

namespace QCode
{
    namespace Financial
    {
        class IborCashflow2 : public LinearInterestRateCashflow
        {
        public:
            typedef std::tuple<
                    QCDate,                  /* Start date */
                    QCDate,                  /* End date */
                    QCDate,                  /* Fixing date */
                    QCDate,                  /* Settlement date */
                    double,                  /* Nominal */
                    double,                  /* Amortization */
                    double,                  /* Interest */
                    bool,                    /* Amortization is cashflow */
                    double,                  /* Total cashflow */
                    shared_ptr<QCCurrency>,  /* Nominal currency */
                    std::string,             /* Interest rate index code */
                    QCInterestRate,			 /* Interest rate */
                    double,                  /* Spread */
                    double,                  /* Gearing */
                    double					 /* Interest rate value */
            > IborCashflow2Wrapper;

            IborCashflow2(
                    std::shared_ptr<InterestRateIndex> index,
                    const QCDate& startDate,
                    const QCDate& endDate,
                    const QCDate& fixingDate,
                    const QCDate& settlementDate,
                    double nominal,
                    double amortization,
                    bool doesAmortize,
                    shared_ptr<QCCurrency> currency,
                    double spread,
                    double gearing);

            double amount() override;

            shared_ptr<QCCurrency> ccy() override;

            QCDate date() override;

            string getType() const override;

            shared_ptr<QCCurrency> getInitialCcy() const override;

            void setRateValue(double rateValue);


            const QCDate& getStartDate() const override;

            const QCDate& getEndDate() const override;

            const QCDate& getSettlementDate() const override;

            const DateList& getFixingDates() const override;

            std::shared_ptr<InterestRateIndex> getInterestRateIndex();

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

            bool doesAmortize() const override;

            double getSpread() const;

            double getGearing() const;

            void setNominal(double nominal);

            void setAmortization(double amortization);

            shared_ptr<IborCashflow2Wrapper> wrap();

            void setForwardRateWfDerivatives(const std::vector<double>& der);

            std::vector<double> getAmountDerivatives() const;

            ~IborCashflow2() override;

        protected:
            double _rateValue;

            void _calculateInterest();

            /** @brief	The associated interest rate index */
            std::shared_ptr<InterestRateIndex> _index;

            /** @brief	The start date */
            QCDate _startDate;

            /** @brief	The end date */
            QCDate _endDate;

            /** @brief	The fixing date */
            QCDate _fixingDate;

            DateList _fixingDates;

            /** @brief	The settlement date */
            QCDate _settlementDate;

            /** @brief	The nominal */
            double _nominal;

            /** @brief	The amortization */
            double _amortization;

            /** @brief	The interest amount calculated */
            double _interest;

            /** @brief	True if amortization is part of the cashflow */
            bool _doesAmortize;

            /** @brief	The currency */
            std::shared_ptr<QCCurrency> _currency;

            /** @brief	The spread */
            double _spread;

            /** @brief	The gearing */
            double _gearing;

            /** @brief	Message that describes errors encountered when constructing the object */
            std::string _validateMsg;

            /** @brief	Stores de derivatives of forward rate wealth factor with respect to rates belonging to zero
             * coupon curve.
            */
            std::vector<double> _forwardRateWfDerivatives;

            /** @brief	Stores de derivatives of amount() with respect to rates belonging to zero
            * coupon curve.
            */
            std::vector<double> _amountDerivatives;


            /**
             * @fn	bool IborCashflow2::_validate();
             *
             * @brief	Validates that the object is properly constructed. More precisely,
             * 			the following checks are performed:
             * 			- startDate < endDate
             * 			- settlementDate >= endDate
             * 			- fixingDate <= startDate
             *
             * @author	Alvaro Díaz V.
             * @date	28/07/2019
             *
             * @return	True if it succeeds, false if it fails.
             */
            bool _validate();

        };

    }
}


#endif //IBORCASHFLOW2_H
