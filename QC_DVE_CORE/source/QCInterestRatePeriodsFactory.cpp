#include "QCInterestRatePeriodsFactory.h"
#include "QCHelperFunctions.h"

QCInterestRatePeriodsFactory::QCInterestRatePeriodsFactory(QCDate startDate, QCDate endDate,
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
	) : _startDate(startDate), _endDate(endDate), _endDateAdjustment(endDateAdjustment),
	_settlementPeriodicity(settlementPeriodicity), _settlementStubPeriod(settlementStubPeriod),
	_settlementCalendar(settlementCalendar), _settlementLag(settlementLag),
	_fixingPeriodicity(fixingPeriodicity), _fixingStubPeriod(fixingStubPeriod),
	_fixingCalendar(fixingCalendar), _fixingLag(fixingLag),
	_indexStartDateLag(indexStartDateLag), _indexTenor(indexTenor)
{}

QCInterestRateLeg::QCInterestRatePeriods QCInterestRatePeriodsFactory::getPeriods()
{
	QCInterestRateLeg::QCInterestRatePeriods result;
	
	//Saca los basicperiods de settlement
	_settlementBasicDates = _buildBasicDates(_settlementPeriodicity, _settlementStubPeriod, _settlementCalendar);

	//Aprovechamos de darle size a result
	result.resize(_settlementBasicDates.size());

	//Saca los basicperiods de fixing
	//Esto es importante de notar, aqui se usa el calendario de settlement.
	//Luego al calcular la fecha efectiva de fixing con el fixing lag se usara el calendario
	//de fixing. Hay que implementar PREV en businessDate de QCDate.
	_fixingBasicDates = _buildBasicDates(_fixingPeriodicity, _fixingStubPeriod, _settlementCalendar);
	
	//Loop sobre los basicperiods de settlement
	QCDate fixingDate;
	for (unsigned int i = 0; i < _settlementBasicDates.size(); ++i)
	{
		//Calcula settlement date
		QCDate settlementDate = get<1>(_settlementBasicDates.at(i)).shift(_settlementCalendar, _settlementLag,
			QCDate::qcFollow);
		
		//Asigna el fixing date. El primero es el primero de los basic dates de fixing
		for (unsigned int j = _fixingBasicDates.size(); j-- > 0;)
		{
			if (get<0>(_fixingBasicDates.at(j)) <= get<0>(_settlementBasicDates.at(i)))
			{
				fixingDate = get<0>(_fixingBasicDates.at(j));
				break;
			}
			
		}

		//Calcula la fecha de fixing con el fixing lag
		fixingDate = fixingDate.shift(_fixingCalendar, _fixingLag, QCDate::qcPrev);

		//Calcula la fecha de inicio del indice con start date rule
		QCDate indexStartDate = fixingDate.shift(_fixingCalendar, _indexStartDateLag, QCDate::qcFollow);

		//Calcula la fecha final del indice con el tenor del indice
		QCDate indexEndDate = indexStartDate.addMonths(QCHelperFunctions::tenor(_indexTenor)).
			businessDay(_fixingCalendar, QCDate::qcFollow);

		//Se arma la tupla period y se inserta en result
		result.at(i) = make_tuple(0, false, 0, true, 0, get<0>(_settlementBasicDates.at(i)),
			get<1>(_settlementBasicDates.at(i)), settlementDate,
			fixingDate, indexStartDate, indexEndDate);
	}

	return result;
}

