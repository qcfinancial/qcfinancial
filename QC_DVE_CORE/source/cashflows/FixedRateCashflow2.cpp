#include "cashflows/FixedRateCashflow2.h"

namespace QCode
{
	namespace Financial
	{
		FixedRateCashflow2::FixedRateCashflow2(const QCDate& startDate,
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
			_fixingDates.resize(1);
			_fixingDates.at(0) = _startDate;
		}


		shared_ptr<QCCurrency> FixedRateCashflow2::getInitialCcy() const
		{
			return _currency;
		}


		double FixedRateCashflow2::_calculateInterest(const QCDate& fecha)
		{
			auto fecha1 = fecha;
			return _nominal * (_rate.wf(_startDate, fecha1) - 1.0);
		}


		double FixedRateCashflow2::amount()
		{
			double amort{ 0.0 };
			if (_doesAmortize)
			{
				amort = _amortization;
			}
			return  amort + _calculateInterest(_endDate);
		}


		shared_ptr<QCCurrency> FixedRateCashflow2::ccy()
		{
			return _currency;
		}


		QCDate FixedRateCashflow2::date()
		{
			return _settlementDate;
		}


		const QCDate& FixedRateCashflow2::getStartDate() const
		{
			return _startDate;
		}


		const QCDate& FixedRateCashflow2::getEndDate() const
		{
			return _endDate;
		}


		const QCDate& FixedRateCashflow2::getSettlementDate() const
		{
			return _settlementDate;
		}


		const DateList& FixedRateCashflow2::getFixingDates() const
		{
			return _fixingDates;
		}


		double FixedRateCashflow2::getNominal() const
		{
			return _nominal;
		}


		double FixedRateCashflow2::nominal(const QCDate& fecha) const
		{
			return _nominal;
		}


		double FixedRateCashflow2::getAmortization() const
		{
			return _amortization;
		}


		double FixedRateCashflow2::amortization() const
		{
			double result = _doesAmortize ? _amortization : 0.0;
			return result;
		}


		double FixedRateCashflow2::interest()
		{
			return _calculateInterest(_endDate);
		}


		double FixedRateCashflow2::interest(const TimeSeries& fixings)
		{
			return interest();
		}


		double FixedRateCashflow2::fixing()
		{
			return _rate.getValue();
		}


		double FixedRateCashflow2::fixing(const TimeSeries& fixings)
		{
			return fixing();
		}


		double FixedRateCashflow2::accruedInterest(const QCDate& valueDate)
		{
			if (valueDate < _startDate || _endDate >= valueDate)
			{
				return 0.0;
			}
			QCDate temp = valueDate;
			return _nominal * (_rate.wf(_startDate, temp) - 1.0);
		}


		double FixedRateCashflow2::accruedInterest(const QCDate& valueDate, const TimeSeries& fixings)
		{
			return accruedInterest(valueDate);
		}


		double FixedRateCashflow2::accruedFixing(const QCDate& fecha)
		{
			return _rate.getValue();
		}

		double FixedRateCashflow2::accruedFixing(const QCDate& fecha, const TimeSeries& fixings)
		{
			return accruedFixing(fecha);
		}


		bool FixedRateCashflow2::doesAmortize() const
		{
			return _doesAmortize;
		}


		bool FixedRateCashflow2::_validate()
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


		shared_ptr<FixedRateCashflowWrapper> FixedRateCashflow2::wrap()
		{
			FixedRateCashflowWrapper tup = std::make_tuple(_startDate,
				_endDate,
				_settlementDate,
				_nominal,
				_amortization,
				interest(),
				_doesAmortize,
				_currency,
				_rate);

			return std::make_shared<FixedRateCashflowWrapper>(tup);
		}


		double FixedRateCashflow2::getInterestRateValue() const
		{
			auto tasa = _rate;
			return tasa.getValue();
		}


		std::string FixedRateCashflow2::getInterestRateType() const
		{
			auto tasa = _rate;
			auto yf = tasa.getYearFraction()->description();
			auto wf = tasa.getWealthFactor()->description();
			return wf + yf;
		}


		FixedRateCashflow2::~FixedRateCashflow2()
		{
		}
	}
}
