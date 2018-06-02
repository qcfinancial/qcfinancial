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
#include "QCInterestRatePeriodsFactory.h"

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

shared_ptr<QCInterestRate> QCFactoryFunctions::zeroIntRateSharedPtr()
{
	//Constructor de QCInterestRate
	//QCInterestRate(double value, QCYrFrctnShrdPtr yearFraction, QCWlthFctrShrdPtr wealthFactor)

	return make_shared<QCInterestRate>(QCInterestRate{ 0.0,
		QCFactoryFunctions::yfSharedPtr("act/360"),
		QCFactoryFunctions::wfSharedPtr("lin") });
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

//El parámetro typeCurve aún no está implementado
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

shared_ptr<QCInterestRateCurve> QCFactoryFunctions::intRtCrvShrdPtr(vector<long>& tenors,
	vector<double>& rates,
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
	if (typeCurve == QCInterestRateCurve::qcProjectingCurve)
	{
		auto result = make_shared<QCProjectingInterestRateCurve>(
			QCProjectingInterestRateCurve{ interpol, intRate });
		return result;
	}

	if (typeCurve == QCInterestRateCurve::qcZeroCouponCurve)
	{
		auto result = make_shared<QCZeroCouponInterestRateCurve>(
			QCZeroCouponInterestRateCurve{ interpol, intRate });
		return result;
	}
	
	if (typeCurve == QCInterestRateCurve::qcDiscountFactorCurve)
	{
		auto result = make_shared<QCZeroCouponDiscountFactorCurve>(
			QCZeroCouponDiscountFactorCurve{ interpol, intRate });
		return result;
	}
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

QCInterestRateLeg QCFactoryFunctions::buildDiscountBondLeg(
	string receivePay,				/*!< receive or pay */
	QCDate startDate,				/*!< start date */
	QCDate endDate,					/*!< end date */
	unsigned int fixingLag,			/*!< fixing lag refers to fx rate */
	unsigned int settlementLag,				/*!< settlement lag */
	vector<QCDate> fixingCalendar,	/*!< fixing calendar refers to fx rate */
	vector<QCDate> settlementCalendar,	/*!< settlement calendar */
	double notional					/*!< notional */
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

	//Sirve para determinar el signo de nocional vigente, disposicion y amortizacion
	int signo;
	if (receivePay == "R") { signo = 1; }
	else { signo = -1; }

	auto period = make_tuple(0.0, false, signo * notional, true, signo * notional, startDate, endDate,
		endDate.shift(settlementCalendar, settlementLag, QCDate::qcFollow),
		endDate.shift(fixingCalendar, fixingLag, QCDate::qcPrev), startDate, startDate);
	QCInterestRateLeg::QCInterestRatePeriods periods{ period };
	return QCInterestRateLeg{ periods, 0 };

}

QCInterestRateLeg QCFactoryFunctions::buildTimeDepositLeg(
	string receivePay,				/*!< receive or pay */
	QCDate startDate,				/*!< start date */
	QCDate endDate,					/*!< end date */
	double notional					/*!< notional */
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
	//Sirve para determinar el signo de nocional vigente, disposicion y amortizacion

	int signo;
	if (receivePay == "R") { signo = 1; }
	else { signo = -1; }

	auto period = make_tuple(-signo * notional, true, signo * notional, true, signo * notional, startDate, endDate,
		endDate, startDate, startDate, startDate);
	QCInterestRateLeg::QCInterestRatePeriods periods{ period };
	return QCInterestRateLeg{ periods, 0 };
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
	size_t numPeriods;
	
	//Sirve para determinar el signo de nocional vigente, disposicion y amortizacion
	int signo;
	if (receivePay == "R") { signo = 1;} else { signo = -1; }
	
	//La distancia en meses y dias entre startDate y endDate
	tuple<unsigned long, int> distanceMonthsDays = startDate.monthDiffDayRemainder(
		endDate, calendar, endDateAdjustment);

	//Cuantos periodos enteros hay entre startDate y endDate y cuantos meses sobran
	if (amortization != QCInterestRateLeg::qcCustomAmort)
	{
		if (amortization != QCInterestRateLeg::qcCustomAmort)
		{
			//Se da de alta la fabrica de periods
			QCInterestRatePeriodsFactory factory{ startDate, endDate,
				endDateAdjustment,
				periodicity,
				stubPeriod,
				make_shared<vector<QCDate>>(calendar),
				(unsigned int)settlementLag,
				periodicity,
				stubPeriod,
				make_shared<vector<QCDate>>(calendar),
				0,
				0,
				periodicity };

			//Se generan los periodos
			periods = factory.getPeriods();
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

QCInterestRateLeg QCFactoryFunctions::buildFixedRateLeg2(
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
	//Aqui se guardaran los periodos
	QCInterestRateLeg::QCInterestRatePeriods periods;

	//Apenas se calcule el numero de periodos se registrara aqui
	size_t numPeriods;

	//Sirve para determinar el signo de nocional vigente, disposicion y amortizacion
	int signo;
	if (receivePay == "R") { signo = 1; }
	else { signo = -1; }

	//Se da de alta la fabrica de periods
	QCInterestRatePeriodsFactory factory{ startDate, endDate,
		endDateAdjustment,
		periodicity,
		stubPeriod,
		make_shared<vector<QCDate>>(calendar),
		(unsigned int)settlementLag,
		periodicity,
		stubPeriod,
		make_shared<vector<QCDate>>(calendar),
		0,
		0,
		periodicity };

	//Se generan los periodos
	periods = factory.getPeriods();

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
	if (amortization == QCInterestRateLeg::qcConstantAmort || amortization == QCInterestRateLeg::qcCustomAmort)
	{
		size_t numAmort = amortNotionalByDate.size();
		for (unsigned int i = 0; i < numAmort; ++i)
		{
			get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(numPeriods - 1- i)) =
				signo * get<2>(amortNotionalByDate.at(numAmort - 1 - i));
			get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(numPeriods - 1 - i)) =
				signo * get<1>(amortNotionalByDate.at(numAmort - 1 - i));
		}
	}
	
	/*else
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
	}*/

	QCInterestRateLeg result{ periods, numPeriods - 1 };
	return result;
}

QCInterestRateLeg QCFactoryFunctions::buildFrenchFixedRateLeg(
	QCDate valueDate,			//value date
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
	double notional,			//notional
	shared_ptr<QCInterestRate> intRate //interest rate
	)
{
	if (amortization != QCInterestRateLeg::qcFrenchAmort)
	{
		throw invalid_argument("La operacion debe tener amortizacion francesa.");
	}
	//Aqui se guardaran los periodos
	QCInterestRateLeg::QCInterestRatePeriods periods;

	//Sirve para determinar el signo de nocional vigente, disposicion y amortizacion
	int signo;
	if (receivePay == "R") { signo = 1; }
	else { signo = -1; }

	//Se da de alta la fabrica de periods
	QCInterestRatePeriodsFactory factory{ startDate, endDate,
		endDateAdjustment,
		periodicity,
		stubPeriod,
		make_shared<vector<QCDate>>(calendar),
		(unsigned int)settlementLag,
		periodicity,
		stubPeriod,
		make_shared<vector<QCDate>>(calendar),
		0,
		0,
		periodicity };

	//Se generan los periodos
	periods = factory.getPeriods();

	size_t numPeriods = periods.size();
	size_t currentPeriod = 0;
	
	//Determinar el current period
	for (size_t i = 0; i < numPeriods; ++i)
	{
		if (valueDate >= get<QCInterestRateLeg::intRtPrdElmntStartDate>(periods.at(i)) &&
			valueDate < get<QCInterestRateLeg::intRtPrdElmntEndDate>(periods.at(i)))
		{
			currentPeriod = i;
			break;
		}
	}

	//Con esas fechas residuales y el notional calcular la cuota
	auto periodsCuota = numPeriods - currentPeriod;
	double r = 1 / (1 + intRate->getValue() / 12.0);
	double formula = r * (1 - pow(r, periodsCuota)) / (1 - r);
	double cuota = notional / formula;

	/*cout << "numPeriods: " << numPeriods << " curr: " << currentPeriod <<
	" cuota: " << cuota << " nocional: " << notional << endl;
	*/

	//Con la cuota y el nocional calcular el nocional vigente y las amortizaciones
	double tempNotional = notional;
	r = intRate->getValue() / 12.0;
	for (size_t i = currentPeriod; i < numPeriods; ++i)
	{
		get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(i)) = tempNotional;
		double amort = cuota - tempNotional * r;
		//cout << "amort: " << amort << endl;
		get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(i)) = amort;
		tempNotional -= amort;
	}
	QCInterestRateLeg result{ periods, numPeriods - 1 };
	return result;
}

QCInterestRateLeg QCFactoryFunctions::buildFixedRateLeg3(
	QCDate valueDate,			//value date
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
	double notional,			//notional
	shared_ptr<QCInterestRate> intRate //interest rate
	)
{
	//Aqui se guardaran los periodos
	QCInterestRateLeg::QCInterestRatePeriods periods;

	//Sirve para determinar el signo de nocional vigente, disposicion y amortizacion
	int signo;
	if (receivePay == "R") { signo = 1; }
	else { signo = -1; }

	//Se da de alta la fabrica de periods
	QCInterestRatePeriodsFactory factory{ startDate, endDate,
		endDateAdjustment,
		periodicity,
		stubPeriod,
		make_shared<vector<QCDate>>(calendar),
		(unsigned int)settlementLag,
		periodicity,
		stubPeriod,
		make_shared<vector<QCDate>>(calendar),
		0,
		0,
		periodicity };

	//Se generan los periodos
	periods = factory.getPeriods();

	size_t numPeriods = periods.size();
	size_t currentPeriod = 0;

	//Determinar el current period
	for (size_t i = 0; i < numPeriods; ++i)
	{
		if (valueDate >= get<QCInterestRateLeg::intRtPrdElmntStartDate>(periods.at(i)) &&
			valueDate < get<QCInterestRateLeg::intRtPrdElmntEndDate>(periods.at(i)))
		{
			currentPeriod = i;
			break;
		}
	}

	if (amortization == QCInterestRateLeg::qcFrenchAmort)
	{
		//Con esas fechas residuales y el notional calcular la cuota
		auto periodsCuota = numPeriods - currentPeriod;
		double r = 1 / (1 + intRate->getValue() / 12.0);
		double formula = r * (1 - pow(r, periodsCuota)) / (1 - r);
		double cuota = notional / formula;

		//Con la cuota y el nocional calcular el nocional vigente y las amortizaciones
		double tempNotional = notional;
		r = intRate->getValue() / 12.0;
		for (size_t i = currentPeriod; i < numPeriods; ++i)
		{
			get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(i)) = tempNotional;
			double amort = cuota - tempNotional * r;
			get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(i)) = amort;
			tempNotional -= amort;
		}
	}

	if (amortization == QCInterestRateLeg::qcBulletAmort)
	{
		for (size_t i = currentPeriod; i < numPeriods; ++i)
		{
			get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(i)) = signo * notional;
		}
		get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(numPeriods - 1)) = signo * notional;
	}

	if (amortization == QCInterestRateLeg::qcConstantAmort)
	{
		double amort = signo * notional / (numPeriods - currentPeriod);
		double tempNotional = signo * notional;
		for (size_t i = currentPeriod; i < numPeriods; ++i)
		{
			get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(i)) = tempNotional;
			get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(i)) = amort;
			tempNotional -= amort;
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
	size_t numPeriods;

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
		if (amortization != QCInterestRateLeg::qcCustomAmort)
		{
			//Se da de alta la fabrica de periods
			QCInterestRatePeriodsFactory factory{ startDate, endDate,
				endDateAdjustment,
				periodicity,
				stubPeriod,
				make_shared<vector<QCDate>>(calendar),
				(unsigned int)settlementLag,
				periodicity,
				stubPeriod,
				make_shared<vector<QCDate>>(calendar),
				0,
				0,
				periodicity };

			//Se generan los periodos
			periods = factory.getPeriods();
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

QCInterestRateLeg QCFactoryFunctions::buildIcpLeg2(
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
	//Aqui se guardaran los periodos
	QCInterestRateLeg::QCInterestRatePeriods periods;

	//Apenas se calcule el numero de periodos se registrara aqui
	size_t numPeriods;

	//Sirve para determinar el signo de nocional vigente, disposicion y amortizacion
	int signo;
	if (receivePay == "R") { signo = 1; }
	else { signo = -1; }

	//Se da de alta la fabrica de periods
	QCInterestRatePeriodsFactory factory{ startDate, endDate,
		endDateAdjustment,
		periodicity,
		stubPeriod,
		make_shared<vector<QCDate>>(calendar),
		(unsigned int)settlementLag,
		periodicity,
		stubPeriod,
		make_shared<vector<QCDate>>(calendar),
		0,
		0,
		periodicity };

	//Se generan los periodos
	periods = factory.getPeriods();
	numPeriods = periods.size();

	//Se ajustan las fechas de inicio y final de índice de fixing y se dejan igual que
	//startDate y endDate de cada período.
	for (unsigned int i = 0; i < numPeriods; ++i)
	{
		get<QCInterestRateLeg::intRtPrdElmntFxngInitDate>(periods.at(i)) =
			get<QCInterestRateLeg::intRtPrdElmntStartDate>(periods.at(i));
		get<QCInterestRateLeg::intRtPrdElmntFxngEndDate>(periods.at(i)) =
			get<QCInterestRateLeg::intRtPrdElmntEndDate>(periods.at(i));
	}

	//Calcular amortizaciones y nocionales vigentes
	if (amortization == QCInterestRateLeg::qcBulletAmort)
	{
		for (unsigned int i = 0; i < numPeriods; ++i)
		{
			get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(i)) = signo * notional;
		}
		get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(numPeriods - 1)) = signo * notional;
	}
	if (amortization == QCInterestRateLeg::qcConstantAmort || amortization == QCInterestRateLeg::qcCustomAmort)
	{
		auto numAmort = amortNotionalByDate.size();
		for (unsigned int i = 0; i < numAmort; ++i)
		{
			get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(numPeriods - 1 - i)) =
				signo * get<2>(amortNotionalByDate.at(numAmort - 1 - i));
			get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(numPeriods - 1 - i)) =
				signo * get<1>(amortNotionalByDate.at(numAmort - 1 - i));
		}
	}
	
	/*else
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
	}*/

	QCInterestRateLeg result{ periods, numPeriods - 1 };
	return result;
}

