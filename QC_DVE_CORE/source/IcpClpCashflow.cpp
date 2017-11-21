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
			_currency = std::make_shared<QCCurrency>(QCCLP());
		}

		double IcpClpCashflow::_calculateInterest(QCDate& date, double icpValue)
		{
			auto yf = _rate.yf(_startDate, date);
			auto tna = (icpValue / _startDateICP - 1) / yf;
			double factor = std::pow(10, 4);
			tna = round(tna * factor) / factor;
			_rate.setValue(tna * _gearing + _spread);
			return _nominal * (_rate.wf(_startDate, date) - 1);
		}

		double IcpClpCashflow::amount()
		{
			auto interest = _calculateInterest(_endDate, _endDateICP);
			if (_doesAmortize)
			{
				return _nominal + interest;
			}
			else
			{
				return interest;
			}
		}

		double IcpClpCashflow::accruedInterest(QCDate& accrualDate, double icpValue)
		{
			return _calculateInterest(accrualDate, icpValue);
		}

		void IcpClpCashflow::setStartDateICP(double icpValue)
		{
			_startDateICP = icpValue;
		}

		void IcpClpCashflow::setEndDateICP(double icpValue)
		{
			_endDateICP = icpValue;
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

		void IcpClpCashflow::setAmortization(double amortization)
		{
			_amortization = amortization;
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
														_calculateInterest(_endDate, _endDateICP),
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

		IcpClpCashflow::~IcpClpCashflow()
		{
		}
	}
}