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
	string stubPeriod,			//stub period
	string periodicity,			//periodicity
	QCDate::QCBusDayAdjRules endDateAdjustment,	//end date adjustment
	string amortization,		//amortization
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

	QCInterestRateLeg::QCInterestRatePeriods periods;
	unsigned int numPeriods;
	int signo;
	if (receivePay == "R") { signo = 1;} else { signo = -1; }
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
						fechaInicioPeriodo, fechaInicioPeriodo, fechaInicioPeriodo);
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
						fechaInicioPeriodo, fechaInicioPeriodo, fechaInicioPeriodo);
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
		/*
		//Recorrer los periods de a uno y
		for (unsigned int i = 0; i < numPeriods; ++i)
		{
			//buscar la end date (needle) en las custom amort
			auto needle = get<QCInterestRateLeg::intRtPrdElmntEndDate>(periods.at(i));
			auto it = find_if(amortNotionalByDate.begin(), amortNotionalByDate.end(),
				[&](const tuple<QCDate, double, double>& e)
			{
				return get<0>(e) == needle;
			});

			//Si se encuentra se reemplaza el nocional y la amortizacion por los valores del vector con
			//el signo que corresponde segun el tipo de pata (usar la variable signo)
			if (it != amortNotionalByDate.end())
			{
				get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(
					periods.at(i)) = signo * get<1>(*it);
				get<QCInterestRateLeg::intRtPrdElmntNotional>(
					periods.at(i)) = signo * get<2>(*it);
			}
			//Si no se encuentra se deja todo igual	
		}
		*/
	}
	
	QCInterestRateLeg result{ periods, numPeriods - 1 };
	//auto x = get<QCInterestRateLeg::intRtPrdElmntStartDate>(periods.at(0));
	//auto y = get<QCInterestRateLeg::intRtPrdElmntEndDate>(periods.at(0));
	return result;
}

QCInterestRateLeg QCFactoryFunctions::buildIcpClpLeg(
	string receivePay,				//receive or pay
	QCDate startDate,				//start date
	QCDate endDate,					//end date
	vector<QCDate> calendar,		//settlement calendar
	int settlementLag,				//settlement lag
	string stubPeriod,				//stub period
	string periodicity,				//periodicity
	QCDate::QCBusDayAdjRules endDateAdjustment,		//end date adjustment
	string amortization,							//amortization
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
						fechaInicioPeriodo, fechaInicioPeriodo, fechaInicioPeriodo);
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
						fechaInicioPeriodo, fechaFinalPeriodo, fechaFinalPeriodo);
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
			fechaInicioPeriodo, fechaFinalPeriodo, fechaFinalPeriodo);
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
				fechaInicioPeriodo, fechaFinalPeriodo, fechaFinalPeriodo);
			periods.at(i) = temp;
		}
	}

	QCInterestRateLeg result{ periods, numPeriods - 1 };

	return result;
}