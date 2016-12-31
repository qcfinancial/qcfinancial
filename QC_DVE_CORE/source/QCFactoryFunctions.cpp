#include <algorithm>

#include "QCFactoryFunctions.h"
#include "QCHelperFunctions.h"
#include "QCAct360.h"
#include "QCAct365.h"
#include "QCActAct.h"
#include "QC30360.h"
#include "QCLinearWf.h"
#include "QCCompoundWf.h"
#include "QCContinousWf.h"
#include "QCCurve.h"
#include "QCInterpolator.h"
#include "QCLinearInterpolator.h"
#include "QCLogLinearInterpolator.h"
#include "QCInterestRateCurve.h"
#include "QCProjectingInterestRateCurve.h"
#include "QCZeroCouponInterestRateCurve.h"
#include "QCZeroCouponDiscountFactorCurve.h"
#include "QCInterestRate.h"

using namespace QCHelperFunctions;
using namespace std;

shared_ptr<QCYearFraction> QCFactoryFunctions::yfSharedPtr(const string& whichYf)
{
	if (whichYf == "act/360")
	{
		shared_ptr<QCYearFraction> result(new QCAct360);
		return result;
	}
	if (whichYf == "act/365")
	{
		shared_ptr<QCYearFraction> result(new QCAct365);
		return result;
	}
	if (whichYf == "act/act")
	{
		shared_ptr<QCYearFraction> result(new QCActAct);
		return result;
	}
	if (whichYf == "30/360")
	{
		shared_ptr<QCYearFraction> result(new QC30360);
		return result;
	}
	return 0;
}

shared_ptr<QCWealthFactor> QCFactoryFunctions::wfSharedPtr(const string& whichWf)
{
	if (whichWf == "lin")
	{
		shared_ptr<QCWealthFactor> result(new QCLinearWf);
		return result;
	}
	if (whichWf == "com")
	{
		shared_ptr<QCWealthFactor> result(new QCCompoundWf);
		return result;
	}
	if (whichWf == "con")
	{
		shared_ptr<QCWealthFactor> result(new QCContinousWf);
		return result;
	}
	return 0;
}

shared_ptr<QCInterestRate> QCFactoryFunctions::intRateSharedPtr(double value,
	const string& whichYf, const string& whichWf)
{
	//Constructor de QCInterestRate
	//QCInterestRate(double value, QCYrFrctnShrdPtr yearFraction, QCWlthFctrShrdPtr wealthFactor)

	return make_shared<QCInterestRate>(QCInterestRate{value,
		QCFactoryFunctions::yfSharedPtr(whichYf),
		QCFactoryFunctions::wfSharedPtr(whichWf) });
}

QCZrCpnCrvShrdPtr QCFactoryFunctions::zrCpnCrvShrdPtr(vector<long>& tenors, vector<double>& rates,
	const string& interpolator, const string& yf, const string& wf)
{
	//QCZeroCouponCurve es abstracta pero instanciable
	shared_ptr<QCCurve<long>> crvPtr(new QCCurve<long>{ tenors, rates });
	shared_ptr<QCInterpolator> interpol;
	if (interpolator == "LINEAR")
	{
		interpol = make_shared<QCLinearInterpolator>(crvPtr);
	}
	else
	{
		interpol = make_shared<QCLinearInterpolator>(crvPtr);
	}
	
	//definir un interest rate y meterlo al constructor
	shared_ptr<QCYearFraction> yfShrdPtr = QCFactoryFunctions::yfSharedPtr(yf);
	shared_ptr<QCWealthFactor> wfShrdPtr = QCFactoryFunctions::wfSharedPtr(wf);
	QCInterestRate intRate{ 0, yfShrdPtr, wfShrdPtr };
	QCZrCpnCrvShrdPtr zrCrvPtr(new QCZeroCouponInterestRateCurve{ interpol, intRate });

	return zrCrvPtr;
}

QCZrCpnCrvShrdPtr QCFactoryFunctions::zrCpnCrvShrdPtr(vector<long>& tenors, vector<double>& rates,
	const string& interpolator, const string& wf, const string& yf, const string& typeCurve)
{
	//QCZeroCouponCurve es abstracta pero instanciable
	shared_ptr<QCCurve<long>> crvPtr(new QCCurve<long>{ tenors, rates });
	shared_ptr<QCInterpolator> interpol;
	if (interpolator == "LINEAR")
	{
		interpol = make_shared<QCLinearInterpolator>(crvPtr);
	}
	else
	{
		interpol = make_shared<QCLinearInterpolator>(crvPtr);
	}

	//definir un interest rate y meterlo al constructor
	shared_ptr<QCYearFraction> yfShrdPtr = QCFactoryFunctions::yfSharedPtr(yf);
	shared_ptr<QCWealthFactor> wfShrdPtr = QCFactoryFunctions::wfSharedPtr(wf);
	QCInterestRate intRate{ 0, yfShrdPtr, wfShrdPtr };
	QCZrCpnCrvShrdPtr zrCrvPtr(new QCZeroCouponInterestRateCurve{ interpol, intRate });

	return zrCrvPtr;
}

