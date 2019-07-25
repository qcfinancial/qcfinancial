#ifndef FXRATEINDEX_H
#define FXRATEINDEX_H

#include<memory>
#include<string>

#include "FinancialIndex.h"
#include "FXRate.h"
#include "Tenor.h"
#include "time/QCDate.h"
#include "time/QCBusinessCalendar.h"
#include "QCCurrency.h"

namespace QCode
{
	namespace Financial
	{


		class FXRateIndex : public FinancialIndex
		{
		public:
			static std::shared_ptr<FXRateIndex> getFxRateIndex(std::string indexCode, std::string fxRateCode)
			{

			}

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

			std::shared_ptr<FXRate> getFxRate() const
			{
				return _fxRate;
			}

			QCDate fixingDate(QCDate& publishDate)
			{
				// S�lo se consideran los d�as de _fixingRule.
				auto dias = (int)_fixingRule.getDays();
				return _calendar.shift(publishDate, -dias);
			}

			QCDate valueDate(QCDate& publishDate)
			{
				// S�lo se consideran los d�as de _valueDateRule.
				return _calendar.shift(fixingDate(publishDate), _valueDateRule.getDays());
			}

			std::string strongCcyCode() const
			{
				return _fxRate->strongCcyCode();
			}

			std::string weakCcyCode() const
			{
				return _fxRate->weakCcyCode();
			}

			QCBusinessCalendar& getCalendar()
			{
				return _calendar;
			}

			double convert(double notional, QCCurrency ccy, double value)
			{
				if (ccy.getIsoCode() == strongCcyCode())
				{
					return notional * value;
				}
				else if (ccy.getIsoCode() == weakCcyCode())
				{
					return notional / value;
				}
				else
				{
					std::string msg = "Currency " + ccy.getIsoCode() + " of notional is incompatible with currencies ";
					msg += strongCcyCode() + " and " + weakCcyCode() + " associated with FxRateIndex " + _code + ".";
					throw invalid_argument(msg);
				}
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