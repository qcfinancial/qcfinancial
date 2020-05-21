#include "asset_classes/InterestRateIndex.h"

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
			auto dias = _startLag.getDays();
			return _fixingCalendar.shift(fixingDate, dias);
		}

		QCDate InterestRateIndex::getEndDate(QCDate& fixingDate) 
		{
			auto dias = _startLag.getDays();
			auto startDate = _fixingCalendar.shift(fixingDate, dias);
			auto meses = _tenor.getMonths();
			return _valueCalendar.nextBusinessDay(startDate.addMonths(meses));
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
			return _tenor.getMonths() + _tenor.getYears() * 12;
		}

		InterestRateIndex::~InterestRateIndex()
		{
		}

    }
}