QCInterestRateLeg QCFactoryFunctions::buildIcpLeg3(
	QCDate valueDate,
	string receivePay,				//receive or pay
	QCDate startDate,				//start date
	QCDate endDate,					//end date
	vector<QCDate> calendar,		//settlement calendar
	int settlementLag,				//settlement lag
	QCInterestRateLeg::QCStubPeriod stubPeriod,		//stub period
	string periodicity,				//periodicity
	QCDate::QCBusDayAdjRules endDateAdjustment,		//end date adjustment
	QCInterestRateLeg::QCAmortization amortization,	//amortization
	double notional									//notional
	)

{
	//Aqui se guardaran los periodos
	QCInterestRateLeg::QCInterestRatePeriods periods;

	//Apenas se calcule el numero de periodos se registrara aqui
	size_t numPeriods;

	//Sirve para determinar el signo de nocional vigente, disposicion y amortizacion
	int signo;
	if (receivePay == "R") { signo = 1; }
	else { signo = -1; }

	//Se da de alta la fabrica de periods
	QCInterestRatePeriodsFactory factory{ startDate, endDate,
		endDateAdjustment,
		periodicity,
		stubPeriod,
		make_shared<vector<QCDate>>(calendar),
		(unsigned int)settlementLag,
		periodicity,
		stubPeriod,
		make_shared<vector<QCDate>>(calendar),
		0,
		0,
		periodicity };

	//Se generan los periodos
	periods = factory.getPeriods();

	numPeriods = periods.size();
	size_t currentPeriod = 0;

	//Determinar el current period
	for (long i = 0; i < numPeriods; ++i)
	{
		if (valueDate >= get<QCInterestRateLeg::intRtPrdElmntStartDate>(periods.at(i)) &&
			valueDate < get<QCInterestRateLeg::intRtPrdElmntEndDate>(periods.at(i)))
		{
			currentPeriod = i;
			break;
		}
	}

	if (amortization == QCInterestRateLeg::qcBulletAmort)
	{
		for (size_t i = currentPeriod; i < numPeriods; ++i)
		{
			get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(i)) = signo * notional;
		}
		get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(numPeriods - 1)) = signo * notional;
	}

	if (amortization == QCInterestRateLeg::qcConstantAmort)
	{
		double amort = signo * notional / (numPeriods - currentPeriod);
		double tempNotional = signo * notional;
		for (size_t i = currentPeriod; i < numPeriods; ++i)
		{
			get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(i)) = tempNotional;
			get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(i)) = amort;
			tempNotional -= amort;
		}
	}

	QCInterestRateLeg result{ periods, numPeriods - 1 };
	return result;
}

