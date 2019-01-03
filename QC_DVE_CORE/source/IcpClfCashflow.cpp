#include "IcpClfCashflow.h"

namespace QCode
{
	namespace Financial
	{
		IcpClfCashflow::IcpClfCashflow(const QCDate& startDate,
									   const QCDate& endDate,
									   const QCDate& settlementDate,
									   double nominal,
									   double amortization,
									   bool doesAmortize,
									   double spread,
									   double gearing,
									   std::vector<double> icpAndUf) :
									   IcpClpCashflow(
									   startDate,
									   endDate,
									   settlementDate,
									   nominal,
									   amortization,
									   doesAmortize,
									   spread,
									   gearing,
									   icpAndUf[0],
									   icpAndUf[1]),
									   _startDateUF(icpAndUf[2]),
									   _endDateUF(icpAndUf[3])
		{
#ifndef NO_CONSTRUCTOR_VALIDATION
			if (!_validate())
			{
				throw std::invalid_argument(_validateMsg);
			}
#endif
			_currency = std::make_shared<QCCurrency>(QCCLF());
			_traDecimalPlaces = DEFAULT_TRA_DECIMAL_PLACES;
			_rate.setValue(getTra(_endDate, icpAndUf[1], icpAndUf[3]));
		}

		void IcpClfCashflow::setTraDecimalPlaces(unsigned int decimalPlaces)
		{
			_traDecimalPlaces = decimalPlaces;
			_rate.setValue(getTra(_endDate, _endDateICP, _endDateUF));
		}

		double IcpClfCashflow::amount()
		{
			auto interest = _calculateInterest(_endDate, _endDateICP, _endDateUF);
			if (_doesAmortize)
			{
				return _amortization + interest;
			}
			else
			{
				return interest;
			}
		}

		double IcpClfCashflow::getTra(QCDate& accrualDate, double icpValue, double ufValue)
		{
			double tna = getTna(accrualDate, icpValue);
			IcpClpCashflow::_rate.setValue(tna);
			double wfTna = IcpClpCashflow::_rate.wf(_startDate, accrualDate);
			double tra = (wfTna * _startDateUF / ufValue - 1) * 360.0 / _startDate.dayDiff(accrualDate);
			double factor = pow(10, _traDecimalPlaces);
			return round(tra * factor) / factor;
		}

		double IcpClfCashflow::getRateValue()
		{
			return _rate.getValue();
		}

		double IcpClfCashflow::_calculateInterest(QCDate& date, double icpValue, double ufValue)
		{
			// Cálculo de TRA
			double tra = getTra(date, icpValue, ufValue);

			// Seteo del valor de la tasa interna
			_rate.setValue(tra * _gearing + _spread);

			// Cálculo de interés
			return _nominal * (_rate.wf(_startDate, date) - 1);
		}

		double IcpClfCashflow::accruedInterest(QCDate& accrualDate, double icpValue, double ufValue)
		{
			return _calculateInterest(accrualDate, icpValue, ufValue);
		}

		void IcpClfCashflow::setStartDateUf(double ufValue)
		{
			_startDateUF = ufValue;
			_rate.setValue(getTra(_endDate, _endDateICP, _endDateUF));
		}

		double IcpClfCashflow::getStartDateUf() const
		{
			return _startDateUF;
		}

		void IcpClfCashflow::setEndDateUf(double ufValue)
		{
			_endDateUF = ufValue;
			_rate.setValue(getTra(_endDate, _endDateICP, _endDateUF));
		}

		double IcpClfCashflow::getEndDateUf() const
		{
			return _endDateUF;
		}

		shared_ptr<QCCurrency> IcpClfCashflow::ccy()
		{
			return _currency;
		}

		shared_ptr<IcpClfCashflowWrapper> IcpClfCashflow::wrap()
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
				//double,                 /* Start date UF value */
				//double,                 /* End date UF value */
				//double,                 /* Rate */
				//double,                 /* Interest */
				//double,                 /* Spread */
				//double                  /* Gearing */

			//Se precalcula el interés porque eso permite asegurar que el valor
			//de la tasa es consistente con los valores de ICP y UF ingresados.
			double interest = accruedInterest(_endDate, _endDateICP, _endDateUF);
			IcpClfCashflowWrapper tup = std::make_tuple(_startDate,
														_endDate,
														_settlementDate,
														_nominal,
														_amortization,
														_doesAmortize,
														_currency,
														_startDateICP,
														_endDateICP,
														_startDateUF,
														_endDateUF,
														_rate.getValue(),
														interest,
														_spread,
														_gearing);

			return std::make_shared<IcpClfCashflowWrapper>(tup);

		}

		IcpClfCashflow::~IcpClfCashflow()
		{
		}
	}
}
