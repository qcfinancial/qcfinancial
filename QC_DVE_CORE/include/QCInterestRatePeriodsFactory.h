#ifndef QCINTERESTRATEPERIODSFACTORY_H
#define QCINTERESTRATEPERIODSFACTORY_H

#include <string>
#include <tuple>

#include "QCInterestRateLeg.h"
#include "QCDate.h"

using namespace std;
/*!
* @brief QCInterestRatePeriodsFactory construye estructuras de fechas para QCInterestRateLeg
* @author Alvaro Díaz (QCode)
*/
class QCInterestRatePeriodsFactory
{
public:
	/*!
	* Constructor de la clase
	* @param startDate fecha de inicio de la estructura
	* @param endDate fecha final de la estructura
	* @param settlementPeriodicity periodicidad de pago
	* @param settlementStubPeriod tipo de período irregular de pago
	* @param settlementCalendar calendario para fechas de cálculo de intereses y pago
	* @param settlementLag días de valuta para determinar la fecha de pago
	* @param fixingPeriodicity periodicidad de fixing
	* @param fixingStubPeriod tipo de período irregular en el fixing
	* @param fixingCalendar calendario para fechas de fixing
	* @param fixingLag días de anticipo respecto a fecha inicio de período de pago de la fecha de fixing
	* @param indexStartDateLag días de valuta del índice flotante desde fecha de fixing
	* @param indexTenor tenor del índice flotante (1M, 3M ...)
	*/
	QCInterestRatePeriodsFactory(QCDate startDate, QCDate endDate,
		QCDate::QCBusDayAdjRules endDateAdjustment,
		string settlementPeriodicity,
		QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
		shared_ptr<vector<QCDate>> settlementCalendar,
		unsigned int settlementLag,
		string fixingPeriodicity,
		QCInterestRateLeg::QCStubPeriod fixingStubPeriod,
		shared_ptr<vector<QCDate>> fixingCalendar,
		unsigned int fixingLag,
		unsigned int indexStartDateLag,
		string indexTenor
		);

	/*!
	* Método que construye la estructura de fechas que se quiere obtener.
	* @return estructura de fechas buscada
	*/
	QCInterestRateLeg::QCInterestRatePeriods getPeriods();

	/*! Destructor de la clase */
	~QCInterestRatePeriodsFactory();

private:
	/*!
	* Fecha de inicio
	*/
	QCDate _startDate;

	/*! Fecha final */
	QCDate _endDate;
	QCDate::QCBusDayAdjRules _endDateAdjustment;

	/*! Periodicidad de pago */
	string _settlementPeriodicity;

	/*! Tipo de período irregular de pago */
	QCInterestRateLeg::QCStubPeriod _settlementStubPeriod;

	/*! Calendario para fechas de cálculo de intereses y pago */
	shared_ptr<vector<QCDate>> _settlementCalendar;

	/*! días de valuta para determinar la fecha de pago */
	unsigned int _settlementLag;

	/*! Periodicidad de fixing */
	string _fixingPeriodicity;

	/*! Tipo de período irregular en el fixing */
	QCInterestRateLeg::QCStubPeriod _fixingStubPeriod;

	/*! Calendario para fechas de fixing */
	shared_ptr<vector<QCDate>> _fixingCalendar;

	/*! Días de anticipo respecto a fecha inicio de período de pago de la fecha de fixing */
	unsigned int _fixingLag;

	/*! Días de valuta del índice flotante desde fecha de fixing */
	unsigned int _indexStartDateLag;

	/*! Tenor del índice flotante (1M, 3M ...) */
	string _indexTenor;
	
	/*! Aquí se guarda una estructura básica de fechas de inicio y fin para cálculo de intereses.
	* Se utiliza en getPeriods() como paso intermedio
	*/
	vector<tuple<QCDate, QCDate>> _settlementBasicDates;

	/*! Aquí se guarda una estructura básica de fechas de inicio y fin para fixing.
	* Se utiliza en getPeriods() como paso intermedio
	*/
	vector<tuple<QCDate, QCDate>> _fixingBasicDates;

