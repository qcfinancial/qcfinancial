#include "IborCashflow.h"

#include<iostream>

namespace QCode
{
	namespace Financial
	{
		IborCashflow::IborCashflow(InterestRateIndex iborIndexCode,
								   const QCDate& startDate,
								   const QCDate& endDate,
								   const QCDate& fixingDate,
								   const QCDate& settlementDate,
								   double nominal,
								   double amortizationn,
								   bool doesAmortize,
								   const QCInterestRate& rate,
								   shared_ptr<QCCurrency> currency,
								   double spread,
								   double gearing) :
								   _iborIndexCode(iborIndexCode),
								   _startDate(startDate),
								   _endDate(endDate),
								   _fixingDate(fixingDate),
								   _settlementDate(settlementDate),
								   _nominal(nominal),
								   _amortization(amortization),
								   _doesAmortize(doesAmortize),
								   _rate(rate),
								   _currency(currency),
								   _spread(spread),
								   _gearing(gearing)
		{
			_calculateInterest();

		}

		double IborCashflow::amount()
		{
			double amort{ 0.0 };
			if (_doesAmortize)
			{
				amort = _amortization;
			}
			return  amort + _interest;
		}

		shared_ptr<QCCurrency> IborCashflow::ccy()
		{
			return _currency;
		}

		QCDate IborCashflow::date()
		{
			return _settlementDate;
		}

		shared_ptr<IborCashflowWrapper> IborCashflow::wrap()
		{
			std::string code = irIndexCode(_iborIndexCode);
			IborCashflowWrapper tup = std::make_tuple(_startDate,
													  _endDate,
													  _fixingDate,
													  _settlementDate,
													  _nominal,
													  _amortization,
													  _interest,
													  _doesAmortize,
													  _currency,
													  code,
													  _rate,
													  _spread,
													  _gearing);

			return std::make_shared<IborCashflowWrapper>(tup);
		}

		void IborCashflow::_calculateInterest()
		{
			double rateValue = _rate.getValue();
			_rate.setValue(rateValue * _gearing + _spread);
			// Con este procedimiento vamos a tener un problema al momento de calcular
			// derivadas. Se dará de alta una sobrecarga de QCInterestRate.wf que considere
			// también gearing y spread (o se agregan como parámetros opcionales en el
			// método ya existente).
			_interest = _nominal * (_rate.wf(_startDate, _endDate) - 1.0);
			_rate.setValue(rateValue);
		}

		IborCashflow::~IborCashflow()
		{
		}
	}
}
