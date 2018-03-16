#ifndef INTERESTRATEINDEX_H
#define INTERESTRATEINDEX_H

#include<string>
#include<memory>

#include "Tenor.h"
#include "QCDate.h"
#include "QCCurrency.h"
#include "FinancialIndex.h"
#include "QCInterestRate.h"
#include "QCBusinessCalendar.h"

namespace QCode
{
	namespace Financial
	{
		class InterestRateIndex : public FinancialIndex
		{
		public:
			InterestRateIndex(std::string code,
							  QCInterestRate rate,
							  Tenor startLag,
							  Tenor tenor,
							  QCBusinessCalendar fixingCalendar,
							  QCBusinessCalendar valueCalendar,
							  std::shared_ptr<QCCurrency> ccy
							  );

			void setRateValue(double rateValue);
			QCDate getStartDate(QCDate& fixingDate);
			QCDate getEndDate(QCDate& fixingDate);
			Tenor getTenor();
			QCInterestRate getRate();
			unsigned int getDaysOfStartLag();
			unsigned int getMonthsOfTenor();
			~InterestRateIndex();

		private:
			QCInterestRate _rate;
			Tenor _startLag;
			Tenor _tenor;
			QCBusinessCalendar _fixingCalendar;
			QCBusinessCalendar _valueCalendar;
			std::shared_ptr<QCCurrency> _ccy;
		};
	}
}
#endif