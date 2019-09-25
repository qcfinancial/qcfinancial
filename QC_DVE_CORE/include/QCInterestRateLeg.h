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
		intRtPrdElmntInitialAccrtn, /*!< Disposición inicial del período */
		intRtPrdElmntAcctrnIsCshflw,/*!< Indica si la disposición inicial es flujo */
		intRtPrdElmntFinalAmrtztn,	/*!< Amortización final del periodo */
		inRtPrdElmntAmrtztnIsCshflw,/*!< Indica si la amortización final es flujo */
		intRtPrdElmntNotional,		/*!< Nocional vigente del período */
		intRtPrdElmntStartDate,		/*!< Fecha de inicio del período */
		intRtPrdElmntEndDate,		/*!< Fecha final del período */
		intRtPrdElmntSettlmntDate,	/*!< Fecha de pago del flujo */
		// Las siguientes 3 son para patas flotantes
		intRtPrdElmntFxngDate,		/*!< Fecha de fixing del índice */
		intRtPrdElmntFxngInitDate,	/*!< Fecha de inicio de devengo (fecha de valor) del índice */
		intRtPrdElmntFxngEndDate,	/*!< Fecha final de devengo del índice */
	};

	/*!
	* Enumera las distintas posibilidades de stub period
	*/
	enum QCStubPeriod
	{
		qcNoStubPeriod, /*!< No hay stub period */
		qcShortBack,	/*!< Período corto al final */
		qcLongBack,		/*!< Período largo al final */
		qcShortFront,	/*!< Período corto al inicio */
		qcLongFront,	/*!< Período largo al inicio */
		/*! Long Front puede pensarse como aplicar primero Short Front y luego fusionar los
		* primeros dos flujos. Esto da origen a las generalizaciones de tipo qcLongFrontx siguientes
		* donde x es el número de períodos que se agrupan al inicio luego de aplicar Short Front.
		* Estos stubs actuarán incluso en el caso de calendarios cuadrados agrupando los x
		* períodos iniciales.
		*/
		qcLongFront2,	/*!< Período largo al inicio agrupando 2*/
		qcLongFront3,	/*!< Período largo al inicio agrupando 3 */
		qcLongFront4,	/*!< Período largo al inicio agrupando 4 */
		qcLongFront5,	/*!< Período largo al inicio agrupando 5 */
		qcLongFront6,	/*!< Período largo al inicio agrupando 6 */
		qcLongFront7,	/*!< Período largo al inicio agrupando 7 */
		qcLongFront8,	/*!< Período largo al inicio agrupando 8 */
		qcLongFront9,	/*!< Período largo al inicio agrupando 9 */
		qcLongFront10,	/*!< Período largo al inicio agrupando 10 */
		qcLongFront11,	/*!< Período largo al inicio agrupando 11 */
		qcLongFront12,	/*!< Período largo al inicio agrupando 12 */
		qcLongFront120,	/*!< Período largo al inicio agrupando 120 */
		qcLongFront121	/*!< Período largo al inicio agrupando 121 */
	};

	/*!
	* Enumera las distintas posibilidades de amortizacion
	*/
	enum QCAmortization
	{
		qcBulletAmort,		/*!< Amortización bullet */
		qcConstantAmort,	/*!< Amortización constante en cada periodo */
		qcCustomAmort,		/*!< Amortización customizada */
		qcFrenchAmort,		/*!< Amortización estilo francés (cuota constante) */
	};

	/*!
	* Representa las componentes esenciales de un período de un
	* instrumento de tasa de interés. El orden es: disposición, esDispFlujo,
	* amortización, esAmortFlujo, nocional, fechaInicio, fechaFinal, fechaPago, fechaFixing,
	* fechaInicioIndice, fechaFinalIndice
	*/
	typedef tuple<double, bool, double, bool, double,
		QCDate, QCDate, QCDate, QCDate, QCDate, QCDate> QCInterestRatePeriod;

	/*!
	* Vector de QCInterestRatePeriod. Objeto base de un QCInterestRateLeg.
	*/
	typedef vector<QCInterestRatePeriod> QCInterestRatePeriods;

	/*! Constructor de la clase.
	* @param periods vector con los períodos
	* @param lastPeriod índice que identifica el último período
	*/
	QCInterestRateLeg(QCInterestRatePeriods periods,
		size_t lastPeriod);

	/*! Operador de igualdad */
	void operator=(const QCInterestRateLeg& otherLeg);
	
	/*! Getter para los períodos.
	* @return vector con los perídos.
	*/
	QCInterestRatePeriods periods() const;

	/*! Getter para el índice del último período.
	* @return índice del último período.
	*/
	size_t lastPeriod() const;
	
	/*! Getter para el tamaño del vector de períodos.
	* @return tamaño del vector de períodos
	*/
	size_t size();

	/*! Getter para un período específico.
	* @param n índice del período que se quiere obtener.
	* @return período con índice n.
	*/
	QCInterestRatePeriod getPeriodAt(size_t n);

	/*! Destructor */
	~QCInterestRateLeg();

protected:
	/*! Variable donde se almacenan los períodos. */
	QCInterestRatePeriods _periods;

	/*! Variable donde se almacena el valor del índice del último período. */
	size_t _lastPeriod;
};

#endif //QCINTERESTRATELEG_H

