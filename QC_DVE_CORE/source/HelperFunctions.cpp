#include "HelperFunctions.h"

namespace QCode
{
	namespace Helpers
	{
		bool isDateInTimeSeries(const QCDate& date, const QCode::Financial::TimeSeries& timeSeries)
		{
			if (timeSeries.find(date) == timeSeries.end())
			{
				return false;
			}
			else
			{
				return true;
			}
		}
	}
}