#include "QCCurrencyConverter.h"
#include "FixedRateMultiCurrencyCashflow.h"

namespace QCode
{
	namespace Financial
	{
		FixedRateMultiCurrencyCashflow::FixedRateMultiCurrencyCashflow(
			const QCDate& startDate,
			const QCDate& endDate,
			const QCDate& settlementDate,
			double nominal,
			double amortization,
			bool doesAmortize,
			const QCInterestRate& rate,
			std::shared_ptr<QCCurrency> notionalCurrency,
			const QCDate& fxRateIndexFixingDate,
			std::shared_ptr<QCCurrency> settlementCurrency,
			std::shared_ptr<FXRateIndex> fxRateIndex,
			double fxRateIndexValue) :
			FixedRateCashflow(startDate, endDate, settlementDate, nominal,
			amortization, doesAmortize, rate, notionalCurrency),
			_fxRateIndexFixingDate(fxRateIndexFixingDate),
			_settlementCurrency(settlementCurrency),
			_fxRateIndex(fxRateIndex),
			_fxRateIndexValue(fxRateIndexValue)
		{
#ifndef NO_CONSTRUCTOR_VALIDATION
			if (!_validate())
			{
				throw std::invalid_argument(_validateMsg);
			}
#endif
		}


		double FixedRateMultiCurrencyCashflow::amount()
		{
			QCCurrencyConverter ccyConverter;
			double cashflow = _interest;
			if (_doesAmortize)
			{
				cashflow += _amortization;
			}

			return ccyConverter.convert(cashflow, _currency, _fxRateIndexValue, *_fxRateIndex);
		}

		shared_ptr<QCCurrency> FixedRateMultiCurrencyCashflow::settlementCcy()
		{
			return _settlementCurrency;
		}

		void FixedRateMultiCurrencyCashflow::setFxRateIndexValue(double fxRateIndexValue)
		{
			_fxRateIndexValue = fxRateIndexValue;
		}

		std::shared_ptr<FixedRateMultiCurrencyCashflowWrapper> FixedRateMultiCurrencyCashflow::wrap()
		{
			QCCurrencyConverter ccyConverter;
			FixedRateMultiCurrencyCashflowWrapper tup = std::make_tuple(
				_startDate,
				_endDate,
				_settlementDate,
				_nominal,
				_amortization,
				_interest,
				_doesAmortize,
				_currency,
				_rate,
				_fxRateIndexFixingDate,
				_settlementCurrency,
				_fxRateIndex,
				_fxRateIndexValue,
				ccyConverter.convert(_amortization, _currency, _fxRateIndexValue, *_fxRateIndex),
				ccyConverter.convert(_interest, _currency, _fxRateIndexValue, *_fxRateIndex)
				);

			return std::make_shared<FixedRateMultiCurrencyCashflowWrapper>(tup);
		}

		bool FixedRateMultiCurrencyCashflow::_validate()
		{
			bool result = true;
			_validateMsg += "";
			if (_fxRateIndexFixingDate > _settlementDate)
			{
				result = false;
				_validateMsg += "Fx rate fixing date (";
				_validateMsg += _fxRateIndexFixingDate.description();
				_validateMsg += ") is gt settlement date (";
				_validateMsg += _settlementDate.description();
				_validateMsg += ").";
			}
		
			return result;
		}

		FixedRateMultiCurrencyCashflow::~FixedRateMultiCurrencyCashflow()
		{
		}
	}
}