#include "InterestRateIndex.h"

namespace QCode
{
	namespace Financial
	{
		InterestRateIndex::InterestRateIndex(std::string code,
											 QCInterestRate rate,
											 Tenor startLag,
											 Tenor tenor,
											 QCBusinessCalendar fixingCalendar,
											 QCBusinessCalendar valueCalendar,
											 std::shared_ptr<QCCurrency> ccy) :
											 FinancialIndex(FinancialIndex::InterestRate, code),
											 _rate(rate),
											 _startLag(startLag),
											 _tenor(tenor),
											 _fixingCalendar(fixingCalendar),
											 _valueCalendar(valueCalendar),
											 _ccy(ccy)
		{
		}

		void InterestRateIndex::setRateValue(double rateValue)
		{
			_rate.setValue(rateValue);
		}

		QCDate InterestRateIndex::getStartDate(QCDate& fixingDate)
		{
			return QCDate();
		}

		QCDate InterestRateIndex::getEndDate(QCDate& fixingDate)
		{
			return QCDate();
		}

		Tenor InterestRateIndex::getTenor()
		{
			return _tenor;
		}

		QCInterestRate InterestRateIndex::getRate()
		{
			return _rate;
		}

		unsigned int InterestRateIndex::getDaysOfStartLag()
		{
			return _startLag.getDays();
		}

		unsigned int InterestRateIndex::getMonthsOfTenor()
		{
			return _tenor.getMonths();
		}

		InterestRateIndex::~InterestRateIndex()
		{
		}
	}
}