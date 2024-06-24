#ifndef QCLINEARWF_H
#define QCLINEARWF_H

#include "QCWealthFactor.h"

class QCLinearWf :
	public QCWealthFactor
{
public:
	QCLinearWf();
	double wf(double rate, double yf) override;
	double rate(double wf, double yf) override;
	
	/**
	* @fn	virtual std::string QCWealthFactor::description();
	*
	* @brief	Gets the description. In this case "Lin".
	*
	* @author	Alvaro Díaz V.
	* @date	28/09/2017
	*
	* @return	A std::string.
	*/
	std::string description() override;
	~QCLinearWf();
};

#endif //QCLINEARWF_H

