#include <string>

#include "cashflows/IcpClpCashflow2.h"

namespace QCode
{
    namespace Financial
    {
        IcpClpCashflow2::IcpClpCashflow2(const QCDate& startDate,
                            const QCDate& endDate,
                            const QCDate& settlementDate,
                            double nominal,
                            double amortization,
                            bool doesAmortize,
                            double spread,
                            double gearing,
                            double startDateICP,
                            double endDateICP) :
                            _startDate(startDate),
                            _endDate(endDate),
                            _settlementDate(settlementDate),
                            _nominal(nominal),
                            _amortization(amortization),
                            _doesAmortize(doesAmortize),
                            _spread(spread),
                            _gearing(gearing),
                            _startDateICP(startDateICP),
                            _endDateICP(endDateICP)
        {
#ifndef NO_CONSTRUCTOR_VALIDATION
            if (!_validate())
			{
				throw std::invalid_argument(_validateMsg);
			}
#endif
            _currency = std::make_shared<QCCurrency>(QCCLP());
            _tnaDecimalPlaces = DEFAULT_TNA_DECIMAL_PLACES;
            _rate.setValue(getTna(_endDate, _endDateICP));
            _fixingDates.resize(2);
            _fixingDates.at(0) = _startDate;
            _fixingDates.at(1) = _endDate;

        }


        // Overriding methods in Cashflow
        double IcpClpCashflow2::amount()
        {
            auto interest = _calculateInterest(_endDate, _endDateICP);
            if (_doesAmortize) {
                return _amortization + interest;
            } else {
                return interest;
            }
        }

        shared_ptr<QCCurrency> IcpClpCashflow2::ccy()
        {
            return _currency;
        }

        QCDate IcpClpCashflow2::date()
        {
            return _settlementDate;
        }

        // Overriding methods in LinearInterestRateCashflow
        std::string IcpClpCashflow2::getType() const
        {
            return "ICPCLP";
        }

        std::shared_ptr<QCCurrency> IcpClpCashflow2::getInitialCcy() const
        {
            return _currency;
        }

        const QCDate& IcpClpCashflow2::getStartDate() const
        {
            return _startDate;
        }

        const QCDate& IcpClpCashflow2::getEndDate() const
        {
            return _endDate;
        }


        const QCDate& IcpClpCashflow2::getSettlementDate() const
        {
            return _settlementDate;
        }


        const DateList& IcpClpCashflow2::getFixingDates() const
        {
            return _fixingDates;
        }


        double IcpClpCashflow2::getNominal() const
        {
            return _nominal;
        }


        double IcpClpCashflow2::nominal(const QCDate& fecha) const
        {
            return _nominal;
        }


        double IcpClpCashflow2::getAmortization() const
        {
            return _amortization;
        }


        double IcpClpCashflow2::amortization() const
        {
            return _amortization;
        }


        double IcpClpCashflow2::interest()
        {
            return _calculateInterest(_endDate, _endDateICP);
        }


        double IcpClpCashflow2::interest(const TimeSeries& fixings)
        {
            try
            {
                _startDateICP = fixings.at(_startDate);
            }
            catch (const exception& e)
            {
                throw invalid_argument("ICP value not found for start date " +
                _startDate.description(false) + ".");
            }
            try
            {
                _endDateICP = fixings.at(_endDate);
            }
            catch (const exception& e)
            {
                throw invalid_argument("ICP value not found for end date " +
                _endDate.description(false) + ".");
            }

            return _calculateInterest(_endDate, _endDateICP);
        }


        double IcpClpCashflow2::fixing()
        {
            return getTna(_endDate, _endDateICP);
        }


        double IcpClpCashflow2::fixing(const TimeSeries& fixings)
        {
            try
            {
                _endDateICP = fixings.at(_endDate);
            }
            catch (const exception& e)
            {
                throw invalid_argument("ICP value not found for end date " +
                                       _endDate.description(false) + ".");
            }

            return getTna(_endDate, _endDateICP);
        }


        double IcpClpCashflow2::accruedInterest(const QCDate& fecha)
        {
            throw invalid_argument("Accrued interest requires ICP value. Use overload with 2nd argument a "
                                   "time series.");
        }


        double IcpClpCashflow2::accruedInterest(const QCDate& fecha, const TimeSeries& fixings)
        {
            auto icpValue = 0.0;
            try
            {
                icpValue = fixings.at(fecha);
            }
            catch (const exception& e)
            {
                throw invalid_argument("ICP value not found for date " +
                                       fecha.description(false) + ".");
            }
            auto fechaOk = fecha;
            return _calculateInterest(fechaOk, icpValue);
        }


