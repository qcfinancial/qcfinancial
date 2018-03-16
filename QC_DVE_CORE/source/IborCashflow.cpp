#include "IborCashflow.h"

#include<iostream>

namespace QCode
{
	namespace Financial
	{
		IborCashflow::IborCashflow(std::shared_ptr<InterestRateIndex> index,
								   const QCDate& startDate,
								   const QCDate& endDate,
								   const QCDate& fixingDate,
								   const QCDate& settlementDate,
								   double nominal,
								   double amortization,
								   bool doesAmortize,
								   shared_ptr<QCCurrency> currency,
								   double spread,
								   double gearing) :
								   _index(index),
								   _startDate(startDate),
								   _endDate(endDate),
								   _fixingDate(fixingDate),
								   _settlementDate(settlementDate),
								   _nominal(nominal),
								   _amortization(amortization),
								   _doesAmortize(doesAmortize),
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

		void IborCashflow::setNominal(double nominal)
		{
			_nominal = nominal;
		}

		void IborCashflow::setAmortization(double amortization)
		{
			_amortization = amortization;
		}

		void IborCashflow::setInterestRateValue(double value)
		{
			_index->setRateValue(value);
			_calculateInterest();
		}

		QCDate IborCashflow::getFixingDate()
		{
			return _fixingDate;
		}

		shared_ptr<IborCashflowWrapper> IborCashflow::wrap()
		{
			IborCashflowWrapper tup = std::make_tuple(_startDate,
													  _endDate,
													  _fixingDate,
													  _settlementDate,
													  _nominal,
													  _amortization,
													  _interest,
													  _doesAmortize,
													  amount(),
													  _currency,
													  _index->getCode(),
													  _index->getRate(),
													  _spread,
													  _gearing);

			return std::make_shared<IborCashflowWrapper>(tup);
		}

		void IborCashflow::_calculateInterest()
		{
			double rateValue = _index->getRate().getValue();
			_index->setRateValue(rateValue * _gearing + _spread);
			// Con este procedimiento vamos a tener un problema al momento de calcular
			// derivadas. Se dará de alta una sobrecarga de QCInterestRate.wf que considere
			// también gearing y spread (o se agregan como parámetros opcionales en el
			// método ya existente).
			_interest = _nominal * (_index->getRate().wf(_startDate, _endDate) - 1.0);
			_index->setRateValue(rateValue);
		}

		IborCashflow::~IborCashflow()
		{
		}
	}
}
