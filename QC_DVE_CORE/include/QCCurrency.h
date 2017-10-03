//
// Created by Alvaro Diaz on 21-09-2017.
//


#ifndef QCCURRENCY_H
#define QCCURRENCY_H

#include <string>
#include <cmath>

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
	 * @fn	QCCurrency::QCCurrency()
	 *
	 * @brief	Default constructor. Returns USD.
	 *
	 * @author	Alvaro Díaz V.
	 * @date	02/10/2017
	 */
	QCCurrency() : _name("U. S. Dollar"), _isoCode("USD"), _isoNumber(840), _decimalPlaces(0)
	{
	}

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
	std::string getName() const
	{
		return _name;
	}

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
	std::string getIsoCode() const
	{
		return _isoCode;
	}

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
	unsigned int getIsoNumber() const
	{
		return _isoNumber;
	}

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
	unsigned int getDecimalPlaces() const
	{
		return _decimalPlaces;
	}

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
	double amount(double calcAmount)
	{
		double factor = pow(10, _decimalPlaces);
		return round(calcAmount * factor) / factor;
	}

	/**
	 * @fn	QCCurrency::~QCCurrency();
	 *
	 * @brief	Destructor
	 *
	 * @author	Alvaro Díaz V.
	 * @date	21/09/2017
	 */
	~QCCurrency()
	{
	}

private:

	/** @brief	The name */
	std::string _name;

	/** @brief	The ISO code */
	std::string _isoCode;

	/** @brief	The ISO number */
	unsigned int _isoNumber;

	/** @brief	The decimal places */
	unsigned int _decimalPlaces;

};

/**
 * @class	QCBRL
 *
 * @brief	Representation for BRL.
 *
 * @author	Alvaro Díaz V.
 * @date	27/09/2017
 */

class QCBRL : public QCCurrency
{
public:

	/**
	 * @fn	QCBRL::QCBRL()
	 *
	 * @brief	Default constructor
	 *
	 * @author	Alvaro Díaz V.
	 * @date	27/09/2017
	 */
	QCBRL() : QCCurrency("Brazilian Real", "BRL", 986, 2)
	{
	}
};

/**
 * @class	QCCLP
 *
 * @brief	Representation for CLP.
 *
 * @author	Alvaro Díaz V.
 * @date	27/09/2017
 */

class QCCLP : public QCCurrency
{
public:

	/**
	 * @fn	QCCLP::QCCLP()
	 *
	 * @brief	Default constructor
	 *
	 * @author	Alvaro Díaz V.
	 * @date	27/09/2017
	 */
	QCCLP() : QCCurrency("Chilean Peso", "CLP", 152, 0)
	{
	}
};

/**
 * @class	QCMXN
 *
 * @brief	Representation for MXN.
 *
 * @author	Alvaro Díaz V.
 * @date	27/09/2017
 */

class QCMXN : public QCCurrency
{
public:

	/**
	 * @fn	QCMXN::QCMXN()
	 *
	 * @brief	Default constructor
	 *
	 * @author	Alvaro Díaz V.
	 * @date	27/09/2017
	 */
	QCMXN() : QCCurrency("Mexican Peso", "MXN", 484, 2)
	{
	}
};

/**
 * @class	QCCAD
 *
 * @brief	Representation for CAD.
 *
 * @author	Alvaro Díaz V.
 * @date	27/09/2017
 */

class QCCAD : public QCCurrency
{
public:

	/**
	 * @fn	QCCAD::QCCAD()
	 *
	 * @brief	Default constructor
	 *
	 * @author	Alvaro Díaz V.
	 * @date	27/09/2017
	 */
	QCCAD() : QCCurrency("Canadian Dollar", "CAD", 124, 2)
	{
	}
};

/**
 * @class	QCUSD
 *
 * @brief	Representation for USD.
 *
 * @author	Alvaro Díaz V.
 * @date	27/09/2017
 */

class QCUSD : public QCCurrency
{
public:

	/**
	 * @fn	QCUSD::QCUSD()
	 *
	 * @brief	Default constructor
	 *
	 * @author	Alvaro Díaz V.
	 * @date	27/09/2017
	 */
	QCUSD() : QCCurrency("U. S. Dollar", "USD", 840, 2)
	{
	}
};

/**
 * @class	QCEUR
 *
 * @brief	Representation for EUR.
 *
 * @author	Alvaro Díaz V.
 * @date	27/09/2017
 */

class QCEUR : public QCCurrency
{
public:

	/**
	 * @fn	QCEUR::QCEUR()
	 *
	 * @brief	Default constructor
	 *
	 * @author	Alvaro Díaz V.
	 * @date	27/09/2017
	 */
	QCEUR() : QCCurrency("European Euro", "EUR", 978, 2)
	{
	}
};

/**
 * @class	QCGBP
 *
 * @brief	Representation for GBP.
 *
 * @author	Alvaro Díaz V.
 * @date	27/09/2017
 */

class QCGBP : public QCCurrency
{
public:

	/**
	 * @fn	QCGBP::QCGBP()
	 *
	 * @brief	Default constructor
	 *
	 * @author	Alvaro Díaz V.
	 * @date	27/09/2017
	 */
	QCGBP() : QCCurrency("British Pound Sterling", "GBP", 826, 2)
	{
	}
};

#endif //QCCURRENCY_H
