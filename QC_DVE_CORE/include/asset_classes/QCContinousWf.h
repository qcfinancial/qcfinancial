#ifndef QCCONTINOUSWF_H
#define QCCONTINOUSWF_H

#include "QCWealthFactor.h"
class QCContinousWf :
	public QCWealthFactor
{
public:
	QCContinousWf();
	virtual double wf(double rate, double yf) override;
	virtual double rate(double wf, double yf) override;

	/**
	* @fn	virtual std::string QCWealthFactor::description();
	*
	* @brief	Gets the description. In this case "Exp".
	*
	* @author	Alvaro Díaz V.
	* @date	28/09/2017
	*
	* @return	A std::string.
	*/
	virtual std::string description();

	~QCContinousWf();
};

#endif

