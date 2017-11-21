#include "SimpleMultiCurrencyCashflow.h"

namespace QCode
{
	namespace Financial
	{
		SimpleMultiCurrencyCashflow::SimpleMultiCurrencyCashflow(const QCDate& endDate,
																 double nominal,
																 std::shared_ptr<QCCurrency> currency,
																 const QCDate& fxRateIndexFixingDate,
																 std::shared_ptr<QCCurrency> settlementCurrency,
																 std::shared_ptr<FXRateIndex> fxRateIndex,
																 double fxRateIndexValue) :
																 SimpleCashflow(endDate, nominal, currency),
																 _fxRateIndexFixingDate(fxRateIndexFixingDate),
																 _settlementCurrency(settlementCurrency),
																 _fxRateIndex(fxRateIndex),
																 _fxRateIndexValue(fxRateIndexValue)
		{
		}

		double SimpleMultiCurrencyCashflow::amount()
		{
			if (_fxRateIndex->strongCcyCode() == _currency->getIsoCode())
			{
				return _nominal * _fxRateIndexValue;
			}
			else
			{
				return _nominal / _fxRateIndexValue;
			}
		}

		double SimpleMultiCurrencyCashflow::nominal()
		{
			return _nominal;
		}

		shared_ptr<QCCurrency> SimpleMultiCurrencyCashflow::settlementCcy()
		{
			return _settlementCurrency;
		}

		void SimpleMultiCurrencyCashflow::setFxRateIndexValue(double fxRateIndexValue)
		{
			_fxRateIndexValue = fxRateIndexValue;
		}

		shared_ptr<SimpleMultiCurrencyCashflowWrapper> SimpleMultiCurrencyCashflow::wrap()
		{
			SimpleMultiCurrencyCashflowWrapper tup = std::make_tuple(_endDate,
																	 _nominal,
																	 _currency,
																	 _fxRateIndexFixingDate,
																	 _settlementCurrency,
																	 _fxRateIndex,
																	 _fxRateIndexValue,
																	 amount());

			return std::make_shared<SimpleMultiCurrencyCashflowWrapper>(tup);
		}

	}
}