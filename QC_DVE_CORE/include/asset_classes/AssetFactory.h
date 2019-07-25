#ifndef ASSETFACTORY_H
#define ASSETFACTORY_H

#include<memory>

#include "asset_classes/QCCurrency.h"
#include "asset_classes/FXRate.h"

namespace QCode
{
	namespace Financial
	{ 
		/**
		* @fn	std::shared_ptr<QCCurrency> getQCCurrencyFromCode(std::string code)
		*
		* @brief	Gets QCCurrency object from the currency's iso code
		*
		* @author	A Diaz V
		* @date	26-04-2019
		*
		* @exception	std::invalid_argument	Thrown when an invalid argument error condition occurs.
		*
		* @param	code	The code.
		*
		* @returns	The QCCurrency from code.
		*/
		std::shared_ptr<QCCurrency> getQCCurrencyFromCode(std::string code)
		{
			if (code == "BRL")
			{
				return std::make_shared<QCBRL>(QCBRL());
			}
			if (code == "CAD")
			{
				return std::make_shared<QCCAD>(QCCAD());
			}
			if (code == "CLF")
			{
				return std::make_shared<QCCLF>(QCCLF());
			}
			if (code == "CLP")
			{
				return std::make_shared<QCCLP>(QCCLP());
			}
			if (code == "EUR")
			{
				return std::make_shared<QCEUR>(QCEUR());
			}
			if (code == "GBP")
			{
				return std::make_shared<QCGBP>(QCGBP());
			}
			if (code == "JPY")
			{
				return std::make_shared<QCJPY>(QCJPY());
			}
			if (code == "MXN")
			{
				return std::make_shared<QCMXN>(QCMXN());
			}
			if (code == "USD")
			{
				return std::make_shared<QCUSD>(QCUSD());
			}
			throw std::invalid_argument("No converter found for currency code " + code + ".");
		}

		/**
		 * @fn	std::shared_ptr<FXRate> getFXRateFromCode(std::string code)
		 *
		 * @brief	Gets FX rate from code
		 *
		 * @author	A Diaz V
		 * @date	26-04-2019
		 *
		 * @param	code	The code.
		 *
		 * @returns	The FX rate from code.
		 */
		std::shared_ptr<FXRate> getFXRateFromCode(std::string code)
		{
			return std::make_shared<FXRate>(FXRate{ getQCCurrencyFromCode(code.substr(0, 3)), getQCCurrencyFromCode(code.substr(3, 6)) });
		}


	}
}


#endif // ASSETFACTORY_H 