shared_ptr<QCInterestRateCurve> QCFactoryFunctions::intRtCrvShrdPtr(vector<long>& tenors, vector<double>& rates,
	const string& interpolator, const string& wf, const string& yf,
	QCInterestRateCurve::QCTypeInterestRateCurve typeCurve)
{
	//QCZeroCouponCurve es abstracta pero instanciable
	shared_ptr<QCCurve<long>> crvPtr(new QCCurve<long>{ tenors, rates });
	shared_ptr<QCInterpolator> interpol;
	if (interpolator == "LINEAR")
	{
		interpol = make_shared<QCLinearInterpolator>(crvPtr);
	}
	else
	{
		interpol = make_shared<QCLinearInterpolator>(crvPtr);
	}

	//definir un interest rate y meterlo al constructor
	shared_ptr<QCYearFraction> yfShrdPtr = QCFactoryFunctions::yfSharedPtr(yf);
	shared_ptr<QCWealthFactor> wfShrdPtr = QCFactoryFunctions::wfSharedPtr(wf);
	QCInterestRate intRate{ 0, yfShrdPtr, wfShrdPtr };
	shared_ptr<QCInterestRateCurve> result;
	switch (typeCurve)
	{
	case QCInterestRateCurve::qcProjectingCurve:
		result = make_shared<QCProjectingInterestRateCurve>(
			QCProjectingInterestRateCurve{ interpol, intRate });
		break;
	case QCInterestRateCurve::qcZeroCouponCurve:
		result = make_shared<QCZeroCouponInterestRateCurve>(
			QCZeroCouponInterestRateCurve{ interpol, intRate });
		break;
	case QCInterestRateCurve::qcDiscountFactorCurve:
		result = make_shared<QCZeroCouponDiscountFactorCurve>(
			QCZeroCouponDiscountFactorCurve{ interpol, intRate });
		break;
	default:
		break;
	}

	return result;
}

QCIntRtCrvShrdPtr QCFactoryFunctions::discFctrCrvShrdPtr(vector<long>& tenors, vector<double>& dfs,
	const string& interpolator, const string& yf, const string& wf)
{
	shared_ptr<QCCurve<long>> crvPtr(new QCCurve<long>{ tenors, dfs });
	shared_ptr<QCInterpolator> interpol;
	if (interpolator == "LOGLINEAR")
	{
		interpol = make_shared<QCLogLinearInterpolator>(crvPtr);
	}
	else
	{
		interpol = make_shared<QCLogLinearInterpolator>(crvPtr);
	}

	//definir un interest rate y meterlo al constructor
	shared_ptr<QCYearFraction> yfShrdPtr = QCFactoryFunctions::yfSharedPtr(yf);
	shared_ptr<QCWealthFactor> wfShrdPtr = QCFactoryFunctions::wfSharedPtr(wf);
	QCInterestRate intRate{ 0, yfShrdPtr, wfShrdPtr };
	QCIntRtCrvShrdPtr discFctrCrvPtr(new QCZeroCouponDiscountFactorCurve{ interpol, intRate });

	return discFctrCrvPtr;
}

