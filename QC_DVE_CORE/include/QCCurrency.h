//
// Created by Alvaro Diaz on 21-09-2017.
//


#ifndef QCCURRENCY_H
#define QCCURRENCY_H

#include <string>

/**
 * @class	QCCurrency
 *
 * @brief	Class that holds the basic data and methods related to a currency.
 * 			For convenience it should be subclassed to hold a particular currency.
 *
 * @author	Alvaro Díaz V.
 * @date	21/09/2017
 */

class QCCurrency
{
public:

	/**
	 * @fn	QCCurrency::QCCurrency(std::string name,
	 * 		std::string isoCode,
	 * 		unsigned int isoNumber,
	 * 		unsigned int decimalPlaces);
	 *
	 * @brief	Constructor
	 *
	 * @author	Alvaro Díaz V.
	 * @date	21/09/2017
	 *
	 * @param	name		 	The name of the currency, e. g. Chilean Peso.
	 * @param	isoCode		 	The ISO code of the currency e. g. CLP.
	 * @param	isoNumber	 	The ISO number of the currency e. g. 152.
	 * @param	decimalPlaces	The number of decimal places used in actual cashflows (CLP = 0).
	 */
	QCCurrency(std::string name,
			   std::string isoCode,
			   unsigned int isoNumber,
			   unsigned int decimalPlaces) :
			   _name(name),
			   _isoCode(isoCode),
			   _isoNumber(isoNumber),
			   _decimalPlaces(decimalPlaces)
	{
	}

	/**
	 * @fn	std::string QCCurrency::getName() const;
	 *
	 * @brief	Gets the name
	 *
	 * @author	Alvaro Díaz V.
	 * @date	21/09/2017
	 *
	 * @return	The name.
	 */
	std::string getName() const;

	/**
	 * @fn	std::string QCCurrency::getIsoCode() const;
	 *
	 * @brief	Gets ISO code
	 *
	 * @author	Alvaro Díaz V.
	 * @date	21/09/2017
	 *
	 * @return	The ISO code.
	 */
	std::string getIsoCode() const;

	/**
	 * @fn	unsigned int QCCurrency::getIsoNumber() const;
	 *
	 * @brief	Gets ISO number
	 *
	 * @author	Alvaro Díaz V.
	 * @date	21/09/2017
	 *
	 * @return	The ISO number.
	 */
	unsigned int getIsoNumber() const;

	/**
	 * @fn	unsigned int QCCurrency::getDecimalPlaces() const;
	 *
	 * @brief	Gets decimal places
	 *
	 * @author	Alvaro Díaz V.
	 * @date	21/09/2017
	 *
	 * @return	The decimal places.
	 */
	unsigned int getDecimalPlaces() const;

	/**
	 * @fn	double QCCurrency::amount(double amt);
	 *
	 * @brief	Returns the properly rounded amount of currency given the unrounded amount.
	 * 			Should be used for actual cashflows.
	 *
	 * @author	Alvaro Díaz V.
	 * @date	21/09/2017
	 *
	 * @param	amount	The amount of currency to be rounded.
	 *
	 * @return	A double with the rounded amount.
	 */
	double amt(double amount);

	/**
	 * @fn	QCCurrency::~QCCurrency();
	 *
	 * @brief	Destructor
	 *
	 * @author	Alvaro Díaz V.
	 * @date	21/09/2017
	 */
	~QCCurrency();

private:
	std::string _name;
	std::string _isoCode;
	unsigned int _isoNumber;
	unsigned int _decimalPlaces;

};

#endif //QCCURRENCY_H