QCInterestRateLeg QCFactoryFunctions::buildFloatingRateLeg(
	string receivePay,					//receive or pay
	QCDate startDate,					//start date
	QCDate endDate,						//end date
	vector<QCDate> settleCalendar,		//settlement calendar
	int settlementLag,					//settlement lag
	QCInterestRateLeg::QCStubPeriod stubPeriod,					//stub period
	string settlePeriodicity,			//settlement periodicity
	QCDate::QCBusDayAdjRules endDateAdjustment, //end date adjustment
	QCInterestRateLeg::QCAmortization amortization,				//amortization
	vector<tuple<QCDate, double, double>> amortNotionalByDate,	//amortization and notional by end date
	int fixingLag,								//fixing lag
	QCInterestRateLeg::QCStubPeriod fixingStubPeriod,			//fixing stub period
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
	size_t numPeriods;
	int signo;
	if (receivePay == "R") { signo = 1; }
	else { signo = -1; }

	if (amortization != QCInterestRateLeg::qcCustomAmort)
	{
		//Se da de alta la fabrica de periods
		QCInterestRatePeriodsFactory factory{ startDate, endDate,
			endDateAdjustment,
			settlePeriodicity,
			stubPeriod,
			make_shared<vector<QCDate>>(settleCalendar),
			(unsigned int)settlementLag,
			fixingPeriodicity,
			fixingStubPeriod,
			make_shared<vector<QCDate>>(fixingCalendar),
			(unsigned int)fixingLag,
			(unsigned int)QCHelperFunctions::lagToInt(interestRateIndexChars.second),
			interestRateIndexChars.first };

		//Se generan los periodos
		periods = factory.getPeriods();

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
		QCDate fechaInicioPeriodo = fechaFinalPeriodo.addMonths(-tenor(settlePeriodicity))
			.businessDay(fixingCalendar, QCDate::qcPrev);
		QCDate fechaPagoPeriodo = fechaFinalPeriodo.businessDay(settleCalendar, endDateAdjustment);
		QCDate fechaFixing = fechaInicioPeriodo.shift(fixingCalendar, (unsigned int)fixingLag,
			QCDate::qcPrev);
		QCDate fechaStartFixing = fechaFixing.shift(fixingCalendar, stoi(interestRateIndexChars.first),
			QCDate::qcFollow);
		QCDate fechaEndFixing = fechaStartFixing.addMonths(QCHelperFunctions::tenor(
			interestRateIndexChars.first)).businessDay(fixingCalendar, QCDate::qcFollow);
		auto temp = make_tuple(-signo * get<2>(amortNotionalByDate.at(0)),
			false, signo * get<1>(amortNotionalByDate.at(0)), true, signo * get<2>(amortNotionalByDate.at(0)),
			fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
			fechaFixing, fechaStartFixing, fechaEndFixing);
		periods.at(0) = temp;

		//Loop sobre el resto de los flujos tomando como start date el end date
		//del period anterior y la misma regla para settlement date y fixing dates
		for (unsigned long i = 1; i < periods.size(); ++i)
		{
			fechaInicioPeriodo = get<QCInterestRateLeg::intRtPrdElmntEndDate>(periods.at(i - 1));
			fechaFinalPeriodo = get<0>(amortNotionalByDate.at(i));
			fechaPagoPeriodo = fechaFinalPeriodo.businessDay(settleCalendar, endDateAdjustment);
			fechaFixing = fechaInicioPeriodo.shift(fixingCalendar, (unsigned int)fixingLag,
				QCDate::qcPrev);
			fechaStartFixing = fechaFixing.shift(fixingCalendar, stoi(interestRateIndexChars.first),
				QCDate::qcFollow);
			fechaEndFixing = fechaStartFixing.addMonths(QCHelperFunctions::tenor(
				interestRateIndexChars.first)).businessDay(fixingCalendar, QCDate::qcFollow);

			auto temp = make_tuple(0, false,
				signo * get<1>(amortNotionalByDate.at(i)), true,
				signo * get<2>(amortNotionalByDate.at(i)),
				fechaInicioPeriodo, fechaFinalPeriodo, fechaPagoPeriodo,
				fechaFixing, fechaStartFixing, fechaEndFixing);
			periods.at(i) = temp;
		}
	}

	QCInterestRateLeg result{ periods, numPeriods - 1 };

	return result;
}

