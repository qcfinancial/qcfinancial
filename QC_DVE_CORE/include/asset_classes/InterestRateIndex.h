#ifndef INTERESTRATEINDEX_H
#define INTERESTRATEINDEX_H

#include<string>
#include<memory>

#include "Tenor.h"
#include "time/QCDate.h"
#include "asset_classes/QCCurrency.h"
#include "asset_classes/FinancialIndex.h"
#include "QCInterestRate.h"
#include "time/QCBusinessCalendar.h"

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

		//class LiborUSD3M : public InterestRateIndex
		//{
		//public:
		//	LiborUSD3M() : InterestRateIndex("LIBORUSD3M",
		//		QCInterestRate(0.0, std::make_shared<QCYearFraction>(QCAct360()),
		//		std::make_shared<QCWealthFactor>(QCLinearWf())), Tenor("2D"),

		//};
	}
}
#endif