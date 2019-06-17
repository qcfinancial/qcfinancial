#include<memory>

#include "IcpClpCashflow.h"
#include "QCCurrency.h"

namespace QCode
{
	namespace Financial
	{
		IcpClpCashflow::IcpClpCashflow(const QCDate& startDate,
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
		}

		void IcpClpCashflow::setTnaDecimalPlaces(unsigned int decimalPlaces)
		{
			_tnaDecimalPlaces = decimalPlaces;
			_rate.setValue(getTna(_endDate, _endDateICP));
		}

		double IcpClpCashflow::amount()
		{
			auto interest = _calculateInterest(_endDate, _endDateICP);
			if (_doesAmortize)
			{
				return _amortization + interest;
			}
			else
			{
				return interest;
			}
		}

		double IcpClpCashflow::getTna(QCDate date, double icpValue)
		{
			unsigned int LIMIT_TNA_DECIMAL_PLACES = 15;
			double yf = _rate.yf(_startDate, date);
			double tna = (icpValue / _startDateICP - 1) / yf;
			if (_tnaDecimalPlaces > LIMIT_TNA_DECIMAL_PLACES)
			{
				return tna;
			}
			else
			{
				double factor = std::pow(10, _tnaDecimalPlaces);
				return round(tna * factor) / factor;
			}
		}

		double IcpClpCashflow::accruedInterest(QCDate& accrualDate, double icpValue)
		{
			return _calculateInterest(accrualDate, icpValue);
		}

		void IcpClpCashflow::setStartDateICP(double icpValue)
		{
			_startDateICP = icpValue;
			_rate.setValue(getTna(_endDate, _endDateICP));
		}

		double IcpClpCashflow::getStartDateICP() const
		{
			return _startDateICP;
		}

		void IcpClpCashflow::setEndDateICP(double icpValue)
		{
			_endDateICP = icpValue;
			_rate.setValue(getTna(_endDate, _endDateICP));
		}

		double IcpClpCashflow::getEndDateICP() const
		{
			return _endDateICP;
		}

		shared_ptr<QCCurrency> IcpClpCashflow::ccy()
		{
			return _currency;
		}

		QCDate IcpClpCashflow::date()
		{
			return _settlementDate;
		}

		void IcpClpCashflow::setNominal(double nominal)
		{
			_nominal = nominal;
		}

		double IcpClpCashflow::getNominal() const
		{
			return _nominal;
		}

		void IcpClpCashflow::setAmortization(double amortization)
		{
			_amortization = amortization;
		}

		double IcpClpCashflow::getAmortization() const
		{
			return _amortization;
		}

		shared_ptr<IcpClpCashflowWrapper> IcpClpCashflow::wrap()
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

			IcpClpCashflowWrapper tup = std::make_tuple(_startDate,
														_endDate,
														_settlementDate,
														_nominal,
														_amortization,
														_doesAmortize,
														_currency,
														_startDateICP,
														_endDateICP,
														_rate.getValue(),
														accruedInterest(_endDate, _endDateICP),
														_spread,
														_gearing);

			return std::make_shared<IcpClpCashflowWrapper>(tup);
		}

		double IcpClpCashflow::getRateValue()
		{
			return _rate.getValue();
		}

		std::string IcpClpCashflow::getTypeOfRate()
		{
			return _rate.getWealthFactor()->description() + _rate.getYearFraction()->description();
		}

		QCDate IcpClpCashflow::getStartDate()
		{
			return _startDate;
		}

		QCDate IcpClpCashflow::getEndDate()
		{
			return _endDate;
		}

		bool IcpClpCashflow::_validate()
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

		double IcpClpCashflow::_calculateInterest(QCDate& date, double icpValue)
		{
			double tna = getTna(date, icpValue);
			_rate.setValue(tna * _gearing + _spread);
			return _nominal * (_rate.wf(_startDate, date) - 1);
		}

		IcpClpCashflow::~IcpClpCashflow()
		{
		}
	}
}