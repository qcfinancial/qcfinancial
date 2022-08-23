
//
// Created by ADiazV on 20-12-2021.
//

#ifndef QC_DVE_CORE_COMPOUNDEDOVERNIGHTRATECASHFLOW_H
#define QC_DVE_CORE_COMPOUNDEDOVERNIGHTRATECASHFLOW_H

#include <asset_classes/InterestRateIndex.h>
#include <cashflows/LinearInterestRateCashflow.h>
#include <asset_classes/QCAct360.h>
#include <asset_classes/QCLinearWf.h>

namespace QCode {

    namespace Financial {

        class CompoundedOvernightRateCashflow : public LinearInterestRateCashflow {
        public:
            typedef std::tuple<
                    QCDate,                  /* Start date */
                    QCDate,                  /* End date */
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
            > CompoundedOvernightRateCashflowWrapper;
            CompoundedOvernightRateCashflow(
                    std::shared_ptr<InterestRateIndex> index,
                    const QCDate &startDate,
                    const QCDate &endDate,
                    const QCDate &settlementDate,
                    const std::vector<QCDate> &fixingDates,
                    double nominal,
                    double amortization,
                    bool doesAmortize,
                    shared_ptr<QCCurrency> currency,
                    double spread,
                    double gearing,
                    bool isAct360,
                    unsigned int eqRateDecimalPlaces,
                    unsigned int lookback,
                    unsigned int lockout);

            // Overriding methods in Cashflow
            double amount() override;

            shared_ptr<QCCurrency> ccy() override;

            QCDate date() override;

            // Overriding methods in LinearInterestRateCashflow
            std::string getType() const override;

            std::shared_ptr<QCCurrency> getInitialCcy() const override;

            const QCDate &getStartDate() const override;

            const QCDate &getEndDate() const override;

            const QCDate &getSettlementDate() const override;

            const DateList &getFixingDates() const override;

            std::string getInterestRateIndexCode() const;

            double getNominal() const override;

            double nominal(const QCDate &fecha) const override;

            double getAmortization() const override;

            double amortization() const override;

            double interest() override;

            double interest(const TimeSeries &fixings) override;

            double fixing() override;

            double fixing(const TimeSeries &fixings) override;

            double accruedInterest(const QCDate &fecha) override;

            double accruedInterest(const QCDate &fecha, const TimeSeries &fixings) override;

            double accruedFixing(const QCDate &fecha) override;

            double accruedFixing(const QCDate &fecha, const TimeSeries &fixings) override;

            bool doesAmortize() const override;

            // Methods specific to this class
            std::shared_ptr<QCode::Financial::InterestRateIndex> getInterestRateIndex();

            std::vector<double> getAmountDerivatives() const;

            void setNominal(double nominal);

            void setAmortization(double amortization);

            std::string getTypeOfRate();

            double getSpread() const;

            double getGearing() const;

            unsigned int getEqRateDecimalPlaces() const;

            unsigned int getLookBack() const;

            unsigned int getLockOut() const;

            void setAmountDerivatives(std::vector<double> amountDerivatives);

            void setInitialDateWf(double wf);

            void setEndDateWf(double wf);

            std::shared_ptr<CompoundedOvernightRateCashflowWrapper> wrap();

            ~CompoundedOvernightRateCashflow() override = default;

        protected:
            /** @brief	The interest rate index. It is always Lin ACT360. */
            QCInterestRate _rate = QCInterestRate(
                    0.0,
                    std::make_shared<QCAct360>(QCAct360()),
                    std::make_shared<QCLinearWf>(QCLinearWf()));

            std::shared_ptr<InterestRateIndex> _index;

            std::map<QCDate, QCDate> _indexEndDates;

            bool _isAct360;

            double _initialDateWf{};

            double _endDateWf{};

            double _getFixingWf(QCDate &fecha, const QCode::Financial::TimeSeries &fixings);

            /** @brief	The default amount of decimal places for TNA. */
            unsigned int _eqRateDecimalPlaces;

            /** @brief	Stores the derivatives of amount() with respect to rates belonging to zero
            * coupon curve.
            */
            std::vector<double> _amountDerivatives;

            unsigned int _lookback;

            unsigned int _lockout;

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

            void _fillIndexEndDates();

            double _calculateInterest(double rateValue, QCDate &fecha);

            double _getRateValue() const;
        };
    }
}


#endif //QC_DVE_CORE_COMPOUNDEDOVERNIGHTRATECASHFLOW_H
