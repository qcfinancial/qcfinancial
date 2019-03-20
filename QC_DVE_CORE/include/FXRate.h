#ifndef FXRATE_H
#define FXRATE_H

#include<memory>
#include<string>

#include "QCCurrency.h"

namespace QCode
{
	namespace Financial
	{
		/**
		* @class	FXRate
		*
		* @brief	Class that models the concept of an FX rate.
		*
		* @author	Alvaro Díaz V.
		* @date	    15/11/2017
		*/
		class FXRate
		{
		public:
			/**
			* @fn	    FXRate(std::shared_ptr<QCCurrency> strongCcy, std::shared_ptr<QCCurrency> weakCcy);
			*
			* @brief	Constructor
			*
			* @author	Alvaro Díaz V.
			* @date	    15/11/2017
			*
			* @param	strongCcy		The strong currency of the pair. For EURUSD it should be EUR.
			* @param	weakCcy		 	The weak currency of the pair. For EURUSD it should be USD.
			*/
			FXRate(std::shared_ptr<QCCurrency> strongCcy,
				   std::shared_ptr<QCCurrency> weakCcy) :
				   _strongCcy(strongCcy),
				   _weakCcy(weakCcy)
			{
			}

			/**
			* @fn	std::string FXRate::getCode() const;
			*
			* @brief	Gets the standard code for the pair. Strong currency first, weak currency second.
			*           For example, if strongCcy is EUR and weakCcy is USD the code will be EURUSD.
			*
			* @author	Alvaro Díaz V.
			* @date	    15/11/2017
			*
			* @return	The standard code.
			*/
			std::string getCode()
			{
				return _strongCcy->getIsoCode() + _weakCcy->getIsoCode();
			}

			std::shared_ptr<QCCurrency> getStrongCcy() const
			{
				return _strongCcy;
			}

			std::string strongCcyCode()
			{
				return _strongCcy->getIsoCode();
			}
			
			std::shared_ptr<QCCurrency> getWeakCcy() const
			{
				return _weakCcy;
			}

			std::string weakCcyCode()
			{
				return _weakCcy->getIsoCode();
			}

		private:
			/** @brief	The strong currency */
			std::shared_ptr<QCCurrency> _strongCcy;

			/** @brief	The weak currency */
			std::shared_ptr<QCCurrency> _weakCcy;
		};
	}
}
#endif //FXRATE_H