	/*! DEPRECATED. Método que calcula estructuras básicas de fechas de inicio y fin.
	* @param periodicity valor de periodicidad (3M, 6M ...)
	* @param stubPeriod tipo de período irregular
	* @param calendar calendario a utilizar
	*/
	vector<tuple<QCDate, QCDate>> _buildBasicDates(string periodicity,
		QCInterestRateLeg::QCStubPeriod stubPeriod, shared_ptr<vector<QCDate>> calendar);

	/*! Método que calcula estructuras básicas de fechas de inicio y fin.
	* @param periodicity valor de periodicidad (3M, 6M ...)
	* @param stubPeriod tipo de período irregular
	* @param calendar calendario a utilizar
	*/
	vector<tuple<QCDate, QCDate>> QCInterestRatePeriodsFactory::_buildBasicDates2(string periodicity,
		QCInterestRateLeg::QCStubPeriod stubPeriod, shared_ptr<vector<QCDate>> calendar);
	
	/*! DEPRECATED. Método que construye la estructura de fechas que se quiere obtener.
	* @return estructura de fechas buscada
	*/
	QCInterestRateLeg::QCInterestRatePeriods _getPeriods1();

	/*! Método que construye la estructura de fechas que se quiere obtener.
	* @return estructura de fechas buscada
	*/
	QCInterestRateLeg::QCInterestRatePeriods _getPeriods2();

	/*! EN DESARROLLO. Método que construye la estructura de fechas que se quiere obtener.
	* @return estructura de fechas buscada
	*/
	QCInterestRateLeg::QCInterestRatePeriods _getPeriods3();

	/*! Permite asignar la fecha de fixing a cada período. Se utiliza en _getPeriods3() */
	vector<bool> _fixingFlags;	
	
	/*! Construye el vector _fixingFlags.
	* @param numPeriods es el número de períodos de pago de la pata
	*/
	void _setFixingFlags(size_t numPeriods);

	/*! Función auxiliar para operaciones LONGFRONTN con easy case (período cuadrado).
	 * Se utiliza en _buildBasicDates2.
	* @param displacement = N - 1 (se ingresa porque no se puede deducir del enum QCStubPeriod)
	* @param numWholePeriods. Se calcula en el cuerpo de _buildBasicDates2
	* @param periods. Se define al inicio de _buildBasicDates2. Se entrega por referencia y se
	* rellena dentro de la función.
	* @param periodicity. Periodicidad, viene de la llamada a _buildBasicDates2.
	* @param calendar. Calendario relevante, viene de la llamada a _buildBasicDates2.
	*/
	void _auxWithLongFrontNEasyCase(unsigned int displacement,
		unsigned int numWholePeriods, vector<tuple<QCDate, QCDate>>& periods,
		string& periodicity, shared_ptr<vector<QCDate>>& calendar);

	/*! Función auxiliar para operaciones LONGFRONTN con not easy case (período no cuadrado).
	* Se utiliza en _buildBasicDates2.
	* @param displacement = N - 1 (se ingresa porque no se puede deducir del enum QCStubPeriod)
	* @param numWholePeriods. Se calcula en el cuerpo de _buildBasicDates2
	* @param remainderInMonths. Se calcula en el cuerpo de _buildBasicDates2
	* @param remainderInDays. Se calcula en el cuerpo de _buildBasicDates2
	* @param periods. Se define al inicio de _buildBasicDates2. Se entrega por referencia y se
	* rellena dentro de la función.
	* @param periodicity. Periodicidad, viene de la llamada a _buildBasicDates2.
	* @param calendar. Calendario relevante, viene de la llamada a _buildBasicDates2.
	*/
	void _auxWithLongFrontNNotEasyCase(unsigned int displacement,
		unsigned int numWholePeriods, unsigned int remainderInMonths, unsigned int remainderInDays,
		vector<tuple<QCDate, QCDate>>& periods, string& periodicity, shared_ptr<vector<QCDate>>& calendar);

};

#endif //QCINTERESTRATEPERIODSFACTORY_H