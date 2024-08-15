#ifndef QCINTERESTRATE_H
#define QCINTERESTRATE_H

#include "asset_classes/QCYearFraction.h"
#include "asset_classes/QCWealthFactor.h"
#include "QCDefinitions.h"

/**
 * @class	QCInterestRate
 *
 * @brief	A class that models an interest rate; it's value, it's year fraction and it's
 * 			compounding method.
 *
 * @author	Alvaro Díaz V.
 * @date	27/09/2017
 */
class QCInterestRate {
public:

    /**
     * @fn	QCInterestRate::QCInterestRate(double value, QCYrFrctnShrdPtr yearFraction, QCWlthFctrShrdPtr wealthFactor);
     *
     * @brief	Constructor
     *
     * @author	Alvaro Díaz V.
     * @date	27/09/2017
     *
     * @param	value			The value.
     * @param	yearFraction	The year fraction.
     * @param	wealthFactor	The wealth factor.
     */
    QCInterestRate(
            double value,
            QCYrFrctnShrdPtr yearFraction,
            QCWlthFctrShrdPtr wealthFactor);

    /**
     * @fn	QCInterestRate::QCInterestRate(const QCInterestRate& otherRate)
     *
     * @brief	Copy constructor
     *
     * @author	Alvaro Díaz V.
     * @date	23/09/2018
     *
     * @param	otherRate	The other rate.
     */
    QCInterestRate(const QCInterestRate &otherRate) : QCInterestRate(otherRate._value, otherRate._yf, otherRate._wf) {
    }


    /**
     * @fn	double QCInterestRate::getValue();
     *
     * @brief	Gets the value of the interest rate
     *
     * @author	Alvaro Díaz V.
     * @date	27/09/2017
     *
     * @return	The value.
     */
    double getValue();

    /**
     * @fn	void QCInterestRate::setValue(double value);
     *
     * @brief	Sets a value for the rate.
     *
     * @author	Alvaro Díaz V.
     * @date	27/09/2017
     *
     * @param	value	The value.
     */
    void setValue(double value);

    /**
     * @fn	double QCInterestRate::wf(QCDate& startDate, QCDate& endDate);
     *
     * @brief	Calculates the value of the wf given a start date and an end date.
     *
     * @author	Alvaro Díaz V.
     * @date	27/09/2017
     *
     * @param [in,out]	startDate	The start date.
     * @param [in,out]	endDate  	The end date.
     *
     * @return	A double.
     */
    double wf(QCDate &startDate, QCDate &endDate);

    /**
     * @fn	double QCInterestRate::wf(long days);
     *
     * @brief	Calculates the value of the wf given a number of days.
     *
     * @author	Alvaro Díaz V.
     * @date	27/09/2017
     *
     * @param	days	The days.
     *
     * @return	A double.
     */
    double wf(long days);

    /**
    * @fn	double QCInterestRate::dwf(QCDate& startDate, QCDate& endDate);
    *
    * @brief	Returns the derivative wrt to rate of the wf calculated given
    * 			'startDate' and 'endDate'.
    *
    * @author	Alvaro Díaz V.
    * @date	27/09/2017
    *
    * @param [in,out]	startDate	The start date.
    * @param [in,out]	endDate  	The end date.
    *
    * @return	A double.
    */
    double dwf(QCDate &startDate, QCDate &endDate);

    /**
     * @fn	double QCInterestRate::dwf(long days);
     *
     * @brief	Returns the derivative wrt to rate of the wf calculated given 'days'.
     *
     * @author	Alvaro Díaz V.
     * @date	27/09/2017
     *
     * @param	days	The days.
     *
     * @return	A double.
     */
    double dwf(long days);

    /**
    * @fn		double QCInterestRate::dwf(long days);
    *
    * @brief	Returns 2nd the derivative wrt to rate of the wf calculated given 'days'.
    *
    * @author	Alvaro Díaz V.
    * @date		15/06/2018
    *
    * @param	days	The days.
    *
    * @return	A double.
    */
    double d2wf(long days);

    /**
    * @fn	double QCInterestRate::dwf(QCDate& startDate, QCDate& endDate);
    *
    * @brief	Returns the derivative wrt to rate of the wf calculated given
    * 			'startDate' and 'endDate'.
    *
    * @author	Alvaro Díaz V.
    * @date	15/06/2018
    *
    * @param [in,out]	startDate	The start date.
    * @param [in,out]	endDate  	The end date.
    *
    * @return	A double.
    */
    double d2wf(QCDate &startDate, QCDate &endDate);

    /**
     * @fn	double QCInterestRate::getRateFromWf(double wf, QCDate& startDate, QCDate& endDate);
     *
     * @brief	Gets rate from a wealth factor 'wf' and 'startDate' and 'endDate'.
     *
     * @author	Alvaro Díaz V.
     * @date	27/09/2017
     *
     * @param 		  	wf		 	The wf.
     * @param [in,out]	startDate	The start date.
     * @param [in,out]	endDate  	The end date.
     *
     * @return	The rate from wf.
     */
    double getRateFromWf(double wf, QCDate &startDate, QCDate &endDate);

    /**
     * @fn	double QCInterestRate::getRateFromWf(double wf, long days);
     *
     * @brief	Gets rate from wealth factor 'wf' given 'days'.
     *
     * @author	Alvaro Díaz V.
     * @date	27/09/2017
     *
     * @param	wf  	The wf.
     * @param	days	The days.
     *
     * @return	The rate from wf and days.
     */
    double getRateFromWf(double wf, long days);

    /**
     * @fn	double QCInterestRate::yf(QCDate& startDate, QCDate& endDate);
     *
     * @brief	Returns the year fraction given start date and end date.
     *
     * @author	Alvaro Díaz V.
     * @date	27/09/2017
     *
     * @param [in,out]	startDate	The start date.
     * @param [in,out]	endDate  	The end date.
     *
     * @return	A double.
     */

    double drate();

    double yf(QCDate &startDate, QCDate &endDate);

    /**
     * @fn	shared_ptr<QCWealthFactor> QCInterestRate::getWealthFactor();
     *
     * @brief	Gets wealth factor
     *
     * @author	Alvaro Díaz V.
     * @date	28/09/2017
     *
     * @return	The wealth factor.
     */
    shared_ptr<QCWealthFactor> getWealthFactor();

    /**
     * @fn	shared_ptr<QCYearFraction> QCInterestRate::getYearFraction();
     *
     * @brief	Gets year fraction
     *
     * @author	Alvaro Díaz V.
     * @date	28/09/2017
     *
     * @return	The year fraction.
     */
    shared_ptr<QCYearFraction> getYearFraction();

    /**
     * @fn	std::string QCInterestRate::description();
     *
     * @brief	Gets the description of the rate. Includes the value, yf and wf.
     *
     * @author	Alvaro Díaz V.
     * @date	20/09/2018
     *
     * @return	A std::string.
     */
    std::string description();

    /**
     * @fn	QCInterestRate::~QCInterestRate();
     *
     * @brief	Destructor
     *
     * @author	Alvaro Díaz V.
     * @date	27/09/2017
     */
    ~QCInterestRate();

private:

    /** @brief	The value of the rate*/
    double _value;

    /** @brief	The derivative of wf wrt rate */
    double _dwf;

    /** @brief	The year fraction */
    QCYrFrctnShrdPtr _yf;

    /** @brief	The wealth factor */
    QCWlthFctrShrdPtr _wf;

    /** @brief	The 2nd derivative of wf wrt rate */
    double _d2wf;

    double _drate;
};

#endif //QCINTERESTRATE_H
