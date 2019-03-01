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

		double FixedRateMultiCurrencyCashflow::accruedInterest(const QCDate& valueDate, const QCDate& fxRateIndexDate, const TimeSeries& fxRateIndexValues)
		{
			double interest = FixedRateCashflow::accruedInterest(valueDate);

			QCCurrencyConverter ccyConverter;
			if (!isDateInTimeSeries(fxRateIndexDate, fxRateIndexValues))
			{
				std::string msg = "No value for ";
				msg += _fxRateIndex->getCode() + " and date " + fxRateIndexDate.description() + ".";
				throw invalid_argument(msg);
			}
			else
			{
				double fxRateIndexValue{ fxRateIndexValues.at(fxRateIndexDate) };
				return ccyConverter.convert(interest, _currency, fxRateIndexValue, *_fxRateIndex);
			}
		}

		FXVariation FixedRateMultiCurrencyCashflow::accruedFXVariation(const QCDate& valueDate,
			const TimeSeries& fxRateIndexValues)
		{
			if (!isDateInTimeSeries(_startDate, fxRateIndexValues))
			{
				std::string msg = "No value for ";
				msg += _fxRateIndex->getCode() + " and date " + _startDate.description() + ".";
				throw invalid_argument(msg);
			}

			//QCDate _valueDate{ valueDate };
			if (!isDateInTimeSeries(valueDate, fxRateIndexValues))
			{
				std::string msg = "No value for ";
				msg += _fxRateIndex->getCode() + " and date " + valueDate.description() + ".";
				throw invalid_argument(msg);
			}

			double fx1 = fxRateIndexValues.at(_startDate);
			double fx2 = fxRateIndexValues.at(valueDate);

			double interest1 = accruedInterest(valueDate, _startDate, fxRateIndexValues);
			double interest2 = accruedInterest(valueDate, valueDate, fxRateIndexValues);

			QCCurrencyConverter ccyConverter;
			return FXVariation { interest2 - interest1,
				                 ccyConverter.convert(_nominal, _currency, fx2, *_fxRateIndex) -
								 ccyConverter.convert(_nominal, _currency, fx1, *_fxRateIndex) };
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