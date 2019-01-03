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
				// Sólo se consideran los días de _fixingRule.
				auto dias = (int)_fixingRule.getDays();
				return _calendar.shift(publishDate, -dias);
			}

			QCDate valueDate(QCDate& publishDate)
			{
				// Sólo se consideran los días de _valueDateRule.
				return _calendar.shift(fixingDate(publishDate), _valueDateRule.getDays());
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