vector<tuple<QCDate, QCDate>> QCInterestRatePeriodsFactory::_buildBasicDates(string periodicity,
	QCInterestRateLeg::QCStubPeriod stubPeriod, shared_ptr<vector<QCDate>> calendar)
{
	//Aqui se almancena el resultado
	vector<tuple<QCDate, QCDate>> periods; 

	//La distancia en meses y dias entre startDate y endDate
	tuple<unsigned long, int> distanceMonthsDays = _startDate.monthDiffDayRemainder(
		_endDate, calendar, _endDateAdjustment);

	//Cuantos periodos enteros hay entre startDate y endDate y cuantos meses sobran
	unsigned int numWholePeriods = get<0>(distanceMonthsDays) / QCHelperFunctions::tenor(periodicity);
	unsigned int remainderInMonths = get<0>(distanceMonthsDays) % QCHelperFunctions::tenor(periodicity);
	if (stubPeriod == QCInterestRateLeg::qcShortBack || stubPeriod == QCInterestRateLeg::qcNoStubPeriod)
	{
		int indicador = 0;
		if (remainderInMonths + get<1>(distanceMonthsDays) == 0)
			indicador = 1;
		periods.resize(numWholePeriods + 1 - indicador);
		unsigned int numPeriods = periods.size();

		QCDate fechaInicioPeriodo = _startDate;
		QCDate fechaFinalPeriodo;

		if (numPeriods == 1)
		{
			fechaFinalPeriodo = _endDate;
		}
		else
		{
			fechaFinalPeriodo = _startDate.addMonths(QCHelperFunctions::tenor(periodicity)).businessDay(
				calendar, _endDateAdjustment);
		}

		periods.at(0) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);

		if (numPeriods >= 2)
		{
			if (numPeriods > 2)
			{
				for (unsigned int i = 1; i < periods.size() - 1; ++i)
				{
					fechaInicioPeriodo = fechaFinalPeriodo;
					fechaFinalPeriodo = _startDate.addMonths(QCHelperFunctions::tenor(periodicity) * (i + 1)).businessDay(
						calendar, _endDateAdjustment);
					periods.at(i) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
				}
			}

			fechaInicioPeriodo = fechaFinalPeriodo;
			fechaFinalPeriodo = _endDate;
			periods.at(periods.size() - 1) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
		}
	}
	if (stubPeriod == QCInterestRateLeg::qcLongBack)
	{
		int indicador = 0;
		if (remainderInMonths + get<1>(distanceMonthsDays) == 0)
			indicador = 1;
		periods.resize(numWholePeriods + 1 - indicador);
		unsigned int numPeriods = periods.size();

		QCDate fechaInicioPeriodo = _startDate;
		QCDate fechaFinalPeriodo;
		if (numPeriods == 1)
		{
			fechaFinalPeriodo = _endDate;
		}
		else
		{
			fechaFinalPeriodo = _startDate.addMonths(QCHelperFunctions::tenor(periodicity)).businessDay(
				calendar, _endDateAdjustment);
		}

		periods.at(0) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);

		if (numPeriods >= 2)
		{
			if (numPeriods > 2)
			{
				for (unsigned int i = 1; i < periods.size() - 1; ++i)
				{
					fechaInicioPeriodo = fechaFinalPeriodo;
					fechaFinalPeriodo = _startDate.addMonths(QCHelperFunctions::tenor(periodicity) * (i + 1)).businessDay(
						calendar, _endDateAdjustment);
					periods.at(i) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
				}
			}

			fechaInicioPeriodo = fechaFinalPeriodo;
			fechaFinalPeriodo = _endDate;
			periods.at(periods.size() - 1) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
		}
	}
	if (stubPeriod == QCInterestRateLeg::qcShortFront)
	{
		int indicador = 0;
		if (remainderInMonths + get<1>(distanceMonthsDays) == 0)
			indicador = 1;
		periods.resize(numWholePeriods + 1 - indicador);
		unsigned int numPeriods = periods.size();

		QCDate fechaFinalPeriodo = _endDate;
		QCDate fechaInicioPeriodo;
		if (numPeriods == 1)
		{
			fechaInicioPeriodo = _startDate;
		}
		else
		{
			fechaInicioPeriodo = _endDate.addMonths(-QCHelperFunctions::tenor(periodicity)).businessDay(
				calendar, _endDateAdjustment);
		}
		QCDate fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, _settlementLag, QCDate::qcFollow);

		periods.at(numPeriods - 1) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);

		if (numPeriods >= 2)
		{
			if (numPeriods > 2)
			{
				for (unsigned int i = numPeriods - 2; i > 0; --i)
				{
					fechaFinalPeriodo = fechaInicioPeriodo;
					fechaInicioPeriodo = _endDate.addMonths((i - numPeriods) *
						QCHelperFunctions::tenor(periodicity)).businessDay(
						calendar, _endDateAdjustment);
					fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, _settlementLag, QCDate::qcFollow);
					periods.at(i) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
				}
			}

			fechaFinalPeriodo = fechaInicioPeriodo;
			fechaInicioPeriodo = _startDate;
			fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, _settlementLag, QCDate::qcFollow);
			periods.at(0) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
		}
	}
	if (stubPeriod == QCInterestRateLeg::qcLongFront)
	{
		int indicador = 0;
		if (remainderInMonths + get<1>(distanceMonthsDays) == 0)
			indicador = 1;
		periods.resize(numWholePeriods + 1 - indicador);
		unsigned int numPeriods = periods.size();

		QCDate fechaFinalPeriodo = _endDate;
		QCDate fechaInicioPeriodo;
		if (numPeriods == 1)
		{
			fechaInicioPeriodo = _startDate;
		}
		else
		{
			fechaInicioPeriodo = _endDate.addMonths(-QCHelperFunctions::tenor(periodicity)).businessDay(
				calendar, _endDateAdjustment);
		}

		QCDate fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, _settlementLag, QCDate::qcFollow);
		periods.at(numPeriods - 1) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);

		if (numPeriods >= 2)
		{
			if (numPeriods > 2)
			{
				for (unsigned int i = numPeriods - 2; i > 0; --i)
				{
					fechaFinalPeriodo = fechaInicioPeriodo;
					fechaInicioPeriodo = _endDate.addMonths((i - numPeriods) *
						QCHelperFunctions::tenor(periodicity)).businessDay(
						calendar, _endDateAdjustment);
					fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, _settlementLag, QCDate::qcFollow);
					periods.at(i) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
				}
			}

			fechaFinalPeriodo = fechaInicioPeriodo;
			fechaInicioPeriodo = _startDate;

			periods.at(0) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
		}
	}

	return periods;
}


QCInterestRatePeriodsFactory::~QCInterestRatePeriodsFactory()
{}

