#ifndef QCCOMPOUNDWF_H
#define QCCOMPOUNDWF_H

#include "QCWealthFactor.h"
class QCCompoundWf :
	public QCWealthFactor
{
public:
	QCCompoundWf();
	virtual double wf(double rate, double yf) override;
	virtual double rate(double wf, double yf) override;

	/**
	* @fn	virtual std::string QCWealthFactor::description();
	*
	* @brief	Gets the description. In this case "Com"
	*
	* @author	Alvaro Díaz V.
	* @date	28/09/2017
	*
	* @return	A std::string.
	*/
	virtual std::string description() override;
	virtual ~QCCompoundWf();
};

#endif //QCCOMPOUNDWF_H

