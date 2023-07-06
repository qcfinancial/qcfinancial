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
	QCCurrency() : _name("U. S. Dollar"), _isoCode("USD"), _isoNumber(840), _decimalPlaces(2)
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
	* @author	Alvaro D�az V.
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
	virtual ~QCCurrency()
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
 * @class	QCCLF
 *
 * @brief	Representation for CLF (ISO Code and Number are ficticious).
 *
 * @author	Alvaro D�az V.
 * @date	09/10/2017
 */
class QCCLF : public QCCurrency
{
public:

	/**
	 * @fn	QCCLF::QCCLF()
	 *
	 * @brief	Default constructor
	 *
	 * @author	Alvaro Díaz V.
	 * @date	09/10/2017
	 */
	QCCLF() : QCCurrency("Chilean Unidad de Fomento", "CLF", 666, 4)
	{
	}
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

class QCCHF : public QCCurrency
{
public:

    /**
     * @fn	QCCHF::QCCHF()
     *
     * @brief	Default constructor
     *
     * @author	Alvaro Díaz V.
     * @date	15/06/2020
     */
    QCCHF() : QCCurrency("Franco Suizo", "CHF", 756, 2)
    {
    }
};

/**
 * @class	QCMXN
 *
 * @brief	Representation for MXN.
 *
 * @author	Alvaro D�az V.
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
	 * @author	Alvaro D�az V.
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
	 * @author	Alvaro D�az V.
	 * @date	27/09/2017
	 */
	QCCAD() : QCCurrency("Canadian Dollar", "CAD", 124, 2)
	{
	}
};

/**
 * @class	QCCOP
 *
 * @brief	Representation for COP.
 *
 * @author	Alvaro Díaz V.
 * @date	27/07/2021
 */
class QCCOP : public QCCurrency
{
public:

    /**
     * @fn	QCCOP::QCCOP()
     *
     * @brief	Default constructor
     *
     * @author	Alvaro Díaz V.
     * @date	27/07/2021
     */
    QCCOP() : QCCurrency("Colombian Peso", "COP", 170, 2)
    {
    }
};

/**
 * @class	QCUSD
 *
 * @brief	Representation for USD.
 *
 * @author	Alvaro D�az V.
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
	 * @author	Alvaro D�az V.
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
 * @author	Alvaro D�az V.
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
	 * @author	Alvaro D�az V.
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
 * @author	Alvaro D�az V.
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
	 * @author	Alvaro D�az V.
	 * @date	27/09/2017
	 */
	QCGBP() : QCCurrency("British Pound Sterling", "GBP", 826, 2)
	{
	}
};

/**
* @fn	QCJPY::QCJPY()
*
* @brief	Default constructor
*
* @author	Alvaro D�az V.
* @date	07/01/2019
*/
class QCJPY : public QCCurrency
{
public:
	QCJPY() : QCCurrency("Japanese Yen", "JPY", 392, 2)
	{
	}
};

/**
* @fn	QCCNY::QCCNY()
*
* @brief	Default constructor
*
* @author	Alvaro Diaz V.
* @date	10/07/2022
*/
class QCCNY : public QCCurrency
{
public:
    QCCNY() : QCCurrency("Chinese CNY", "CNY", 156, 2)
    {
    }
};

class QCPEN : public QCCurrency
{
public:
    QCPEN() : QCCurrency("Sol peruano", "PEN", 604, 2)
    {
    }
};

class QCNOK : public QCCurrency
{
public:
    QCNOK() : QCCurrency("Corona noruega", "NOK", 578, 2)
    {
    }
};

class QCSEK : public QCCurrency
{
public:
    QCSEK() : QCCurrency("Corona sueca", "SEK", 752, 2)
    {
    }
};

class QCDKK : public QCCurrency
{
public:
    QCDKK() : QCCurrency("Corona danesa", "DKK", 208, 2)
    {
    }
};

class QCAUD : public QCCurrency
{
public:
    QCAUD() : QCCurrency("Dólar australiano", "AUD", 36, 2)
    {
    }
};

class QCHKD : public QCCurrency
{
public:
    QCHKD() : QCCurrency("Dólar de Hong Kong", "HKD", 344, 2)
    {
    }
};



#endif //QCCURRENCY_H
