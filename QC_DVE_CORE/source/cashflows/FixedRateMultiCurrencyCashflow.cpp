#include "asset_classes/QCCurrencyConverter.h"
#include "cashflows/FixedRateMultiCurrencyCashflow.h"
#include "HelperFunctions.h"

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

        std::string FixedRateMultiCurrencyCashflow::getType() const {
            return "FixedRateMultiCurrencyCashflow";
        }


		shared_ptr<QCCurrency> FixedRateMultiCurrencyCashflow::settlementCcy()
		{
			return _settlementCurrency;
		}

		void FixedRateMultiCurrencyCashflow::setFxRateIndexValue(double fxRateIndexValue)
		{
			_fxRateIndexValue = fxRateIndexValue;
		}

		double FixedRateMultiCurrencyCashflow::accruedInterestInSettCcy(
                const QCDate& valueDate,
                const TimeSeries& fxRateIndexValues)
		{
			double interest = FixedRateCashflow::accruedInterest(valueDate);

			QCCurrencyConverter ccyConverter;
			if (!QCode::Helpers::isDateInTimeSeries(valueDate, fxRateIndexValues))
			{
				std::string msg = "No value for ";
				msg += _fxRateIndex->getCode() + " and date " + valueDate.description(false) + ".";
				throw invalid_argument(msg);
			}
			else
			{
				double fxRateIndexValue{ fxRateIndexValues.at(valueDate) };
				return ccyConverter.convert(
                        interest,
                        _currency,
                        fxRateIndexValue,
                        *_fxRateIndex);
			}
		}

        // Este method no se expone a Python ya que es todavía experimental y puede que se deje de utilizar
		FXVariation FixedRateMultiCurrencyCashflow::accruedFXVariation(const QCDate& valueDate,
			const TimeSeries& fxRateIndexValues)
		{
			if (!QCode::Helpers::isDateInTimeSeries(_startDate, fxRateIndexValues))
			{
				std::string msg = "No value for ";
				msg += _fxRateIndex->getCode() + " and date " + _startDate.description() + ".";
				throw invalid_argument(msg);
			}

			if (!QCode::Helpers::isDateInTimeSeries(valueDate, fxRateIndexValues))
			{
				std::string msg = "No value for ";
				msg += _fxRateIndex->getCode() + " and date " + valueDate.description() + ".";
				throw invalid_argument(msg);
			}

			double fx1 = fxRateIndexValues.at(_startDate);
			double fx2 = fxRateIndexValues.at(valueDate);

			double interest1 = accruedInterestInSettCcy(valueDate, fxRateIndexValues);
			double interest2 = accruedInterestInSettCcy(valueDate, fxRateIndexValues);

			QCCurrencyConverter ccyConverter;
			return FXVariation { interest2 - interest1,
				                 ccyConverter.convert(
                                         _nominal,
                                         _currency,
                                         fx2,
                                         *_fxRateIndex) -
								 ccyConverter.convert(
                                         _nominal,
                                         _currency,
                                         fx1,
                                         *_fxRateIndex) };
		}

		double FixedRateMultiCurrencyCashflow::getAmortizationInSettCcy(const TimeSeries& fxRateIndexValues)
		{
			double amort = FixedRateCashflow::getAmortization();
			QCCurrencyConverter converter;
			return converter.convert(
                    amort,
                    _currency,
                    fxRateIndexValues.at(_fxRateIndexFixingDate),
                    *_fxRateIndex);
		}

		std::string FixedRateMultiCurrencyCashflow::getFXRateIndexCode() const
		{
			return _fxRateIndex->getCode();
		}

		shared_ptr<FXRateIndex> FixedRateMultiCurrencyCashflow::getFXRateIndex() const
		{
			return _fxRateIndex;
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

		FixedRateMultiCurrencyCashflow::~FixedRateMultiCurrencyCashflow() = default;

        QCDate FixedRateMultiCurrencyCashflow::getFXPublishDate() const {
            return _fxRateIndexFixingDate;
        }

        double FixedRateMultiCurrencyCashflow::settlementAmount() {
            QCCurrencyConverter ccyConverter;
            auto cashflow = _currency->amount(amount());

            cashflow = ccyConverter.convert(
                    cashflow,
                    _currency,
                    _fxRateIndexValue,
                    *_fxRateIndex);

            return _settlementCurrency->amount(cashflow);
        }

        Record FixedRateMultiCurrencyCashflow::record() {
            QCCurrencyConverter ccyConverter;
            auto result = FixedRateCashflow::record();
            result["type_of_cashflow"] = "fixed_rate_multi_currency";
            result["fx_fixing_date"] = _fxRateIndexFixingDate.description(true);
            result["settlement_currency"] = _settlementCurrency->getIsoCode();
            result["fx_rate_index"] = _fxRateIndex->getCode();
            result["fx_rate_index_value"] = _fxRateIndexValue;
            result["amort_sett_currency"] = ccyConverter.convert(
                    _amortization,
                    _currency,
                    _fxRateIndexValue,
                    *_fxRateIndex);
            result["interest_sett_currency"] = ccyConverter.convert(
                    _interest,
                    _currency,
                    _fxRateIndexValue,
                    *_fxRateIndex);

            return result;
        }

        double FixedRateMultiCurrencyCashflow::settlementCurrencyAmount() {
            QCCurrencyConverter ccyConverter;
            auto cashflow = amount();

            return ccyConverter.convert(
                    cashflow,
                    _currency,
                    _fxRateIndexValue,
                    *_fxRateIndex);
        }
    }
}