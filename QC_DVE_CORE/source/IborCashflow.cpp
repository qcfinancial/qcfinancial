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
			if (!_validate())
			{
				throw std::invalid_argument(_validateMsg);
			}
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

		QCDate IborCashflow::getStartDate() const
		{
			return _startDate;
		}

		QCDate IborCashflow::getEndDate() const
		{
			return _endDate;
		}

		QCDate IborCashflow::getFixingDate() const
		{
			return _fixingDate;
		}

		void IborCashflow::setNominal(double nominal)
		{
			_nominal = nominal;
		}

		double IborCashflow::getNominal() const
		{
			return _nominal;
		}

		void IborCashflow::setAmortization(double amortization)
		{
			_amortization = amortization;
		}

		double IborCashflow::getAmortization() const
		{
			return _amortization;
		}

		void IborCashflow::setInterestRateValue(double value)
		{
			_index->setRateValue(value);
			_calculateInterest();
		}

		double IborCashflow::getInterestRateValue() const
		{
			return _index->getRate().getValue();
		}

		double IborCashflow::accruedInterest(const QCDate& valueDate)
		{
			if (Cashflow::isExpired(valueDate) || valueDate < _startDate)
			{
				return 0.0;
			}
			QCDate temp = valueDate;
			double indexValue = _index->getRate().getValue();
			_index->setRateValue(indexValue * _gearing + _spread);
			double result = _nominal * (_index->getRate().wf(_startDate, temp) - 1.0);
			_index->setRateValue(indexValue);
			return result;
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
			// derivadas. Se dar� de alta una sobrecarga de QCInterestRate.wf que considere
			// tambi�n gearing y spread (o se agregan como par�metros opcionales en el
			// m�todo ya existente).
			_interest = _nominal * (_index->getRate().wf(_startDate, _endDate) - 1.0);
			_index->setRateValue(rateValue);
		}

		bool IborCashflow::_validate()
		{
			bool result;
			_validateMsg = "";
			if (_startDate >= _endDate)
			{
				result = false;
				_validateMsg += "Start date (" + _startDate.description();
				_validateMsg += ") is gt or eq to end date (" + _endDate.description() + ").";
			}
			if (_fixingDate > _startDate)
			{
				result = false;
				_validateMsg += "Fixing date (" + _fixingDate.description() + ") ";
				_validateMsg +=	"is gt start date (" + _startDate.description() + ").";
			}
			if (_settlementDate < _endDate)
			{
				result = false;
				_validateMsg += "Settlement date (" + _settlementDate.description() + ") ";
				_validateMsg +=	"is lt end date (" + _endDate.description() + ").";
			}
			return result;
		}

		IborCashflow::~IborCashflow()
		{
		}
	}
}
