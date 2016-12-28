#ifndef QCINTERESTRATELEG_H
#define QCINTERESTRATELEG_H

#include <memory>
#include <tuple>

#include "QCDate.h"

using namespace std;

class QCInterestRateLeg
{
public:
	/*!
	* Enumera las componentes de un QCInterestRatePeriod
	*/
	enum InterestRatePeriodElement
	{
		intRtPrdElmntInitialAccrtn, /*!< Disposicion inicial del periodo */
		intRtPrdElmntAcctrnIsCshflw,/*!< Indica si la disposicion inicial es flujo */
		intRtPrdElmntFinalAmrtztn,	/*!< Amortizacion final del periodo */
		inRtPrdElmntAmrtztnIsCshflw,/*!< Indica si la amortizacion final es flujo */
		intRtPrdElmntNotional,		/*!< Nocional vigente del periodo */
		intRtPrdElmntStartDate,		/*!< Fecha de inicio del periodo */
		intRtPrdElmntEndDate,		/*!< Fecha final del periodo */
		intRtPrdElmntSettlmntDate,	/*!< Fecha de pago del flujo */
		//Las siguientes 3 son para patas flotantes
		intRtPrdElmntFxngDate,		/*!< Fecha de fixing del indice */
		intRtPrdElmntFxngInitDate,	/*!< Fecha de inicio de devengo del indice */
		intRtPrdElmntFxngEndDate	/*!< Fecha final de devengo del indice */
		//Las siguientes 3 son para patas multicurrency
	};

	/*!
	* Representa las componentes esenciales de un periodo de un
	* instrumento de tasa de interes. El orden es: disposicion, esDispFlujo,
	* amortizacion, esAmortFlujo, nocional, fechaInicio, fechaFinal, fechaPago, fechaFixing,
	* fechaInicioIndiceFixing, fechaFinalIndiceFixing
	*/
	typedef tuple<double, bool, double, bool, double,
		QCDate, QCDate, QCDate, QCDate, QCDate, QCDate> QCInterestRatePeriod;

	/*!
	* Vector de QCInterestRatePeriod. Objeto base de un QCInterestRateLeg.
	*/
	typedef vector<QCInterestRatePeriod> QCInterestRatePeriods;

	QCInterestRateLeg(QCInterestRatePeriods periods,
		unsigned int lastPeriod);
	void operator=(const QCInterestRateLeg& otherLeg);
	
	QCInterestRatePeriods periods() const;
	unsigned int lastPeriod() const;
	
	int size();
	QCInterestRatePeriod getPeriodAt(unsigned int n);
	~QCInterestRateLeg();

protected:
	QCInterestRatePeriods _periods;
	unsigned int _lastPeriod;
};

#endif //QCINTERESTRATELEG_H

