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
		/**
		 * @class	FXRateIndex
		 *
		 * @brief	A class that represents an FX Rate Index. Consider for instance
		 * 			Dolar Observado in Chile or Petax in Brazil.
		 *
		 * @author	Alvaro Díaz V.
		 * @date	05/11/2018
		 */
		class FXRateIndex : public FinancialIndex
		{
		public:

			/**
			 * @fn	FXRateIndex::FXRateIndex(std::shared_ptr<FXRate> fxRate, std::string code, Tenor fixingRule, Tenor valueDateRule, QCBusinessCalendar calendar) : FinancialIndex(FinancialIndex::Fx, code), _fxRate(fxRate), _fixingRule(fixingRule), _valueDateRule(valueDateRule), _calendar(calendar)
			 *
			 * @brief	Constructor
			 *
			 * @author	Alvaro Díaz V.
			 * @date	05/11/2018
			 *
			 * @param	fxRate		 	The FX rate corresponding to the index.
			 * @param	code		 	The code of the FX Rate Index.
			 * @param	fixingRule   	The fixing rule.
			 * @param	valueDateRule	The value date rule.
			 * @param	calendar	 	The appropriate calendar for the index.
			 */
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

			/**
			 * @fn	QCDate FXRateIndex::fixingDate(QCDate& publishDate)
			 *
			 * @brief	Returns the financial fixing date of the index.
			 * 			For instance, in the caso of Petax in Brazil, it is 1 business
			 * 			day before publishing date.
			 *
			 * @author	Alvaro Díaz V.
			 * @date	05/11/2018
			 *
			 * @param [in,out]	publishDate	The publishing date.
			 *
			 * @return	A QCDate.
			 */
			QCDate fixingDate(QCDate& publishDate)
			{
				// Sólo se consideran los días de _fixingRule.
				auto dias = (int)_fixingRule.getDays();
				return _calendar.shift(publishDate, -dias);
			}

			/**
			 * @fn	QCDate FXRateIndex::valueDate(QCDate& publishDate)
			 *
			 * @brief	Returns the value date of the index. It considers the setllement lag
			 * 			of the underlying fx rate.
			 *
			 * @author	Alvaro Díaz V.
			 * @date	05/11/2018
			 *
			 * @param [in,out]	publishDate	The publishing date.
			 *
			 * @return	A QCDate.
			 */
			QCDate valueDate(QCDate& publishDate)
			{
				// Sólo se consideran los días de _valueDateRule.
				return _calendar.shift(fixingDate(publishDate), _valueDateRule.getDays());
			}

			/**
			 * @fn	std::string FXRateIndex::strongCcyCode()
			 *
			 * @brief	Returns the code for the strong currency of the underlying fx rate.
			 *
			 * @author	Alvaro Díaz V.
			 * @date	05/11/2018
			 *
			 * @return	A std::string.
			 */
			std::string strongCcyCode()
			{
				return _fxRate->strongCcyCode();
			}

			/**
			 * @fn	std::string FXRateIndex::weakCcyCode()
			 *
			 * @brief	Returns the code for the weak currency of the underlying fx rate.
			 *
			 * @author	Alvaro Díaz V.
			 * @date	05/11/2018
			 *
			 * @return	A std::string.
			 */
			std::string weakCcyCode()
			{
				return _fxRate->weakCcyCode();
			}

			/**
			 * @fn	double FXRateIndex::convert(double amount, QCCurrency currency, double fxRateIndexValue)
			 *
			 * @brief	Converts an amount in one of the currencies corresponding to the underlying
			 * 			fx rate to an amount in the other currency.
			 *
			 * @author	Alvaro Díaz V.
			 * @date	05/11/2018
			 *
			 * @exception	std::invalid_argument	Thrown when an invalid argument error condition occurs.
			 *
			 * @param	amount				The amount to convert.
			 * @param	currency			The currency of the amount.
			 * @param	fxRateIndexValue	The fx rate index value to be used.
			 *
			 * @return	A double.
			 */
			double convert(double amount, QCCurrency currency, double fxRateIndexValue)
			{
				if (currency.getIsoCode() == this->strongCcyCode())
				{
					return amount * fxRateIndexValue;
				}
				if (currency.getIsoCode() == this->weakCcyCode())
				{
					return amount / fxRateIndexValue;
				}
				else
				{
					std::string msg = "Amount in " + currency.getIsoCode();
					msg += " cannot be converted using FX Rate Index: " + this->getCode() + " .";
					throw std::invalid_argument(msg);
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