QCInterestRateLeg QCFactoryFunctions::buildFixedRateLeg(
	string receivePay,			//receive or pay
	QCDate startDate,			//start date
	QCDate endDate,				//end date
	vector<QCDate> calendar,	//settlement calendar
	int settlementLag,			//settlement lag
	QCInterestRateLeg::QCStubPeriod stubPeriod,	//stub period
	string periodicity,			//periodicity
	QCDate::QCBusDayAdjRules endDateAdjustment,		//end date adjustment
	QCInterestRateLeg::QCAmortization amortization,	//amortization
	vector<tuple<QCDate, double, double>> amortNotionalByDate, //amortization and notional by date
	double notional				//notional
	)
{
	//QCInterestRatePeriod es:
	//tuple<double, bool, double, bool, double, QCDate, QCDate, QCDate, QCDate, QCDate, QCDate>
	//intRtPrdElmntInitialAccrtn,	/*!< Disposicion inicial del periodo */
	//intRtPrdElmntAcctrnIsCshflw,	/*!< Indica si la disposicion inicial es flujo */
	//intRtPrdElmntFinalAmrtztn,	/*!< Amortizacion final del periodo */
	//inRtPrdElmntAmrtztnIsCshflw,	/*!< Indica si la amortizacion final es flujo */
	//intRtPrdElmntNotional,		/*!< Nocional vigente del periodo */
	//intRtPrdElmntStartDate,		/*!< Fecha de inicio del periodo */
	//intRtPrdElmntEndDate,			/*!< Fecha final del periodo */
	//intRtPrdElmntSettlmntDate,	/*!< Fecha de pago del flujo */

	//Las siguientes 3 son para patas flotantes
	//intRtPrdElmntFxngDate,		/*!< Fecha de fixing del indice */
	//intRtPrdElmntFxngInitDate,	/*!< Fecha de inicio de devengo del indice */
	//intRtPrdElmntFxngEndDate		/*!< Fecha final de devengo del indice */

	//Aqui se guardaran los periodos
	QCInterestRateLeg::QCInterestRatePeriods periods;

	//Apenas se calcule el numero de periodos se registrara aqui
	unsigned int numPeriods;
	
	//Sirve para determinar el signo de nocional vigente, disposicion y amortizacion
	int signo;
	if (receivePay == "R") { signo = 1;} else { signo = -1; }
	
	//La distancia en meses y dias entre startDate y endDate
	tuple<unsigned long, int> distanceMonthsDays = startDate.monthDiffDayRemainder(
		endDate, calendar, endDateAdjustment);

	//Cuantos periodos enteros hay entre startDate y endDate y cuantos meses sobran
	if (amortization != QCInterestRateLeg::qcCustomAmort)
	{
		unsigned int numWholePeriods = get<0>(distanceMonthsDays) / QCHelperFunctions::tenor(periodicity);
		unsigned int remainderInMonths = get<0>(distanceMonthsDays) % QCHelperFunctions::tenor(periodicity);
		if (stubPeriod == QCInterestRateLeg::qcShortBack || stubPeriod == QCInterestRateLeg::qcNoStubPeriod)
		{
			/*
			Case "CORTO FINAL"
				periods = numWholePeriods + 1 - Indicador(remainderMonths + dDias, 0)
				ReDim result(1 To periods, 1 To 2) As Date
				result(1, 1) = startDate
				result(1, 2) = boolBusDay(busDay, AddMonths(startDate, Ten(periodicity)))
				For i = 2 To periods - 1
					result(i, 1) = result(i - 1, 2)
					result(i, 2) = boolBusDay(busDay, AddMonths(startDate, i * Ten(periodicity)))
				Next i
				result(periods, 1) = result(periods - 1, 2)
				result(periods, 2) = endDate
			*/
			int indicador = 0;
			if (remainderInMonths + get<1>(distanceMonthsDays) == 0)
				indicador = 1;
			periods.resize(numWholePeriods + 1- indicador);

			QCDate fechaInicioPeriodo = startDate;
			QCDate fechaFinalPeriodo = startDate.addMonths(tenor(periodicity)).businessDay(
				calendar, endDateAdjustment);
			QCDate fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);

			periods.at(0) = make_tuple(-signo * notional, false, 0, true, 0,
				fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
				fechaInicioPeriodo, fechaInicioPeriodo, fechaInicioPeriodo);

			for (unsigned int i = 1; i < periods.size() - 1; ++i)
			{
				fechaInicioPeriodo = fechaFinalPeriodo;
				fechaFinalPeriodo = startDate.addMonths(tenor(periodicity) * (i + 1)).businessDay(
					calendar, endDateAdjustment);
				fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);
				periods.at(i) = make_tuple(0, false, 0, true, 0,
					fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
					fechaInicioPeriodo, fechaInicioPeriodo, fechaInicioPeriodo);
			}
			fechaInicioPeriodo = fechaFinalPeriodo;
			fechaFinalPeriodo = endDate;
			fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);
			periods.at(periods.size() - 1) = make_tuple(0, false, 0, true, 0,
				fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
				fechaInicioPeriodo, fechaInicioPeriodo, fechaInicioPeriodo);
		}
		if (stubPeriod == QCInterestRateLeg::qcLongBack)
		{
			/*
			Case "LARGO FINAL"
				periods = numWholePeriods
				ReDim result(1 To periods, 1 To 2) As Date
				result(1, 1) = startDate
				result(1, 2) = boolBusDay(busDay, AddMonths(startDate, Ten(periodicity)))
				For i = 2 To periods - 1
					result(i, 1) = result(i - 1, 2)
					result(i, 2) = boolBusDay(busDay, AddMonths(startDate, i * Ten(periodicity)))
				Next i
				result(periods, 1) = result(periods - 1, 2)
				result(periods, 2) = endDate
			*/
			periods.resize(numWholePeriods);

			QCDate fechaInicioPeriodo = startDate;
			QCDate fechaFinalPeriodo = startDate.addMonths(tenor(periodicity)).businessDay(
				calendar, endDateAdjustment);
			QCDate fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);

			periods.at(0) = make_tuple(-signo * notional, false, 0, true, 0,
				fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
				fechaInicioPeriodo, fechaInicioPeriodo, fechaInicioPeriodo);

			for (unsigned int i = 1; i < periods.size() - 1; ++i)
			{
				fechaInicioPeriodo = fechaFinalPeriodo;
				fechaFinalPeriodo = startDate.addMonths(tenor(periodicity) * (i + 1)).businessDay(
					calendar, endDateAdjustment);
				fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);
				periods.at(i) = make_tuple(0, false, 0, true, 0,
					fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
					fechaInicioPeriodo, fechaInicioPeriodo, fechaInicioPeriodo);
			}
			fechaInicioPeriodo = fechaFinalPeriodo;
			fechaFinalPeriodo = endDate;
			fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);
			periods.at(periods.size() - 1) = make_tuple(0, false, 0, true, 0,
				fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
				fechaInicioPeriodo, fechaInicioPeriodo, fechaInicioPeriodo);
		}
		if (stubPeriod == QCInterestRateLeg::qcShortFront)
		{
			/*
			Case "CORTO INICIO"
				periods = numWholePeriods + 1 - Indicador(remainderMonths + dDias, 0)
				ReDim result(1 To periods, 1 To 2) As Date
				result(periods, 1) = BussDay(AddMonths(endDate, -Ten(periodicity)))
				result(periods, 2) = endDate
				For i = periods - 1 To 2 Step -1
					result(i, 1) = boolBusDay(busDay, AddMonths(endDate, (i - periods - 1) * Ten(periodicity)))
					result(i, 2) = result(i + 1, 1)
				Next i
				result(1, 1) = startDate
				result(1, 2) = result(2, 1)
			*/

			int indicador = 0;
			if (remainderInMonths + get<1>(distanceMonthsDays) == 0)
				indicador = 1;
			periods.resize(numWholePeriods + 1 - indicador);

			QCDate fechaFinalPeriodo = endDate;
			QCDate fechaInicioPeriodo = endDate.addMonths(-tenor(periodicity)).businessDay(
				calendar, endDateAdjustment);
			QCDate fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);

			unsigned int numPeriods = periods.size();
			periods.at(numPeriods - 1) = make_tuple(0, false, 0, true, 0,
				fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
				fechaInicioPeriodo, fechaInicioPeriodo, fechaInicioPeriodo);

			if (numPeriods >= 2)
			{
				if (numPeriods > 2)
				{
					for (unsigned int i = numPeriods - 2; i > 0; --i)
					{
						fechaFinalPeriodo = fechaInicioPeriodo;
						fechaInicioPeriodo = endDate.addMonths((i - numPeriods) * tenor(periodicity)).businessDay(
							calendar, endDateAdjustment);
						fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);
						periods.at(i) = make_tuple(0, false, 0, true, 0,
							fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
							fechaInicioPeriodo, fechaInicioPeriodo, fechaFinalPeriodo);
					}
				}

				fechaFinalPeriodo = fechaInicioPeriodo;
				fechaInicioPeriodo = startDate;
				fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);
				periods.at(0) = make_tuple(0, false, 0, true, 0,
					fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
					fechaInicioPeriodo, fechaInicioPeriodo, fechaInicioPeriodo);
			}
		}
		if (stubPeriod == QCInterestRateLeg::qcLongFront)
		{
			/*
			Case "LARGO INICIO"
				periods = numWholePeriods
				ReDim result(1 To periods, 1 To 2) As Date
				result(periods, 1) = BussDay(AddMonths(endDate, -Ten(periodicity)))
				result(periods, 2) = endDate
				For i = periods - 1 To 2 Step -1
					result(i, 1) = boolBusDay(busDay, AddMonths(endDate, (i - periods - 1) * Ten(periodicity)))
					result(i, 2) = result(i + 1, 1)
				Next i
				result(1, 1) = startDate
				result(1, 2) = result(2, 1)
			*/

			periods.resize(numWholePeriods);

			QCDate fechaFinalPeriodo = endDate;
			QCDate fechaInicioPeriodo = endDate.addMonths(-tenor(periodicity)).businessDay(
				calendar, endDateAdjustment);
			QCDate fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);

			unsigned int numPeriods = periods.size();
			periods.at(numPeriods - 1) = make_tuple(0, false, 0, true, 0,
				fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
				fechaInicioPeriodo, fechaInicioPeriodo, fechaInicioPeriodo);

			if (numPeriods >= 2)
			{
				if (numPeriods > 2)
				{
					for (unsigned int i = numPeriods - 2; i > 0; --i)
					{
						fechaFinalPeriodo = fechaInicioPeriodo;
						fechaInicioPeriodo = endDate.addMonths((i - numPeriods) * tenor(periodicity)).businessDay(
							calendar, endDateAdjustment);
						fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);
						periods.at(i) = make_tuple(0, false, 0, true, 0,
							fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
							fechaInicioPeriodo, fechaInicioPeriodo, fechaFinalPeriodo);
					}
				}

				fechaFinalPeriodo = fechaInicioPeriodo;
				fechaInicioPeriodo = startDate;
				fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);

				periods.at(0) = make_tuple(0, false, 0, true, 0,
					fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
					fechaInicioPeriodo, fechaInicioPeriodo, fechaInicioPeriodo);

			}
		}
		//Calcular amortizaciones y nocionales vigentes
		
		numPeriods = periods.size();
		if (amortization == QCInterestRateLeg::qcBulletAmort)
		{
			for (unsigned int i = 0; i < numPeriods; ++i)
			{
				get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(i)) = signo * notional;
			}
			get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(numPeriods - 1)) = signo * notional;
		}
		if (amortization == QCInterestRateLeg::qcConstantAmort)
		{
			get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(0)) = signo * notional;
			get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(0)) = signo * notional / numPeriods;
			for (unsigned int i = 1; i < numPeriods; ++i)
			{
				get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(i)) =
					get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(i - 1)) -
					get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(i - 1));
				get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(i)) = signo * notional / numPeriods;
			}
		}
	}
	else
	{
		//Se arma el primer period con la primera linea de amortNotionalByDate usando como startDate
		//la fecha de amortizacion - 1 vez la periodicidad. Settlement date queda como businessDay de endDate
		//y las fechas de pata flotante igual a start date
		numPeriods = amortNotionalByDate.size();
		periods.resize(numPeriods); //En este caso se sabe a priori cuantos flujos son
		QCDate fechaFinalPeriodo = get<0>(amortNotionalByDate.at(0));
		QCDate fechaInicioPeriodo = fechaFinalPeriodo.addMonths(-tenor(periodicity));
		QCDate fechaPagoPeriodo = fechaFinalPeriodo.businessDay(calendar, endDateAdjustment);
		auto temp = make_tuple(-signo * get<2>(amortNotionalByDate.at(0)),
			false, signo * get<1>(amortNotionalByDate.at(0)), true, signo * get<2>(amortNotionalByDate.at(0)),
			fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
			fechaInicioPeriodo, fechaInicioPeriodo, fechaInicioPeriodo);
		periods.at(0) = temp;

		//Loop sobre el resto de los flujos tomando como start date el end date
		//del period anterior y la misma regla para settlement date y fixing dates
		for (unsigned long i = 1; i < periods.size(); ++i)
		{
			fechaInicioPeriodo = get<QCInterestRateLeg::intRtPrdElmntEndDate>(periods.at(i - 1));
			fechaFinalPeriodo = get<0>(amortNotionalByDate.at(i));
			fechaPagoPeriodo = fechaFinalPeriodo.businessDay(calendar, endDateAdjustment);
			auto temp = make_tuple(0, false,
				signo * get<1>(amortNotionalByDate.at(i)), true,
				signo * get<2>(amortNotionalByDate.at(i)),
				fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
				fechaInicioPeriodo, fechaInicioPeriodo, fechaInicioPeriodo);
			periods.at(i) = temp;
		}
	}
	
	QCInterestRateLeg result{ periods, numPeriods - 1 };
	return result;
}

