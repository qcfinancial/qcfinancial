#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include "time/QCDate.h"
#include "TypeAliases.h"

namespace QCode
{
	namespace Helpers
	{
		/**
		* @fn	bool isDateInTimeSeries(const QCDate& date, const TimeSeries& timeSeries)
		*
		* @brief	Query if 'date' is date in time series
		*
		* @author	A Diaz V
		* @date	01-03-2019
		*
		* @param	date	  	The date.
		* @param	timeSeries	The time series.
		*
		* @returns	True if date in time series, false if not.
		*/
		bool isDateInTimeSeries(const QCDate& date, const QCode::Financial::TimeSeries& timeSeries);
	}
}

#endif //HELPERFUNCTIONS_H
