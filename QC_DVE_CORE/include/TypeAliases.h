#ifndef TYPEALIASES_H
#define TYPEALIASES_H

#include <map>
#include <string>

#include "QCDate.h"

namespace QCode
{
	namespace Financial
	{
		/**
		 * @typedef	std::map<QCDate, double> TimeSeries
		 *
		 * @brief	Defines an alias representing a time series. The key is a date represented by a QCDate object and the value is a double.
		 */
		typedef std::map<QCDate, double> TimeSeries;

		/**
		 * @typedef	std::map<std::string, std::map<QCDate, double>> ManyTimeSeries
		 *
		 * @brief	Defines an alias representing a container with many TimeSeries objects. The key for the outer std::map
		 * 			is a std::string equal to the code of the financial index to which the time series corresponds.
		 */
		typedef std::map<std::string, std::map<QCDate, double>> ManyTimeSeries;

		/**
		* @struct	FXVariation
		*
		* @brief	Provides a container for the FX variation (reajustes) associated to interest and nominal
		* 			for interest rate type cashflows.
		*
		* @author	A Diaz V
		* @date	28-02-2019
		*/
		struct FXVariation
		{
			FXVariation(double interest, double nominal) : interestVariation(interest), nominalVariation(nominal)
			{
			}

			double interestVariation;
			double nominalVariation;
		};


	}
}

#endif //TYPEALIASES_H