QCInterestRateLeg QCFactoryFunctions::buildIcpLeg(
	string receivePay,				//receive or pay
	QCDate startDate,				//start date
	QCDate endDate,					//end date
	vector<QCDate> calendar,		//settlement calendar
	int settlementLag,				//settlement lag
	QCInterestRateLeg::QCStubPeriod stubPeriod,		//stub period
	string periodicity,				//periodicity
	QCDate::QCBusDayAdjRules endDateAdjustment,		//end date adjustment
	QCInterestRateLeg::QCAmortization amortization,	//amortization
	vector<tuple<QCDate, double, double>> amortNotionalByDate, //amortization and notional by date
	double notional									//notional
	)
{
	//QCInterestRatePeriod es:
	//tuple<double, bool, double, bool, double, QCDate, QCDate, QCDate, QCDate, QCDate, QCDate>
	//intRtPrdElmntInitialAccrtn,	/*!< Disposicion inicial del periodo */
	//intRtPrdElmntAcctrnIsCshflw,	/*!< Indica si la disposicion inicial es flujo */
	//intRtPrdElmntFinalAmrtztn,	/*!< Amortizacion final del periodo */
	//inRtPrdElmntAmrtztnIsCshflw,	/*!< Indica si la amortizacion final es flujo */
	//intRtPrdElmntNotional,		/*!< Nocional vigente del periodo */
	//intRtPrdElmntStartDate,		/*!< Fecha de inicio del periodo */
	//intRtPrdElmntEndDate,			/*!< Fecha final del periodo */
	//intRtPrdElmntSettlmntDate,	/*!< Fecha de pago del flujo */

	//Las siguientes 3 son para patas flotantes
	//intRtPrdElmntFxngDate,		/*!< Fecha de fixing del indice */
	//intRtPrdElmntFxngInitDate,	/*!< Fecha de inicio de devengo del indice */
	//intRtPrdElmntFxngEndDate		/*!< Fecha final de devengo del indice */

	//Aqui se guardaran los periodos
	QCInterestRateLeg::QCInterestRatePeriods periods;

	//Apenas se calcule el numero de periodos se registrara aqui
	unsigned int numPeriods;

	//Sirve para determinar el signo de nocional vigente, disposicion y amortizacion
	int signo;
	if (receivePay == "R") { signo = 1; }
	else { signo = -1; }

	//La distancia en meses y dias entre startDate y endDate
	tuple<unsigned long, int> distanceMonthsDays = startDate.monthDiffDayRemainder(
		endDate, calendar, endDateAdjustment);

	//Cuantos periodos enteros hay entre startDate y endDate y cuantos meses sobran
	if (amortization != QCInterestRateLeg::qcCustomAmort)
	{
		unsigned int numWholePeriods = get<0>(distanceMonthsDays) / QCHelperFunctions::tenor(periodicity);
		unsigned int remainderInMonths = get<0>(distanceMonthsDays) % QCHelperFunctions::tenor(periodicity);
		if (stubPeriod == QCInterestRateLeg::qcShortBack || stubPeriod == QCInterestRateLeg::qcNoStubPeriod)
		{
			/*
			Case "CORTO FINAL"
			periods = numWholePeriods + 1 - Indicador(remainderMonths + dDias, 0)
			ReDim result(1 To periods, 1 To 2) As Date
			result(1, 1) = startDate
			result(1, 2) = boolBusDay(busDay, AddMonths(startDate, Ten(periodicity)))
			For i = 2 To periods - 1
			result(i, 1) = result(i - 1, 2)
			result(i, 2) = boolBusDay(busDay, AddMonths(startDate, i * Ten(periodicity)))
			Next i
			result(periods, 1) = result(periods - 1, 2)
			result(periods, 2) = endDate
			*/
			int indicador = 0;
			if (remainderInMonths + get<1>(distanceMonthsDays) == 0)
				indicador = 1;
			periods.resize(numWholePeriods + 1 - indicador);

			QCDate fechaInicioPeriodo = startDate;
			QCDate fechaFinalPeriodo = startDate.addMonths(tenor(periodicity)).businessDay(
				calendar, endDateAdjustment);
			QCDate fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);

			periods.at(0) = make_tuple(-signo * notional, false, 0, true, 0,
				fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
				fechaInicioPeriodo, fechaInicioPeriodo, fechaFinalPeriodo);

			for (unsigned int i = 1; i < periods.size() - 1; ++i)
			{
				fechaInicioPeriodo = fechaFinalPeriodo;
				fechaFinalPeriodo = startDate.addMonths(tenor(periodicity) * (i + 1)).businessDay(
					calendar, endDateAdjustment);
				fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);
				periods.at(i) = make_tuple(0, false, 0, true, 0,
					fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
					fechaInicioPeriodo, fechaInicioPeriodo, fechaFinalPeriodo);
			}
			fechaInicioPeriodo = fechaFinalPeriodo;
			fechaFinalPeriodo = endDate;
			fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);
			periods.at(periods.size() - 1) = make_tuple(0, false, 0, true, 0,
				fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
				fechaInicioPeriodo, fechaInicioPeriodo, fechaFinalPeriodo);
		}
		if (stubPeriod == QCInterestRateLeg::qcLongBack)
		{
			/*
			Case "LARGO FINAL"
			periods = numWholePeriods
			ReDim result(1 To periods, 1 To 2) As Date
			result(1, 1) = startDate
			result(1, 2) = boolBusDay(busDay, AddMonths(startDate, Ten(periodicity)))
			For i = 2 To periods - 1
			result(i, 1) = result(i - 1, 2)
			result(i, 2) = boolBusDay(busDay, AddMonths(startDate, i * Ten(periodicity)))
			Next i
			result(periods, 1) = result(periods - 1, 2)
			result(periods, 2) = endDate
			*/
			periods.resize(numWholePeriods);

			QCDate fechaInicioPeriodo = startDate;
			QCDate fechaFinalPeriodo = startDate.addMonths(tenor(periodicity)).businessDay(
				calendar, endDateAdjustment);
			QCDate fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);

			periods.at(0) = make_tuple(-signo * notional, false, 0, true, 0,
				fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
				fechaInicioPeriodo, fechaInicioPeriodo, fechaFinalPeriodo);

			for (unsigned int i = 1; i < periods.size() - 1; ++i)
			{
				fechaInicioPeriodo = fechaFinalPeriodo;
				fechaFinalPeriodo = startDate.addMonths(tenor(periodicity) * (i + 1)).businessDay(
					calendar, endDateAdjustment);
				fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);
				periods.at(i) = make_tuple(0, false, 0, true, 0,
					fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
					fechaInicioPeriodo, fechaInicioPeriodo, fechaFinalPeriodo);
			}
			fechaInicioPeriodo = fechaFinalPeriodo;
			fechaFinalPeriodo = endDate;
			fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);
			periods.at(periods.size() - 1) = make_tuple(0, false, 0, true, 0,
				fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
				fechaInicioPeriodo, fechaInicioPeriodo, fechaFinalPeriodo);
		}
		if (stubPeriod == QCInterestRateLeg::qcShortFront)
		{
			/*
			Case "CORTO INICIO"
			periods = numWholePeriods + 1 - Indicador(remainderMonths + dDias, 0)
			ReDim result(1 To periods, 1 To 2) As Date
			result(periods, 1) = BussDay(AddMonths(endDate, -Ten(periodicity)))
			result(periods, 2) = endDate
			For i = periods - 1 To 2 Step -1
			result(i, 1) = boolBusDay(busDay, AddMonths(endDate, (i - periods - 1) * Ten(periodicity)))
			result(i, 2) = result(i + 1, 1)
			Next i
			result(1, 1) = startDate
			result(1, 2) = result(2, 1)
			*/

			int indicador = 0;
			if (remainderInMonths + get<1>(distanceMonthsDays) == 0)
				indicador = 1;
			periods.resize(numWholePeriods + 1 - indicador);

			QCDate fechaFinalPeriodo = endDate;
			QCDate fechaInicioPeriodo = endDate.addMonths(-tenor(periodicity)).businessDay(
				calendar, endDateAdjustment);
			QCDate fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);

			unsigned int numPeriods = periods.size();
			periods.at(numPeriods - 1) = make_tuple(0, false, 0, true, 0,
				fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
				fechaInicioPeriodo, fechaInicioPeriodo, fechaFinalPeriodo);

			if (numPeriods >= 2)
			{
				if (numPeriods > 2)
				{
					for (unsigned int i = numPeriods - 2; i > 0; --i)
					{
						fechaFinalPeriodo = fechaInicioPeriodo;
						fechaInicioPeriodo = endDate.addMonths((i - numPeriods) * tenor(periodicity)).businessDay(
							calendar, endDateAdjustment);
						fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);
						periods.at(i) = make_tuple(0, false, 0, true, 0,
							fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
							fechaInicioPeriodo, fechaInicioPeriodo, fechaFinalPeriodo);
					}
				}

				fechaFinalPeriodo = fechaInicioPeriodo;
				fechaInicioPeriodo = startDate;
				fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);
				periods.at(0) = make_tuple(0, false, 0, true, 0,
					fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
					fechaInicioPeriodo, fechaInicioPeriodo, fechaFinalPeriodo);
			}
		}
		if (stubPeriod == QCInterestRateLeg::qcLongFront)
		{
			/*
			Case "LARGO INICIO"
			periods = numWholePeriods
			ReDim result(1 To periods, 1 To 2) As Date
			result(periods, 1) = BussDay(AddMonths(endDate, -Ten(periodicity)))
			result(periods, 2) = endDate
			For i = periods - 1 To 2 Step -1
			result(i, 1) = boolBusDay(busDay, AddMonths(endDate, (i - periods - 1) * Ten(periodicity)))
			result(i, 2) = result(i + 1, 1)
			Next i
			result(1, 1) = startDate
			result(1, 2) = result(2, 1)
			*/

			periods.resize(numWholePeriods);

			QCDate fechaFinalPeriodo = endDate;
			QCDate fechaInicioPeriodo = endDate.addMonths(-tenor(periodicity)).businessDay(
				calendar, endDateAdjustment);
			QCDate fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);

			unsigned int numPeriods = periods.size();
			periods.at(numPeriods - 1) = make_tuple(0, false, 0, true, 0,
				fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
				fechaInicioPeriodo, fechaInicioPeriodo, fechaFinalPeriodo);

			if (numPeriods >= 2)
			{
				if (numPeriods > 2)
				{
					for (unsigned int i = numPeriods - 2; i > 0; --i)
					{
						fechaFinalPeriodo = fechaInicioPeriodo;
						fechaInicioPeriodo = endDate.addMonths((i - numPeriods) * tenor(periodicity)).businessDay(
							calendar, endDateAdjustment);
						fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);
						periods.at(i) = make_tuple(0, false, 0, true, 0,
							fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
							fechaInicioPeriodo, fechaInicioPeriodo, fechaFinalPeriodo);
					}
				}

				fechaFinalPeriodo = fechaInicioPeriodo;
				fechaInicioPeriodo = startDate;
				fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);

				periods.at(0) = make_tuple(0, false, 0, true, 0,
					fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
					fechaInicioPeriodo, fechaInicioPeriodo, fechaFinalPeriodo);

			}
		}
		//Calcular amortizaciones y nocionales vigentes
		numPeriods = periods.size();
		if (amortization == QCInterestRateLeg::qcBulletAmort)
		{
			for (unsigned int i = 0; i < numPeriods; ++i)
			{
				get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(i)) = signo * notional;
			}
			get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(numPeriods - 1)) = signo * notional;
		}
		if (amortization == QCInterestRateLeg::qcConstantAmort)
		{
			get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(0)) = signo * notional;
			get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(0)) = signo * notional / numPeriods;
			for (unsigned int i = 1; i < numPeriods; ++i)
			{
				get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(i)) =
					get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(i - 1)) -
					get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(i - 1));
				get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(i)) = signo * notional / numPeriods;
			}
		}
	}
	else
	{
		//Se arma el primer period con la primera linea de amortNotionalByDate usando como startDate
		//la fecha de amortizacion - 1 vez la periodicidad. Settlement date queda como businessDay de endDate
		//y las fechas de pata flotante igual a start date
		numPeriods = amortNotionalByDate.size();
		periods.resize(numPeriods); //En este caso se sabe a priori cuantos flujos son
		QCDate fechaFinalPeriodo = get<0>(amortNotionalByDate.at(0));
		QCDate fechaInicioPeriodo = fechaFinalPeriodo.addMonths(-tenor(periodicity));
		QCDate fechaPagoPeriodo = fechaFinalPeriodo.businessDay(calendar, endDateAdjustment);
		auto temp = make_tuple(-signo * get<2>(amortNotionalByDate.at(0)),
			false, signo * get<1>(amortNotionalByDate.at(0)), true, signo * get<2>(amortNotionalByDate.at(0)),
			fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
			fechaInicioPeriodo, fechaInicioPeriodo, fechaFinalPeriodo);
		periods.at(0) = temp;

		//Loop sobre el resto de los flujos tomando como start date el end date
		//del period anterior y la misma regla para settlement date y fixing dates
		for (unsigned long i = 1; i < periods.size(); ++i)
		{
			fechaInicioPeriodo = get<QCInterestRateLeg::intRtPrdElmntEndDate>(periods.at(i - 1));
			fechaFinalPeriodo = get<0>(amortNotionalByDate.at(i));
			fechaPagoPeriodo = fechaFinalPeriodo.businessDay(calendar, endDateAdjustment);
			auto temp = make_tuple(0, false,
				signo * get<1>(amortNotionalByDate.at(i)), true,
				signo * get<2>(amortNotionalByDate.at(i)),
				fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
				fechaInicioPeriodo, fechaInicioPeriodo, fechaFinalPeriodo);
			periods.at(i) = temp;
		}
	}

	QCInterestRateLeg result{ periods, numPeriods - 1 };

	return result;
}

