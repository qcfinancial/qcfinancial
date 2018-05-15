#include "QCInterestRatePeriodsFactory.h"
#include "QCHelperFunctions.h"

QCInterestRatePeriodsFactory::QCInterestRatePeriodsFactory(
	QCDate startDate,
	QCDate endDate,
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
	) :
	_startDate(startDate),
	_endDate(endDate),
	_endDateAdjustment(endDateAdjustment),
	_settlementPeriodicity(settlementPeriodicity),
	_settlementStubPeriod(settlementStubPeriod),
	_settlementCalendar(settlementCalendar),
	_settlementLag(settlementLag),
	_fixingPeriodicity(fixingPeriodicity),
	_fixingStubPeriod(fixingStubPeriod),
	_fixingCalendar(fixingCalendar),
	_fixingLag(fixingLag),
	_indexStartDateLag(indexStartDateLag),
	_indexTenor(indexTenor)
{}

QCInterestRateLeg::QCInterestRatePeriods QCInterestRatePeriodsFactory::getPeriods()
{
	return _getPeriods2();
}

QCInterestRateLeg::QCInterestRatePeriods QCInterestRatePeriodsFactory::_getPeriods1()
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

QCInterestRateLeg::QCInterestRatePeriods QCInterestRatePeriodsFactory::_getPeriods2()
{
	QCInterestRateLeg::QCInterestRatePeriods result;

	//Saca los basicperiods de settlement
	_settlementBasicDates = _buildBasicDates2(_settlementPeriodicity, _settlementStubPeriod, _settlementCalendar);

	//Aprovechamos de darle size a result
	result.resize(_settlementBasicDates.size());

	//Saca los basicperiods de fixing
	//Esto es importante de notar, aqui se usa el calendario de settlement.
	//Luego al calcular la fecha efectiva de fixing con el fixing lag se usara el calendario
	//de fixing.
	_fixingBasicDates = _buildBasicDates2(_fixingPeriodicity, _fixingStubPeriod, _settlementCalendar);

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

//Con _getPeriods3() se va a cambiar la lógica de construcción de las fechas de fixing
QCInterestRateLeg::QCInterestRatePeriods QCInterestRatePeriodsFactory::_getPeriods3()
{
	QCInterestRateLeg::QCInterestRatePeriods result;

	//Saca los basicperiods de settlement
	_settlementBasicDates = _buildBasicDates2(_settlementPeriodicity, _settlementStubPeriod, _settlementCalendar);

	//Aprovechamos de darle size a result
	unsigned int numPeriods = _settlementBasicDates.size();
	result.resize(numPeriods);

	_setFixingFlags(numPeriods);

	//Loop sobre los basicperiods de settlement
	QCDate fixingDate;
	for (unsigned int i = 0; i < numPeriods; ++i)
	{
		//Calcula settlement date
		QCDate settlementDate = get<1>(_settlementBasicDates.at(i)).shift(_settlementCalendar, _settlementLag,
			QCDate::qcFollow);

		//Calcula la fecha de fixing con el fixing lag
		if (_fixingFlags.at(i))
		{
			fixingDate = get<0>(_settlementBasicDates.at(i)).
				shift(_fixingCalendar, _fixingLag, QCDate::qcPrev);
		}

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

void QCInterestRatePeriodsFactory::_setFixingFlags(unsigned int numPeriods)
{
	_fixingFlags.resize(numPeriods);
	unsigned int numWholeFixingPeriods = numPeriods / QCHelperFunctions::tenor(_fixingPeriodicity);
	unsigned int remainderFixing = numPeriods % QCHelperFunctions::tenor(_fixingPeriodicity);

	if (_fixingStubPeriod == QCInterestRateLeg::qcNoStubPeriod)
	{
		for (unsigned int i = 0; i < numPeriods; ++i)
		{
			if (i % QCHelperFunctions::tenor(_fixingPeriodicity) == 0)
			{
				_fixingFlags.at(i) = true;
			}
			else
			{
				_fixingFlags.at(i) = false;
			}
		}
	}
	else if (_fixingStubPeriod == QCInterestRateLeg::qcShortFront)
	{
		_fixingFlags.at(0) = true;
		_fixingFlags.at(remainderFixing) = true;
		for (unsigned int i = remainderFixing + 1; i < numPeriods; ++i)
		{
			if (i % QCHelperFunctions::tenor(_fixingPeriodicity) == remainderFixing)
			{
				_fixingFlags.at(i) = true;
			}
			else
			{
				_fixingFlags.at(i) = false;
			}
		}
	}
	else if (_fixingStubPeriod == QCInterestRateLeg::qcLongFront)
	{
		_fixingFlags.at(0) = true;
		_fixingFlags.at(remainderFixing + QCHelperFunctions::tenor(_fixingPeriodicity)) = true;
		for (unsigned int i = remainderFixing + QCHelperFunctions::tenor(_fixingPeriodicity) + 1;
			i < numPeriods; ++i)
		{
			if (i % QCHelperFunctions::tenor(_fixingPeriodicity) == remainderFixing)
			{
				_fixingFlags.at(i) = true;
			}
			else
			{
				_fixingFlags.at(i) = false;
			}
		}

	}
	else if (_fixingStubPeriod == QCInterestRateLeg::qcShortBack)
	{
		for (unsigned int i = 0; i < numPeriods; ++i)
		{
			if (i % QCHelperFunctions::tenor(_fixingPeriodicity) == 0)
			{
				_fixingFlags.at(i) = true;
			}
			else
			{
				_fixingFlags.at(i) = false;
			}
		}
	}
	else if (_fixingStubPeriod == QCInterestRateLeg::qcLongBack)
	{
		for (unsigned int i = 0; i < numPeriods - 1; ++i)
		{
			if (i % QCHelperFunctions::tenor(_fixingPeriodicity) == 0)
			{
				_fixingFlags.at(i) = true;
			}
			else
			{
				_fixingFlags.at(i) = false;
			}
		}
	}
}

vector<tuple<QCDate, QCDate>> QCInterestRatePeriodsFactory::_buildBasicDates(string periodicity,
	QCInterestRateLeg::QCStubPeriod stubPeriod, shared_ptr<vector<QCDate>> calendar)
{
	//Aqui se almacena el resultado
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

vector<tuple<QCDate, QCDate>> QCInterestRatePeriodsFactory::_buildBasicDates2(string periodicity,
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
	unsigned int remainderInDays = get<1>(distanceMonthsDays);

	//Se perfecciona el input. Si numWholePeriods == 0, solo hay un periodo CORTO INICIO
	if (numWholePeriods == 0)
	{
		stubPeriod = QCInterestRateLeg::qcShortFront;
	}
	bool easyCase = remainderInDays == 0 && remainderInMonths == 0;
	if (easyCase)
	{
		bool easyStub = stubPeriod != QCInterestRateLeg::qcLongFront2 &&
			stubPeriod != QCInterestRateLeg::qcLongFront3 &&
			stubPeriod != QCInterestRateLeg::qcLongFront4 &&
			stubPeriod != QCInterestRateLeg::qcLongFront5 &&
			stubPeriod != QCInterestRateLeg::qcLongFront6 &&
			stubPeriod != QCInterestRateLeg::qcLongFront7 &&
			stubPeriod != QCInterestRateLeg::qcLongFront8 &&
			stubPeriod != QCInterestRateLeg::qcLongFront9 &&
			stubPeriod != QCInterestRateLeg::qcLongFront10 &&
			stubPeriod != QCInterestRateLeg::qcLongFront11 &&
			stubPeriod != QCInterestRateLeg::qcLongFront12 &&
			stubPeriod != QCInterestRateLeg::qcLongFront120 &&
			stubPeriod != QCInterestRateLeg::qcLongFront121;
			if (easyStub)
		{
			QCDate fechaInicioPeriodo = _startDate;
			QCDate fechaFinalPeriodo;
			periods.resize(numWholePeriods);
			for (unsigned int i = 0; i < numWholePeriods; ++i)
			{
				fechaFinalPeriodo = _startDate.addMonths(QCHelperFunctions::tenor(periodicity) * (i + 1)).businessDay(
					calendar, _endDateAdjustment);
				periods.at(i) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
				fechaInicioPeriodo = fechaFinalPeriodo;
			}
			return periods;
		}
		else if (stubPeriod == QCInterestRateLeg::qcLongFront2)
		{
			unsigned int displacement = 1;
			_auxWithLongFrontNEasyCase(displacement, numWholePeriods, periods, periodicity, calendar);
			return periods;
			/*vector<tuple<QCDate, QCDate>> tempPeriods;
			QCDate fechaInicioPeriodo = _startDate;
			QCDate fechaFinalPeriodo;
			tempPeriods.resize(numWholePeriods);
			for (unsigned int i = 0; i < numWholePeriods; ++i)
			{
				fechaFinalPeriodo = _startDate.addMonths(QCHelperFunctions::tenor(periodicity) * (i + 1)).businessDay(
					calendar, _endDateAdjustment);
				tempPeriods.at(i) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
				fechaInicioPeriodo = fechaFinalPeriodo;
			}
			if (numWholePeriods == 1)
			{
				periods = tempPeriods;
			}
			else
			{
				periods.resize(numWholePeriods - 1);
				fechaInicioPeriodo = get<0>(tempPeriods.at(0));
				fechaFinalPeriodo = get<1>(tempPeriods.at(1));
				periods.at(0) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
				for (unsigned int i = 2; i < numWholePeriods; ++i)
				{
					periods.at(i - 1) = tempPeriods.at(i);
				}
			}
			return periods;*/
		}
		else if (stubPeriod == QCInterestRateLeg::qcLongFront3)
		{
			unsigned int displacement = 2;
			_auxWithLongFrontNEasyCase(displacement, numWholePeriods, periods, periodicity, calendar);
			return periods;
		}
		else if (stubPeriod == QCInterestRateLeg::qcLongFront3)
		{
			unsigned int displacement = 2;
			_auxWithLongFrontNEasyCase(displacement, numWholePeriods, periods, periodicity, calendar);
			return periods;
		}
		else if (stubPeriod == QCInterestRateLeg::qcLongFront4)
		{
			unsigned int displacement = 3;
			_auxWithLongFrontNEasyCase(displacement, numWholePeriods, periods, periodicity, calendar);
			return periods;
		}
		else if (stubPeriod == QCInterestRateLeg::qcLongFront5)
		{
			unsigned int displacement = 4;
			_auxWithLongFrontNEasyCase(displacement, numWholePeriods, periods, periodicity, calendar);
			return periods;
		}
		else if (stubPeriod == QCInterestRateLeg::qcLongFront6)
		{
			unsigned int displacement = 5;
			_auxWithLongFrontNEasyCase(displacement, numWholePeriods, periods, periodicity, calendar);
			return periods;
		}
		else if (stubPeriod == QCInterestRateLeg::qcLongFront7)
		{
			unsigned int displacement = 6;
			_auxWithLongFrontNEasyCase(displacement, numWholePeriods, periods, periodicity, calendar);
			return periods;
		}
		else if (stubPeriod == QCInterestRateLeg::qcLongFront8)
		{
			unsigned int displacement = 7;
			_auxWithLongFrontNEasyCase(displacement, numWholePeriods, periods, periodicity, calendar);
			return periods;
		}
		else if (stubPeriod == QCInterestRateLeg::qcLongFront9)
		{
			unsigned int displacement = 8;
			_auxWithLongFrontNEasyCase(displacement, numWholePeriods, periods, periodicity, calendar);
			return periods;
		}
		else if (stubPeriod == QCInterestRateLeg::qcLongFront10)
		{
			unsigned int displacement = 9;
			_auxWithLongFrontNEasyCase(displacement, numWholePeriods, periods, periodicity, calendar);
			return periods;
		}
		else if (stubPeriod == QCInterestRateLeg::qcLongFront11)
		{
			unsigned int displacement = 10;
			_auxWithLongFrontNEasyCase(displacement, numWholePeriods, periods, periodicity, calendar);
			return periods;
		}
		else if (stubPeriod == QCInterestRateLeg::qcLongFront12)
		{
			unsigned int displacement = 11;
			_auxWithLongFrontNEasyCase(displacement, numWholePeriods, periods, periodicity, calendar);
			return periods;
		}
		else if (stubPeriod == QCInterestRateLeg::qcLongFront120)
		{
			unsigned int displacement = 119;
			_auxWithLongFrontNEasyCase(displacement, numWholePeriods, periods, periodicity, calendar);
			return periods;
		}
		else if (stubPeriod == QCInterestRateLeg::qcLongFront121)
		{
			unsigned int displacement = 120;
			_auxWithLongFrontNEasyCase(displacement, numWholePeriods, periods, periodicity, calendar);
			return periods;
		}


	}

	if (stubPeriod == QCInterestRateLeg::qcShortBack)
	{
		unsigned int numPeriods = numWholePeriods + 1;
		periods.resize(numPeriods);
		
		QCDate fechaInicioPeriodo = _startDate;
		QCDate fechaFinalPeriodo;
		
		if (numPeriods == 1)
		{
			fechaFinalPeriodo = _startDate.addMonths(remainderInMonths).businessDay(
				calendar, _endDateAdjustment);
			fechaFinalPeriodo = fechaFinalPeriodo.addDays(remainderInDays);
			periods.at(0) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
		}

		if (numPeriods > 1)
		{
			for (unsigned int i = 0; i < numPeriods - 1; ++i)
			{
				fechaFinalPeriodo = _startDate.addMonths(QCHelperFunctions::tenor(periodicity) * (i + 1))
					.businessDay(calendar, _endDateAdjustment);
				periods.at(i) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
				fechaInicioPeriodo = fechaFinalPeriodo;
			}
			
			fechaFinalPeriodo = fechaInicioPeriodo.addMonths(remainderInMonths);
			if (fechaFinalPeriodo <= _endDate)
			{
				fechaFinalPeriodo = fechaFinalPeriodo.moveToDayOfMonth(_endDate.day(), QCDate::qcForward)
					.businessDay(calendar, _endDateAdjustment);
			}
			else
			{
				fechaFinalPeriodo = fechaFinalPeriodo.moveToDayOfMonth(_endDate.day(), QCDate::qcBackward)
					.businessDay(calendar, _endDateAdjustment);
			}
			periods.at(numPeriods - 1) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
		}
	}
	if (stubPeriod == QCInterestRateLeg::qcLongBack)
	{
		unsigned int numPeriods = numWholePeriods;
		periods.resize(numPeriods);

		QCDate fechaInicioPeriodo = _startDate;
		QCDate fechaFinalPeriodo;

		if (numPeriods == 1)
		{
			fechaFinalPeriodo = _startDate.addMonths(QCHelperFunctions::tenor(
				periodicity) + remainderInMonths).businessDay(calendar, _endDateAdjustment);
			fechaFinalPeriodo = fechaFinalPeriodo.addDays(remainderInDays);
			periods.at(0) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
		}

		if (numPeriods > 1)
		{
			for (unsigned int i = 0; i < numPeriods - 1; ++i)
			{
				fechaFinalPeriodo = _startDate.addMonths(QCHelperFunctions::tenor(periodicity) * (i + 1)).businessDay(
					calendar, _endDateAdjustment);
				periods.at(i) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
				fechaInicioPeriodo = fechaFinalPeriodo;
			}
			
			fechaFinalPeriodo = _startDate.addMonths(numPeriods * QCHelperFunctions::tenor(periodicity)
				+ remainderInMonths).businessDay(calendar, _endDateAdjustment);
			fechaFinalPeriodo = fechaFinalPeriodo.addDays(remainderInDays);
			periods.at(numPeriods - 1) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
		}
	}
	if (stubPeriod == QCInterestRateLeg::qcShortFront)
	{
		unsigned int numPeriods = numWholePeriods + 1;
		periods.resize(numPeriods);

		QCDate fechaInicioPeriodo = _startDate;
		QCDate fechaFinalPeriodo;

		if (numPeriods == 1)
		{
			fechaFinalPeriodo = _startDate.addMonths(remainderInMonths);
			fechaFinalPeriodo = fechaFinalPeriodo.moveToDayOfMonth(_endDate.day(), QCDate::qcForward).
				businessDay(calendar, _endDateAdjustment);
			periods.at(0) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
		}

		if (numPeriods > 1)
		{
			fechaFinalPeriodo = _startDate.addMonths(remainderInMonths);
			fechaFinalPeriodo = fechaFinalPeriodo.moveToDayOfMonth(_endDate.day(), QCDate::qcForward);
			periods.at(0) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
			fechaInicioPeriodo = fechaFinalPeriodo;

			QCDate pseudoStartDate = fechaFinalPeriodo;

			for (unsigned int i = 1; i < numPeriods; ++i)
			{
				fechaFinalPeriodo = pseudoStartDate.addMonths(QCHelperFunctions::tenor(periodicity) * i)
					.businessDay(calendar, _endDateAdjustment);
				periods.at(i) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
				fechaInicioPeriodo = fechaFinalPeriodo;
			}
			//Ejecuta el end_date_adjustment de la fecha final del primer período y en 
			//consecuencia de la fecha inicial del segundo período.
			get<1>(periods.at(0)) = get<1>(periods.at(0)).businessDay(calendar, _endDateAdjustment);
			get<0>(periods.at(1)) = get<1>(periods.at(0));
		}

	}
	if (stubPeriod == QCInterestRateLeg::qcLongFront)
	{
		unsigned int numPeriods = numWholePeriods;
		periods.resize(numPeriods);

		QCDate fechaInicioPeriodo = _startDate;
		QCDate fechaFinalPeriodo;
		
		if (numPeriods == 1)
		{
			fechaFinalPeriodo = _startDate.addMonths(QCHelperFunctions::tenor(periodicity) + remainderInMonths).
				businessDay(calendar, _endDateAdjustment);
			fechaFinalPeriodo = fechaFinalPeriodo.addDays(remainderInDays)
				.businessDay(calendar, _endDateAdjustment);
			periods.at(0) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
		}

		if (numPeriods > 1)
		{
			fechaFinalPeriodo = _startDate.addMonths(QCHelperFunctions::tenor(periodicity) + remainderInMonths);
			fechaFinalPeriodo = fechaFinalPeriodo.moveToDayOfMonth(_endDate.day(), QCDate::qcForward,
				_endDate.isEndOfMonth());
			periods.at(0) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
			fechaInicioPeriodo = fechaFinalPeriodo;

			QCDate pseudoStartDate = fechaInicioPeriodo;

			for (unsigned int i = 1; i < numPeriods; ++i)
			{
				fechaFinalPeriodo = pseudoStartDate.addMonths(i * QCHelperFunctions::tenor(periodicity)).
					businessDay(calendar, _endDateAdjustment);
				periods.at(i) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
				fechaInicioPeriodo = fechaFinalPeriodo;
			}
			//Ejecuta el end_date_adjustment de la fecha final del primer período y en 
			//consecuencia de la fecha inicial del segundo período.
			get<1>(periods.at(0)) = get<1>(periods.at(0)).businessDay(calendar, _endDateAdjustment);
			get<0>(periods.at(1)) = get<1>(periods.at(0));
		}
	}
	if (stubPeriod == QCInterestRateLeg::qcLongFront2)
	{
		unsigned int numPeriods = numWholePeriods + 1;
		vector<tuple<QCDate, QCDate>> tempPeriods;
		tempPeriods.resize(numPeriods);

		QCDate fechaInicioPeriodo = _startDate;
		QCDate fechaFinalPeriodo;

		if (numPeriods == 1)
		{
			fechaFinalPeriodo = _startDate.addMonths(remainderInMonths);
			fechaFinalPeriodo = fechaFinalPeriodo.moveToDayOfMonth(_endDate.day(), QCDate::qcForward).
				businessDay(calendar, _endDateAdjustment);
			tempPeriods.at(0) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
		}

		if (numPeriods > 1)
		{
			fechaFinalPeriodo = _startDate.addMonths(remainderInMonths);
			fechaFinalPeriodo = fechaFinalPeriodo.moveToDayOfMonth(_endDate.day(), QCDate::qcForward);
			tempPeriods.at(0) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
			fechaInicioPeriodo = fechaFinalPeriodo;

			QCDate pseudoStartDate = fechaFinalPeriodo;

			for (unsigned int i = 1; i < numPeriods; ++i)
			{
				fechaFinalPeriodo = pseudoStartDate.addMonths(QCHelperFunctions::tenor(periodicity) * i)
					.businessDay(calendar, _endDateAdjustment);
				tempPeriods.at(i) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
				fechaInicioPeriodo = fechaFinalPeriodo;
			}
			//Ejecuta el end_date_adjustment de la fecha final del primer período y en 
			//consecuencia de la fecha inicial del segundo período.
			get<1>(tempPeriods.at(0)) = get<1>(tempPeriods.at(0)).businessDay(calendar, _endDateAdjustment);
			get<0>(tempPeriods.at(1)) = get<1>(tempPeriods.at(0));
		}
		if (numPeriods == 1)
		{
			periods = tempPeriods;
		}
		else
		{
			periods.resize(numPeriods - 1);
			fechaInicioPeriodo = get<0>(tempPeriods.at(0));
			fechaFinalPeriodo = get<1>(tempPeriods.at(1));
			periods.at(0) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
			for (unsigned int i = 2; i < numPeriods; ++i)
			{
				periods.at(i - 1) = tempPeriods.at(i);
			}
		}
	}
	if (stubPeriod == QCInterestRateLeg::qcLongFront3)
	{
		unsigned int displacement = 2;
		_auxWithLongFrontNNotEasyCase(displacement, numWholePeriods, remainderInMonths,
			remainderInDays, periods, periodicity, calendar);
	}
	if (stubPeriod == QCInterestRateLeg::qcLongFront4)
	{
		unsigned int displacement = 3;
		_auxWithLongFrontNNotEasyCase(displacement, numWholePeriods, remainderInMonths,
			remainderInDays, periods, periodicity, calendar);
	}
	if (stubPeriod == QCInterestRateLeg::qcLongFront5)
	{
		unsigned int displacement = 4;
		_auxWithLongFrontNNotEasyCase(displacement, numWholePeriods, remainderInMonths,
			remainderInDays, periods, periodicity, calendar);
	}
	if (stubPeriod == QCInterestRateLeg::qcLongFront6)
	{
		unsigned int displacement = 5;
		_auxWithLongFrontNNotEasyCase(displacement, numWholePeriods, remainderInMonths,
			remainderInDays, periods, periodicity, calendar);
	}
	if (stubPeriod == QCInterestRateLeg::qcLongFront7)
	{
		unsigned int displacement = 6;
		_auxWithLongFrontNNotEasyCase(displacement, numWholePeriods, remainderInMonths,
			remainderInDays, periods, periodicity, calendar);
	}
	if (stubPeriod == QCInterestRateLeg::qcLongFront8)
	{
		unsigned int displacement = 7;
		_auxWithLongFrontNNotEasyCase(displacement, numWholePeriods, remainderInMonths,
			remainderInDays, periods, periodicity, calendar);
	}
	if (stubPeriod == QCInterestRateLeg::qcLongFront9)
	{
		unsigned int displacement = 8;
		_auxWithLongFrontNNotEasyCase(displacement, numWholePeriods, remainderInMonths,
			remainderInDays, periods, periodicity, calendar);
	}
	if (stubPeriod == QCInterestRateLeg::qcLongFront10)
	{
		unsigned int displacement = 9;
		_auxWithLongFrontNNotEasyCase(displacement, numWholePeriods, remainderInMonths,
			remainderInDays, periods, periodicity, calendar);
	}
	if (stubPeriod == QCInterestRateLeg::qcLongFront11)
	{
		unsigned int displacement = 10;
		_auxWithLongFrontNNotEasyCase(displacement, numWholePeriods, remainderInMonths,
			remainderInDays, periods, periodicity, calendar);
	}
	if (stubPeriod == QCInterestRateLeg::qcLongFront12)
	{
		unsigned int displacement = 11;
		_auxWithLongFrontNNotEasyCase(displacement, numWholePeriods, remainderInMonths,
			remainderInDays, periods, periodicity, calendar);
	}
	if (stubPeriod == QCInterestRateLeg::qcLongFront120)
	{
		unsigned int displacement = 119;
		_auxWithLongFrontNNotEasyCase(displacement, numWholePeriods, remainderInMonths,
			remainderInDays, periods, periodicity, calendar);
	}
	if (stubPeriod == QCInterestRateLeg::qcLongFront121)
	{
		unsigned int displacement = 120;
		_auxWithLongFrontNNotEasyCase(displacement, numWholePeriods, remainderInMonths,
			remainderInDays, periods, periodicity, calendar);
	}

	return periods;
}

void QCInterestRatePeriodsFactory::_auxWithLongFrontNEasyCase(unsigned int displacement,
	unsigned int numWholePeriods, vector<tuple<QCDate, QCDate>>& periods,
	string& periodicity, shared_ptr<vector<QCDate>>& calendar)
{
	vector<tuple<QCDate, QCDate>> tempPeriods;
	QCDate fechaInicioPeriodo = _startDate;
	QCDate fechaFinalPeriodo;
	tempPeriods.resize(numWholePeriods);
	for (unsigned int i = 0; i < numWholePeriods; ++i)
	{
		fechaFinalPeriodo = _startDate.addMonths(QCHelperFunctions::tenor(periodicity) * (i + 1)).businessDay(
			calendar, _endDateAdjustment);
		tempPeriods.at(i) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
		fechaInicioPeriodo = fechaFinalPeriodo;
	}
	if (numWholePeriods <= displacement)
	{
		periods = tempPeriods;
	}
	else
	{
		periods.resize(numWholePeriods - displacement);
		fechaInicioPeriodo = get<0>(tempPeriods.at(0));
		fechaFinalPeriodo = get<1>(tempPeriods.at(displacement));
		periods.at(0) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
		for (unsigned int i = displacement + 1; i < numWholePeriods; ++i)
		{
			periods.at(i - displacement) = tempPeriods.at(i);
		}
	}

}

void QCInterestRatePeriodsFactory::_auxWithLongFrontNNotEasyCase(unsigned int displacement,
	unsigned int numWholePeriods, unsigned int remainderInMonths, unsigned int remainderInDays,
	vector<tuple<QCDate, QCDate>>& periods, string& periodicity, shared_ptr<vector<QCDate>>& calendar)
{
	unsigned int numPeriods = numWholePeriods + 1;
	vector<tuple<QCDate, QCDate>> tempPeriods;
	tempPeriods.resize(numPeriods);

	QCDate fechaInicioPeriodo = _startDate;
	QCDate fechaFinalPeriodo;

	if (numPeriods == 1)
	{
		fechaFinalPeriodo = _startDate.addMonths(remainderInMonths);
		fechaFinalPeriodo = fechaFinalPeriodo.moveToDayOfMonth(_endDate.day(), QCDate::qcForward).
			businessDay(calendar, _endDateAdjustment);
		tempPeriods.at(0) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
	}

	if (numPeriods > 1)
	{
		fechaFinalPeriodo = _startDate.addMonths(remainderInMonths);
		fechaFinalPeriodo = fechaFinalPeriodo.moveToDayOfMonth(_endDate.day(), QCDate::qcForward);
		tempPeriods.at(0) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
		fechaInicioPeriodo = fechaFinalPeriodo;

		QCDate pseudoStartDate = fechaFinalPeriodo;

		for (unsigned int i = 1; i < numPeriods; ++i)
		{
			fechaFinalPeriodo = pseudoStartDate.addMonths(QCHelperFunctions::tenor(periodicity) * i)
				.businessDay(calendar, _endDateAdjustment);
			tempPeriods.at(i) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
			fechaInicioPeriodo = fechaFinalPeriodo;
		}
		//Ejecuta el end_date_adjustment de la fecha final del primer período y en 
		//consecuencia de la fecha inicial del segundo período.
		get<1>(tempPeriods.at(0)) = get<1>(tempPeriods.at(0)).businessDay(calendar, _endDateAdjustment);
		get<0>(tempPeriods.at(1)) = get<1>(tempPeriods.at(0));
	}
	if (numPeriods <= displacement)
	{
		periods = tempPeriods;
	}
	else
	{
		periods.resize(numPeriods - displacement);
		fechaInicioPeriodo = get<0>(tempPeriods.at(0));
		fechaFinalPeriodo = get<1>(tempPeriods.at(displacement));
		periods.at(0) = make_tuple(fechaInicioPeriodo, fechaFinalPeriodo);
		for (unsigned int i = displacement + 1; i < numPeriods; ++i)
		{
			periods.at(i - displacement) = tempPeriods.at(i);
		}
	}

}

QCInterestRatePeriodsFactory::~QCInterestRatePeriodsFactory()
{}

