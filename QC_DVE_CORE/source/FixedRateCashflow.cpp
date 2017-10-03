#include "FixedRateCashflow.h"

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
			_calculateInterest();
		}

		double FixedRateCashflow::amount()
		{
			double amort{ 0.0 };
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

		void FixedRateCashflow::_calculateInterest()
		{
			_interest = _nominal * (_rate.wf(_startDate, _endDate) - 1.0);
		}

		FixedRateCashflow::~FixedRateCashflow()
		{
		}
	}
}