        double IcpClpCashflow2::accruedFixing(const QCDate& fecha)
        {
            throw invalid_argument("Accrued fixing requires ICP value. Use overload with 2nd argument a "
                                   "time series.");
        }


        double IcpClpCashflow2::accruedFixing(const QCDate& fecha, const TimeSeries& fixings)
        {
            auto icpValue = 0.0;
            try
            {
                icpValue = fixings.at(fecha);
            }
            catch (const exception& e)
            {
                throw invalid_argument("ICP value not found for date " +
                                       fecha.description(false) + ".");
            }
            auto fechaOk = fecha;
            return getTna(fechaOk, icpValue);
        }


        bool IcpClpCashflow2::doesAmortize() const
        {
            return _doesAmortize;
        }


        // Methods specific to this class

        void IcpClpCashflow2::setTnaDecimalPlaces(unsigned int decimalPlaces)
        {
            _tnaDecimalPlaces = decimalPlaces;
        }


        double IcpClpCashflow2::getTna(QCDate date, double icpValue)
        {
            unsigned int LIMIT_TNA_DECIMAL_PLACES = 15;
            double yf = _rate.yf(_startDate, date);
            if (yf == 0.0)
            {
                return 0.0;
            }

            long double tna = (icpValue / _startDateICP - 1) / yf;

            if (_tnaDecimalPlaces > LIMIT_TNA_DECIMAL_PLACES)
            {
                return tna;
            }
            else
                {
                    long double factor = std::pow(10, _tnaDecimalPlaces);
                    return round(tna * factor) / factor;
                }
        }


        void IcpClpCashflow2::setStartDateICP(double icpValue)
        {
            _startDateICP = icpValue;
        }


        double IcpClpCashflow2::getStartDateICP() const
        {
            return _startDateICP;
        }


        double IcpClpCashflow2::getEndDateICP() const
        {
            return _endDateICP;
        }

        void IcpClpCashflow2::setEndDateICP(double icpValue)
        {
            _endDateICP = icpValue;
        }

        void IcpClpCashflow2::setNominal(double nominal)
        {
            _nominal = nominal;
        }

        void IcpClpCashflow2::setAmortization(double amortization)
        {
            _amortization = amortization;
        }

        shared_ptr<IcpClpCashflow2Wrapper> IcpClpCashflow2::wrap()
        {
            //QCDate,                 /* Start Date */
            //QCDate,                 /* End Date */
            //QCDate,                 /* Settlement Date */
            //double,                 /* Nominal */
            //double,                 /* Amortization */
            //bool,                   /* Amortization is cashflow */
            //shared_ptr<QCCurrency>, /* Nominal Currency (always CLP) */
            //double,                 /* Start date ICP value */
            //double,                 /* End date ICP value */
            //double,                 /* Rate */
            //double,                 /* Interest */
            //double,                 /* Spread */
            //double                  /* Gearing */

            IcpClpCashflow2Wrapper tup = std::make_tuple(_startDate,
                                                        _endDate,
                                                        _settlementDate,
                                                        _nominal,
                                                        _amortization,
                                                        _doesAmortize,
                                                        _currency,
                                                        _startDateICP,
                                                        _endDateICP,
                                                        _rate.getValue(),
                                                        interest(),
                                                        _spread,
                                                        _gearing);

            return std::make_shared<IcpClpCashflowWrapper>(tup);

        }

        double IcpClpCashflow2::getRateValue()
        {
            return _rate.getValue();
        }

        std::string IcpClpCashflow2::getTypeOfRate()
        {
            return _rate.getWealthFactor()->description() + _rate.getYearFraction()->description();
        }

        bool IcpClpCashflow2::_validate()
        {
            bool result = true;
            _validateMsg = "";
            if (_startDate >= _endDate)
            {
                result = false;
                _validateMsg += "Start date (" + _startDate.description();
                _validateMsg += ") is gt o eq to end date (";
                _validateMsg += _endDate.description() + ").";
            }
            if (_settlementDate < _endDate)
            {
                result = false;
                _validateMsg += "Settlement date (" + _settlementDate.description();
                _validateMsg += ") is lt end date (" + _endDate.description() + ").";
            }
            if (_amortization > _nominal)
            {
                result = false;
                _validateMsg += "Amortization is gt nominal.";
            }
            return result;
        }

        double IcpClpCashflow2::_calculateInterest(QCDate& date, double icpValue)
        {
            double tna = getTna(date, icpValue);
            _rate.setValue(tna * _gearing + _spread);
            return _nominal * (_rate.wf(_startDate, date) - 1);
        }

        IcpClpCashflow2::~IcpClpCashflow2()
        {

        }
    }
}