QCInterestRateLeg QCFactoryFunctions::buildFloatingRateLeg2(
	string receivePay,					//receive or pay
	QCDate startDate,					//start date
	QCDate endDate,						//end date
	vector<QCDate> settleCalendar,		//settlement calendar
	int settlementLag,					//settlement lag
	QCInterestRateLeg::QCStubPeriod stubPeriod,					//stub period
	string settlePeriodicity,			//settlement periodicity
	QCDate::QCBusDayAdjRules endDateAdjustment, //end date adjustment
	QCInterestRateLeg::QCAmortization amortization,				//amortization
	vector<tuple<QCDate, double, double>> amortNotionalByDate,	//amortization and notional by end date
	int fixingLag,								//fixing lag
	QCInterestRateLeg::QCStubPeriod fixingStubPeriod,			//fixing stub period
	string fixingPeriodicity,					//fixing periodicity
	vector<QCDate> fixingCalendar,				//fixing calendar
	pair<string, string> interestRateIndexChars,	//interest rate index tenor (3M, 6M ...) and fixing lag	
	double notional								//notional
	)
{
	QCInterestRateLeg::QCInterestRatePeriods periods;
	size_t numPeriods;
	int signo;
 	if (receivePay == "R") { signo = 1; }
	else { signo = -1; }

	//Se da de alta la fabrica de periods
	QCInterestRatePeriodsFactory factory{ startDate, endDate,
		endDateAdjustment,
		settlePeriodicity,
		stubPeriod,
		make_shared<vector<QCDate>>(settleCalendar),
		(unsigned int)settlementLag,
		fixingPeriodicity,
		fixingStubPeriod,
		make_shared<vector<QCDate>>(fixingCalendar),
		(unsigned int)fixingLag,
		(unsigned int)QCHelperFunctions::lagToInt(interestRateIndexChars.second),
		interestRateIndexChars.first };

	//Se generan los periodos
	periods = factory.getPeriods();

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
	if (amortization == QCInterestRateLeg::qcConstantAmort || amortization == QCInterestRateLeg::qcCustomAmort)
	{
		auto numAmort = amortNotionalByDate.size();
		for (unsigned int i = 0; i < numAmort; ++i)
		{
			get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(numPeriods - 1 - i)) =
				signo * get<2>(amortNotionalByDate.at(numAmort - 1 - i));
			get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(numPeriods - 1 - i)) =
				signo * get<1>(amortNotionalByDate.at(numAmort - 1 - i));
		}
	}
	
	QCInterestRateLeg result{ periods, numPeriods - 1 };
	return result;
}

	QCInterestRateLeg QCFactoryFunctions::buildFloatingRateLeg3(
		QCDate valueDate,					//value date
		string receivePay,					//receive or pay
		QCDate startDate,					//start date
		QCDate endDate,						//end date
		vector<QCDate> settleCalendar,		//settlement calendar
		int settlementLag,					//settlement lag
		QCInterestRateLeg::QCStubPeriod stubPeriod,					//stub period
		string settlePeriodicity,			//settlement periodicity
		QCDate::QCBusDayAdjRules endDateAdjustment, //end date adjustment
		QCInterestRateLeg::QCAmortization amortization,				//amortization
		int fixingLag,								//fixing lag
		QCInterestRateLeg::QCStubPeriod fixingStubPeriod,			//fixing stub period
		string fixingPeriodicity,					//fixing periodicity
		vector<QCDate> fixingCalendar,				//fixing calendar
		pair<string, string> interestRateIndexChars,	//interest rate index tenor (3M, 6M ...)	
		double notional								//notional
		)
	{
		QCInterestRateLeg::QCInterestRatePeriods periods;
		size_t numPeriods;
		int signo;
		if (receivePay == "R") { signo = 1; }
		else { signo = -1; }

		//Se da de alta la fabrica de periods
		QCInterestRatePeriodsFactory factory{ startDate, endDate,
			endDateAdjustment,
			settlePeriodicity,
			stubPeriod,
			make_shared<vector<QCDate>>(settleCalendar),
			(unsigned int)settlementLag,
			fixingPeriodicity,
			fixingStubPeriod,
			make_shared<vector<QCDate>>(fixingCalendar),
			(unsigned int)fixingLag,
			(unsigned int)QCHelperFunctions::lagToInt(interestRateIndexChars.second),
			interestRateIndexChars.first };

		//Se generan los periodos
		periods = factory.getPeriods();

		numPeriods = periods.size();
		size_t currentPeriod = 0;

		//Determinar el current period
		for (long i = 0; i < numPeriods; ++i)
		{
			if (valueDate >= get<QCInterestRateLeg::intRtPrdElmntStartDate>(periods.at(i)) &&
				valueDate < get<QCInterestRateLeg::intRtPrdElmntEndDate>(periods.at(i)))
			{
				currentPeriod = i;
				break;
			}
		}

		if (amortization == QCInterestRateLeg::qcBulletAmort)
		{
			for (size_t i = currentPeriod; i < numPeriods; ++i)
			{
				get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(i)) = signo * notional;
			}
			get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(numPeriods - 1)) = signo * notional;
		}

		if (amortization == QCInterestRateLeg::qcConstantAmort)
		{
			double amort = signo * notional / (numPeriods - currentPeriod);
			double tempNotional = signo * notional;
			for (size_t i = currentPeriod; i < numPeriods; ++i)
			{
				get<QCInterestRateLeg::intRtPrdElmntNotional>(periods.at(i)) = tempNotional;
				get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(periods.at(i)) = amort;
				tempNotional -= amort;
			}
		}

		QCInterestRateLeg result{ periods, numPeriods - 1 };
		return result;
	}
