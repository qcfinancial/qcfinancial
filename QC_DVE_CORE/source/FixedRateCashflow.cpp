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

		const QCDate& FixedRateCashflow::getStartDate() const
		{
			return _startDate;
		}

		const QCDate& FixedRateCashflow::getEndDate() const
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

		void FixedRateCashflow::setAmortization(double amortization)
		{
			_amortization = amortization;
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
			if (Cashflow::isExpired(valueDate) || valueDate > _endDate)
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

		FixedRateCashflow::~FixedRateCashflow()
		{
		}
	}
}
