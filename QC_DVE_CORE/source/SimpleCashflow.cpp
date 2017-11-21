#include "SimpleCashflow.h"

namespace QCode
{
	namespace Financial
	{
		SimpleCashflow::SimpleCashflow(const QCDate& endDate,
									   double nominal,
									   std::shared_ptr<QCCurrency> currency) :
									   _endDate(endDate),
									   _nominal(nominal),
									   _currency(currency)
		{
		}

		double SimpleCashflow::amount()
		{
			return _nominal;
		}

		shared_ptr<QCCurrency> SimpleCashflow::ccy()
		{
			return _currency;
		}

		QCDate SimpleCashflow::date()
		{
			return _endDate;
		}

		void SimpleCashflow::setNominal(double nominal)
		{
			_nominal = nominal;
		}

		shared_ptr<SimpleCashflowWrapper> SimpleCashflow::wrap()
		{
			SimpleCashflowWrapper tup = std::make_tuple(_endDate, _nominal,_currency);

			return std::make_shared<SimpleCashflowWrapper>(tup);
		}

		SimpleCashflow::~SimpleCashflow()
		{
		}




	}
}
