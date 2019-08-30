#ifndef TENOR_H
#define TENOR_H

#include<regex>
#include<string>

namespace QCode
{
	namespace Financial
	{
		/**
		 * @class	Tenor
		 *
		 * @brief	A class that represents a financial tenor (1D, 1M, 1Y ...).
		 *
		 * @author	Alvaro Díaz V.
		 * @date	14/09/2018
		 */
		class Tenor
		{
		public:

			/**
			 * @fn	Tenor::Tenor(std::string tenor);
			 *
			 * @brief	Constructor. The constructor validates that the provided string is valid. A valid string
			 * 			is a regular expression where the characters "d", "m" and "y" are preceded by a
			 * 			positive integer.
			 *
			 * @author	Alvaro Díaz V.
			 * @date	14/09/2018
			 *
			 * @param	tenor	The tenor.
			 */
			Tenor(std::string tenor);

			/**
			 * @fn	void Tenor::setTenor(std::string tenor);
			 *
			 * @brief	Sets a tenor
			 *
			 * @author	Alvaro Díaz V.
			 * @date	14/09/2018
			 *
			 * @param	tenor	The tenor.
			 */
			void setTenor(std::string tenor);

			/**
			 * @fn	unsigned int Tenor::getYears();
			 *
			 * @brief	Gets the number of years
			 *
			 * @author	Alvaro Díaz V.
			 * @date	14/09/2018
			 *
			 * @return	The years.
			 */
			unsigned int getYears();

			/**
			 * @fn	unsigned int Tenor::getMonths();
			 *
			 * @brief	Gets the number of months
			 *
			 * @author	Alvaro Díaz V.
			 * @date	14/09/2018
			 *
			 * @return	The months.
			 */
			unsigned int getMonths();

			/**
			 * @fn	unsigned int Tenor::getDays();
			 *
			 * @brief	Gets the number of days
			 *
			 * @author	Alvaro Díaz V.
			 * @date	14/09/2018
			 *
			 * @return	The days.
			 */
			unsigned int getDays();

			/**
			 * @fn	std::string Tenor::getString();
			 *
			 * @brief	Gets the minimal string needed to represent the tenor
			 *
			 * @author	Alvaro Díaz V.
			 * @date	14/09/2018
			 *
			 * @return	The string.
			 */
			std::string getString();

			/**
			 * @fn	Tenor::~Tenor();
			 *
			 * @brief	Destructor
			 *
			 * @author	Alvaro Díaz V.
			 * @date	14/09/2018
			 */
			~Tenor();

		private:

			/** @brief	The tenor */
			std::string _tenor;

			/**
			 * @fn	bool Tenor::_validate(std::string tenor);
			 *
			 * @brief	Validates the string given to represent the tenor
			 *
			 * @author	Alvaro Díaz V.
			 * @date	14/09/2018
			 *
			 * @param	tenor	The tenor.
			 *
			 * @return	True if it succeeds, false if it fails.
			 */
			bool _validate(std::string tenor);
		};
	}
}
#endif //TENOR_H