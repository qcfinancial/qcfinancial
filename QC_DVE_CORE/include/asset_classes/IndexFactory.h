#ifndef INDEXFACTORY_H
#define INDEXFACTORY_H

#include<memory>

#include "AssetFactory.h"
#include "asset_classes/FXRateIndex.h"
#include "Tenor.h"
#include "time/QCDate.h"
#include "time/QCBusinessCalendar.h"

namespace QCode
{
	namespace Financial
	{
		/**
		 * @fn	std::shared_ptr<FXRateindex> getFXRateIndexFromFXRateCode(std::string indexCode, std::string fxRateCode)
		 *
		 * @brief	Gets FX rate index from FX rate code. Note that this function does not consider fixing rule, value date rule and calendar.
		 * 			If these are needed use the corresponding overload of this function.
		 *
		 * @author	A Diaz V
		 * @date	26-04-2019
		 *
		 * @param	indexCode 	The index code.
		 * @param	fxRateCode	The FX rate code.
		 *
		 * @returns	The FX rate index from FX rate code. 
		 */

		std::shared_ptr<FXRateIndex> getFXRateIndexFromFXRateCode(std::string indexCode, std::string fxRateCode)
		{
			auto fxRate = getFXRateFromCode(fxRateCode);
			auto tenor = Tenor{ "1D" };
			auto calendar = QCBusinessCalendar{ QCDate{ 12, 1, 1969 }, 1 };
			return std::make_shared<FXRateIndex>(FXRateIndex{ fxRate, indexCode, tenor, tenor, calendar });
		}
	}
}
#endif // INDEXFACTORY_H