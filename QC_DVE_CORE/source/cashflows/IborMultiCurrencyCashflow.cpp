#include "asset_classes/QCCurrencyConverter.h"
#include "cashflows/IborMultiCurrencyCashflow.h"
#include "HelperFunctions.h"

namespace QCode
{
	namespace Financial
	{
		IborMultiCurrencyCashflow::IborMultiCurrencyCashflow(std::shared_ptr<InterestRateIndex> index,
			const QCDate& startDate,
			const QCDate& endDate,
			const QCDate& fixingDate,
			const QCDate& settlementDate,
			double nominal,
			double amortization,
			bool doesAmortize,
			std::shared_ptr<QCCurrency> nominalCurrency,
			double spread,
			double gearing,
			const QCDate& fxRateIndexFixingDate,
			std::shared_ptr<QCCurrency> settlementCurrency,
			std::shared_ptr<FXRateIndex> fxRateIndex,
			double fxRateIndexValue) : IborCashflow(index, startDate, endDate, fixingDate, settlementDate, nominal,
			amortization, doesAmortize, nominalCurrency, spread, gearing), _fxRateIndexFixingDate(fxRateIndexFixingDate),
			_settlementCurrency(settlementCurrency), _fxRateIndex(fxRateIndex), _fxRateIndexValue(fxRateIndexValue)
		{
#ifndef NO_CONSTRUCTOR_VALIDATION
			if (!_validate())
			{
				throw std::invalid_argument(_validateMsg);
			}
#endif
		}

        std::string IborMultiCurrencyCashflow::getType() const {
            return "IborMultiCurrencyCashflow";
        }

		double IborMultiCurrencyCashflow::amount()
		{
			QCCurrencyConverter ccyConverter;
			double cashflow = _interest;
			if (_doesAmortize)
			{
				cashflow += _amortization;
			}

			return ccyConverter.convert(cashflow, _currency, _fxRateIndexValue, *_fxRateIndex);
		}

		shared_ptr<QCCurrency> IborMultiCurrencyCashflow::settlementCcy()
		{
			return _settlementCurrency;
		}

		void IborMultiCurrencyCashflow::setFxRateIndexValue(double fxRateIndexValue)
		{
			_fxRateIndexValue = fxRateIndexValue;
		}

		double IborMultiCurrencyCashflow::accruedInterest(const QCDate& valueDate, const QCDate& fxRateIndexDate, const TimeSeries& fxRateIndexValues)
		{
			double interest = IborCashflow::accruedInterest(valueDate);

			QCCurrencyConverter ccyConverter;
			if (!QCode::Helpers::isDateInTimeSeries(fxRateIndexDate, fxRateIndexValues))
			{
				std::string msg = "No value for ";
				msg += _fxRateIndex->getCode() + " and date " + fxRateIndexDate.description(false) + ".";
				throw invalid_argument(msg);
			}
			else
			{
				double fxRateIndexValue{ fxRateIndexValues.at(fxRateIndexDate) };
				return ccyConverter.convert(interest, _currency, fxRateIndexValue, *_fxRateIndex);
			}
		}

		FXVariation IborMultiCurrencyCashflow::accruedFXVariation(const QCDate& valueDate,
			const TimeSeries& fxRateIndexValues)
		{
			if (!QCode::Helpers::isDateInTimeSeries(_startDate, fxRateIndexValues))
			{
				std::string msg = "No value for ";
				msg += _fxRateIndex->getCode() + " and date " + _startDate.description() + ".";
				throw invalid_argument(msg);
			}

			//QCDate _valueDate{ valueDate };
			if (!QCode::Helpers::isDateInTimeSeries(valueDate, fxRateIndexValues))
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
			return FXVariation{ interest2 - interest1,
				ccyConverter.convert(_nominal, _currency, fx2, *_fxRateIndex) -
				ccyConverter.convert(_nominal, _currency, fx1, *_fxRateIndex) };
		}

		std::shared_ptr<IborMultiCurrencyCashflowWrapper> IborMultiCurrencyCashflow::wrap()
		{
			QCCurrencyConverter ccyConverter;
			IborMultiCurrencyCashflowWrapper tup = std::make_tuple(
				_startDate,
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
				_gearing,
				_rateValue,
				_fxRateIndexFixingDate,
				_settlementCurrency,
				_fxRateIndex,
				_fxRateIndexValue,
				ccyConverter.convert(_amortization, _currency, _fxRateIndexValue, *_fxRateIndex),
				ccyConverter.convert(_interest, _currency, _fxRateIndexValue, *_fxRateIndex)
				);

			return std::make_shared<IborMultiCurrencyCashflowWrapper>(tup);
		}

		bool IborMultiCurrencyCashflow::_validate()
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

		IborMultiCurrencyCashflow::~IborMultiCurrencyCashflow()
		{
		}

        QCDate IborMultiCurrencyCashflow::getFXFixingDate() const {
            return _fxRateIndexFixingDate;
        }
    }
}