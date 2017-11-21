#ifndef FXRATEINDEX_H
#define FXRATEINDEX_H

#include<memory>
#include<string>

#include "FinancialIndex.h"
#include "FXRate.h"
#include "Tenor.h"
#include "QCDate.h"
#include "QCBusinessCalendar.h"

namespace QCode
{
	namespace Financial
	{
		class FXRateIndex : public FinancialIndex
		{
		public:
			FXRateIndex(std::shared_ptr<FXRate> fxRate,
						std::string code,
						Tenor fixingRule,
						Tenor valueDateRule,
						QCBusinessCalendar calendar) :
						FinancialIndex(FinancialIndex::Fx, code),
						_fxRate(fxRate),
						_fixingRule(fixingRule),
						_valueDateRule(valueDateRule),
						_calendar(calendar)
			{
			}

			QCDate fixingDate(QCDate& publishDate)
			{
				return publishDate;
			}

			QCDate valueDate(QCDate& publishDate)
			{
				return publishDate;
			}

			std::string strongCcyCode()
			{
				return _fxRate->strongCcyCode();
			}

			std::string weakCcyCode()
			{
				return _fxRate->weakCcyCode();
			}

		private:
			std::shared_ptr<FXRate> _fxRate;
			Tenor _fixingRule;
			Tenor _valueDateRule;
			QCBusinessCalendar _calendar;
		};
	}
}
#endif //FXRATEINDEX_H