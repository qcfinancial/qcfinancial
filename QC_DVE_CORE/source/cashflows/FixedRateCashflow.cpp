#include "cashflows/FixedRateCashflow.h"

namespace QCode
{
	namespace Financial
	{
		FixedRateCashflow::FixedRateCashflow(const QCDate& startDate,
											 const QCDate& endDate,
											 const QCDate& settlementDate,
											 double nominal,
											 double amortization,
											 bool doesAmortize,
											 const QCInterestRate& rate,
											 shared_ptr<QCCurrency> currency) :
											 _startDate(startDate),
											 _endDate(endDate),
											 _settlementDate(settlementDate),
											 _nominal(nominal),
											 _amortization(amortization),
											 _doesAmortize(doesAmortize),
											 _rate(rate),
											 _currency(currency)
		{
#ifndef NO_CONSTRUCTOR_VALIDATION
			if (!_validate())
			{
				throw std::invalid_argument(_validateMsg);
			}
#endif
			_calculateInterest();
		}

		double FixedRateCashflow::amount()
		{
			double amort{ 0.0 };
			_calculateInterest();
			if (_doesAmortize)
			{
				amort = _amortization;
			}
			return  amort + _interest;
		}

		shared_ptr<QCCurrency> FixedRateCashflow::ccy()
		{
			return _currency;
		}

		QCDate FixedRateCashflow::date()
		{
			return _settlementDate;
		}

		QCDate FixedRateCashflow::getStartDate() const
		{
			return _startDate;
		}

		QCDate FixedRateCashflow::getEndDate() const
		{
			return _endDate;
		}

		double FixedRateCashflow::getNominal() const
		{
			return _nominal;
		}

		void FixedRateCashflow::setNominal(double nominal)
		{
			_nominal = nominal;
			_calculateInterest();
		}

		double FixedRateCashflow::getAmortization() const
		{
			return _amortization;
		}

		void FixedRateCashflow::setAmortization(double amortization)
		{
			_amortization = amortization;
		}

        const QCInterestRate& FixedRateCashflow::getRate() const
        {
            return _rate;
        }

        shared_ptr<FixedRateCashflowWrapper> FixedRateCashflow::wrap()
		{
			FixedRateCashflowWrapper tup = std::make_tuple(_startDate,
								   _endDate,
								   _settlementDate,
								   _nominal,
								   _amortization,
								   _interest,
								   _doesAmortize,
								   _currency,
								   _rate);

			return std::make_shared<FixedRateCashflowWrapper>(tup);
		}

		double FixedRateCashflow::accruedInterest(const QCDate& valueDate)
		{
			if (Cashflow::isExpired(valueDate) || valueDate < _startDate)
			{
				return 0.0;
			}
			QCDate temp = valueDate;
			return _nominal * (_rate.wf(_startDate, temp) - 1.0);
		}

		void FixedRateCashflow::_calculateInterest()
		{
			_interest = _nominal * (_rate.wf(_startDate, _endDate) - 1.0);
		}

		bool FixedRateCashflow::_validate()
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
				_validateMsg +=	") is lt end date (" + _endDate.description() + ").";
			}
			if (_amortization > _nominal)
			{
				result = false;
				_validateMsg += "Amortization is gt nominal.";
			}
			return result;
		}

		FixedRateCashflow::~FixedRateCashflow()
		{
		}

    }
}