QCInterestRateLeg buildFloatingRateLeg(
	string receivePay,					//receive or pay
	QCDate startDate,					//start date
	QCDate endDate,						//end date
	vector<QCDate> settleCalendar,		//settlement calendar
	int settlementLag,					//settlement lag
	string stubPeriod,					//stub period
	string settlePeriodicity,			//settlement periodicity
	QCDate::QCBusDayAdjRules endDateAdjustment, //end date adjustment
	string amortization,						//amortization
	vector<tuple<QCDate, double, double>> amortNotionalByDate,	//amortization and notional by end date
	string fixingStubPeriod,					//fixing stub period
	string fixingPeriodicity,					//fixing periodicity
	vector<QCDate> fixingCalendar,				//fixing calendar
	pair<string, string> interestRateIndexChars,	//interest rate index tenor (3M, 6M ...)	
	double notional								//notional
	)
{
	//QCInterestRatePeriod es:
	//tuple<double, bool, double, bool, double, QCDate, QCDate, QCDate, QCDate, QCDate, QCDate>
	//intRtPrdElmntInitialAccrtn,	/*!< Disposicion inicial del periodo */
	//intRtPrdElmntAcctrnIsCshflw,	/*!< Indica si la disposicion inicial es flujo */
	//intRtPrdElmntFinalAmrtztn,	/*!< Amortizacion final del periodo */
	//inRtPrdElmntAmrtztnIsCshflw,	/*!< Indica si la amortizacion final es flujo */
	//intRtPrdElmntNotional,		/*!< Nocional vigente del periodo */
	//intRtPrdElmntStartDate,		/*!< Fecha de inicio del periodo */
	//intRtPrdElmntEndDate,			/*!< Fecha final del periodo */
	//intRtPrdElmntSettlmntDate,	/*!< Fecha de pago del flujo */

	//Las siguientes 3 son para patas flotantes
	//intRtPrdElmntFxngDate,		/*!< Fecha de fixing del indice */
	//intRtPrdElmntFxngInitDate,	/*!< Fecha de inicio de devengo del indice */
	//intRtPrdElmntFxngEndDate		/*!< Fecha final de devengo del indice */

	QCInterestRateLeg::QCInterestRatePeriods periods;
	unsigned int numPeriods;
	int signo;
	if (receivePay == "R") { signo = 1; }
	else { signo = -1; }
	if (amortization != "CUSTOM")
	{
		if (stubPeriod == "CORTO FINAL")
		{
			QCDate fechaInicioPeriodo = startDate;
			QCDate fechaFinalPeriodo = startDate;
			QCDate fechaPagoPeriodo;
			double disposicionInicial = -signo * notional;
			int counter = 1;
			while (true)
			{
				fechaFinalPeriodo = startDate.addMonths(tenor(periodicity) * counter).businessDay(
					calendar, endDateAdjustment);
				fechaPagoPeriodo = fechaFinalPeriodo.shift(calendar, settlementLag);
				if (fechaFinalPeriodo < endDate)
				{
					//Meter las fechas a la tupla del periodo. Amortizacion y nocional en 0 hasta
					//saber cuantos periodos hay.
					auto temp = make_tuple(disposicionInicial, false, 0, true, 0,
						fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
						fechaInicioPeriodo, fechaInicioPeriodo, fechaFinalPeriodo);
					periods.push_back(temp);

					//Definir el startDate del proximo periodo
					fechaInicioPeriodo = fechaFinalPeriodo;
					disposicionInicial = 0.0;
					++counter;
				}
				else
				{
					//Meter datos a la tupla del periodo. Como ya superamos la endDate
					//ponemos endDate como ultima fecha final de periodo
					auto temp = make_tuple(disposicionInicial, false, 0, true, 0,
						fechaInicioPeriodo, endDate, fechaPagoPeriodo,
						fechaInicioPeriodo, fechaInicioPeriodo, fechaFinalPeriodo);
					periods.push_back(temp);
					break;
				}
			}
		}
		//Calcular amortizaciones y nocionales vigentes
		numPeriods = periods.size();
		if (amortization == "BULLET")
		{
			for (unsigned int i = 0; i < numPeriods; ++i)
			{
				get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(i)) = signo * notional;
			}
			get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(numPeriods - 1)) = signo * notional;
		}
		if (amortization == "CONSTANT")
		{
			get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(0)) = signo * notional;
			get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(0)) = signo * notional / numPeriods;
			for (unsigned int i = 1; i < numPeriods; ++i)
			{
				get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(i)) =
					get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(i - 1)) -
					get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(i - 1));
				get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(i)) = signo * notional / numPeriods;
			}
		}
	}
	else
	{
		//Se arma el primer period con la primera linea de amortNotionalByDate usando como startDate
		//la fecha de amortizacion - 1 vez la periodicidad. Settlement date queda como businessDay de endDate
		//y las fechas de pata flotante igual a start date
		numPeriods = amortNotionalByDate.size();
		periods.resize(numPeriods); //En este caso se sabe a priori cuantos flujos son
		QCDate fechaFinalPeriodo = get<0>(amortNotionalByDate.at(0));
		QCDate fechaInicioPeriodo = fechaFinalPeriodo.addMonths(-tenor(periodicity));
		QCDate fechaPagoPeriodo = fechaFinalPeriodo.businessDay(calendar, endDateAdjustment);
		auto temp = make_tuple(-signo * get<2>(amortNotionalByDate.at(0)),
			false, signo * get<1>(amortNotionalByDate.at(0)), true, signo * get<2>(amortNotionalByDate.at(0)),
			fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
			fechaInicioPeriodo, fechaInicioPeriodo, fechaFinalPeriodo);
		periods.at(0) = temp;

		//Loop sobre el resto de los flujos tomando como start date el end date
		//del period anterior y la misma regla para settlement date y fixing dates
		for (unsigned long i = 1; i < periods.size(); ++i)
		{
			fechaInicioPeriodo = get<QCInterestRateLeg::intRtPrdElmntEndDate>(periods.at(i - 1));
			fechaFinalPeriodo = get<0>(amortNotionalByDate.at(i));
			fechaPagoPeriodo = fechaFinalPeriodo.businessDay(calendar, endDateAdjustment);
			auto temp = make_tuple(0, false,
				signo * get<1>(amortNotionalByDate.at(i)), true,
				signo * get<2>(amortNotionalByDate.at(i)),
				fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
				fechaInicioPeriodo, fechaInicioPeriodo, fechaFinalPeriodo);
			periods.at(i) = temp;
		}
	}

	QCInterestRateLeg result{ periods, numPeriods - 1 };

	return result;
}
