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
			_currency = std::make_shared<QCCurrency>(QCCLF());
		}

		double IcpClfCashflow::_calculateInterest(QCDate& date, double icpValue, double ufValue)
		{
			// Calcular la fracción de año correspondiente
			auto yf = _rate.yf(_startDate, date);

			// Factor para redondeo de TNA
			double factor = std::pow(10, 4);

			//Cálculo de TNA
			auto tna = (icpValue / _startDateICP - 1) / yf;
			tna = round(tna * factor) / factor;
			_rate.setValue(tna);

			// Se redefine para redondeo de TRA
			factor = std::pow(10, 6);

			// Cálculo de TRA
			double tra = (_rate.wf(_startDate, date) * _startDateUF / ufValue - 1) / yf;
			tra = round(tra * factor) / factor;
			_rate.setValue(tra * _gearing + _spread);

			// Cálculo de interés
			return _nominal * (_rate.wf(_startDate, date) - 1);
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

		double IcpClfCashflow::accruedInterest(QCDate& accrualDate, double icpValue, double ufValue)
		{
			return _currency->amount(_calculateInterest(accrualDate, icpValue, ufValue));
		}

		void IcpClfCashflow::setStartDateUf(double ufValue)
		{
			_startDateUF = ufValue;
		}

		void IcpClfCashflow::setEndDateUf(double ufValue)
		{
			_endDateUF = ufValue;
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
