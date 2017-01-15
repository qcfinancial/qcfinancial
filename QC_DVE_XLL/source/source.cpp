
#include<cppinterface.h>
#include <algorithm>
#include <string>
#include <utility>
#include <memory>

#pragma warning (disable : 4996)

#include "QCTest.h"
#include "QCDate.h"
#include "QCAct360.h"
#include "QCAct365.h"
#include "QC30360.h"
#include "QCActAct.h"
#include "QCLinearWf.h"
#include "QCCompoundWf.h"
#include "QCContinousWf.h"
#include "QCInterestRate.h"
#include "QCInterestRateCurve.h"
#include "QCInterestRateLeg.h"
#include "QCFixedRatePayoff.h"
#include "QCFloatingRatePayoff.h"
#include "QCIcpClpPayoff.h"
#include "QCIcpClfPayoff.h"
#include "QCCurve.h"
#include "QCInterpolator.h"
#include "QCLinearInterpolator.h"
#include "QCLogLinearInterpolator.h"
#include "QCZeroCouponInterestRateCurve.h"
#include "QCProjectingInterestRateCurve.h"
#include "QCFactoryFunctions.h"
#include "QCHelperFunctions.h"
#include "QCInterestRatePeriodsFactory.h"

#include "HelperFunctions.h"

using namespace std;

#define BASIS_POINT .0001

double qcFecha(string f)
{
	QCDate fecha{ f };
	return (double)fecha.excelSerial();
}
double qcYearFraction(int startDate, int endDate, string yf)
{
	QCDate stDate{ startDate };
	QCDate eDate{ endDate };
	transform(yf.begin(), yf.end(), yf.begin(), ::toupper);
	if (yf == "ACT360")
	{
		QCAct360 act360;
		return act360.yf(stDate, eDate);
	}
	else if (yf == "ACT365")
	{
		QCAct365 act365;
		return act365.yf(stDate, eDate);
	}
	else if (yf == "30360")
	{
		QC30360 qc30360;
		return qc30360.yf(stDate, eDate);
	}
	else
	{
		QCActAct actAct;
		return actAct.yf(stDate, eDate);
	}
}

double qcWealthFactor(double rate, int stDt, int endDt, string yf, string wf)
{
	QCDate startDate{ stDt };
	QCDate endDate{ endDt };
	transform(yf.begin(), yf.end(), yf.begin(), ::toupper);
	transform(wf.begin(), wf.end(), wf.begin(), ::toupper);

	shared_ptr<QCWealthFactor> comp(new QCCompoundWf);
	shared_ptr<QCWealthFactor> lin(new QCLinearWf);
	shared_ptr<QCWealthFactor> cont(new QCContinousWf);

	shared_ptr<QCYearFraction> act360(new QCAct360);
	shared_ptr<QCYearFraction> act365(new QCAct365);
	shared_ptr<QCYearFraction> _30360(new QC30360);
	shared_ptr<QCYearFraction> actAct(new QCActAct);

	string aux{ wf + yf };
	if (aux == "COMACT360")
	{
		QCInterestRate ir{ rate, act360, comp };
		return ir.wf(startDate, endDate);
	}
	else if (aux == "COMACT365")
	{
		QCInterestRate ir{ rate, act365, comp };
		return ir.wf(startDate, endDate);
	}
	else if (aux == "COM30360")
	{
		QCInterestRate ir{ rate, _30360, comp };
		return ir.wf(startDate, endDate);
	}
	else if (aux == "COMACTACT")
	{
		QCInterestRate ir{ rate, actAct, comp };
		return ir.wf(startDate, endDate);
	}
	else if (aux == "LINACT360")
	{
		QCInterestRate ir{ rate, act360, lin };
		return ir.wf(startDate, endDate);
	}
	else if (aux == "LINACT365")
	{
		QCInterestRate ir{ rate, act365, lin };
		return ir.wf(startDate, endDate);
	}
	else if (aux == "LIN30360")
	{
		QCInterestRate ir{ rate, _30360, lin };
		return ir.wf(startDate, endDate);
	}
	else if (aux == "LINACTACT")
	{
		QCInterestRate ir{ rate, actAct, lin };
		return ir.wf(startDate, endDate);
	}
	else
	{
		QCInterestRate ir{ rate, act365, cont };
		return ir.wf(startDate, endDate);
	}

}

CellMatrix cashFlow(CellMatrix tablaDesarrollo, double tasa, int fecha, string yf, string wf)
{
	QCInterestRateLeg::QCInterestRatePeriods periods;
	unsigned int filas = tablaDesarrollo.RowsInStructure();

	for (unsigned int i = 0; i < filas; ++i)
	{
		periods.push_back(
			make_tuple(tablaDesarrollo(i, 0).NumericValue(),
			(bool)tablaDesarrollo(i, 1).NumericValue(),
			tablaDesarrollo(i, 2).NumericValue(),
			(bool)tablaDesarrollo(i, 3).NumericValue(),
			tablaDesarrollo(i, 4).NumericValue(),
			QCDate{ (long)tablaDesarrollo(i, 5).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 6).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 7).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 8).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 9).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 10).NumericValue() }
		));
	}

	/*QCInterestRate, QCInterestRateLeg, QCDate, QCZeroCouponCurve y QCTimeSeriesShrdPointer*/
	
	QCInterestRateLeg intLeg{ periods, filas - 1 };
	auto intLegPtr = make_shared<QCInterestRateLeg>(intLeg);

	//Contruimos shared_ptr de QCYearFraction. Por ahora solo QCAct360
	shared_ptr<QCYearFraction> act360Ptr(new QCAct360);
	//QCAct360 act360;
	//QCYrFrctnShrdPtr act360Ptr = make_shared<QCYearFraction>(act360);

	//Construimos un shared pointer de QCWealthFactor. Por ahora solo lineal.
	shared_ptr<QCWealthFactor> linPtr(new QCLinearWf);
	//QCLinearWf lin;
	//QCWlthFctrShrdPtr linPtr = make_shared<QCWealthFactor>(lin);

	//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
	QCInterestRate intRate{ tasa, act360Ptr, linPtr };
	QCIntrstRtShrdPtr intRatePtr = make_shared<QCInterestRate>(intRate);

	//Vamos a construir los shared_ptr de QCZeroCouponCurve y QCTimeSeries y los dejamos nulos
	QCZrCpnCrvShrdPtr crvPtr;
	QCTimeSeriesShrdPtr timeSrsPtr;

	//Construimos el objeto QCDate con la fecha de valorizacion
	QCDate valDate{ fecha }; //constructor que toma un Excel serial
	
	//Finalmente contruimos el payoff
	QCFixedRatePayoff fxRtPff{ intRatePtr, intLegPtr, crvPtr, valDate, timeSrsPtr };
	QCIntrstRtPffShrdPtr fxRtPffPtr = make_shared<QCFixedRatePayoff>(fxRtPff);

	CellMatrix result(50,3);

	fxRtPffPtr->payoff();
	for (int i = 0; i < fxRtPffPtr->payoffSize(); ++i)
	{
		//En cada elemento de payoff viene fecha, label, monto
		QCDate fechaTemp = get<0>(fxRtPffPtr->getCashflowAt(i));
		int label = get<1>(fxRtPffPtr->getCashflowAt(i));
		double monto = get<2>(fxRtPffPtr->getCashflowAt(i));

		result(i, 0) = fechaTemp.excelSerial();
		result(i, 1) = label;
		result(i, 2) = monto;
	}

	return result;
}

double pvFixed1(CellMatrix tablaDesarrollo,
	CellMatrix curva,
	double tasa, int fecha, string yf, string wf)
{
	QCInterestRateLeg::QCInterestRatePeriods periods;
	unsigned int filas = tablaDesarrollo.RowsInStructure();

	for (int i = 0; i < filas; ++i)
	{
		periods.push_back(
			make_tuple(tablaDesarrollo(i, 0).NumericValue(),
			(bool)tablaDesarrollo(i, 1).NumericValue(),
			tablaDesarrollo(i, 2).NumericValue(),
			(bool)tablaDesarrollo(i, 3).NumericValue(),
			tablaDesarrollo(i, 4).NumericValue(),
			QCDate{ (long)tablaDesarrollo(i, 5).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 6).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 7).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 8).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 9).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 10).NumericValue() }
		));
	}

	/*QCInterestRate, QCInterestRateLeg, QCDate, QCZeroCouponCurve y QCTimeSeriesShrdPointer*/

	//Construimos un shared pointer de QCInterestRateLeg
	QCInterestRateLeg intLeg{ periods, filas - 1 };
	auto intLegPtr = make_shared<QCInterestRateLeg>(intLeg);

	//Contruimos shared_ptr de QCYearFraction. Por ahora solo QCAct360
	shared_ptr<QCYearFraction> act360Ptr(new QCAct360);

	//Construimos un shared pointer de QCWealthFactor. Por ahora solo lineal.
	shared_ptr<QCWealthFactor> linPtr(new QCLinearWf);

	//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
	QCInterestRate intRate{ tasa, act360Ptr, linPtr };
	QCIntrstRtShrdPtr intRatePtr = make_shared<QCInterestRate>(intRate);

	//Vamos a construir el shared_ptr de QCZeroCouponCurve
	int puntosCurva = curva.RowsInStructure();
	vector<long> tenors;
	tenors.resize(puntosCurva);
	vector<double> rates;
	rates.resize(puntosCurva);
	for (int i = 0; i < puntosCurva; ++i)
	{
		tenors.at(i) = curva(i, 0).NumericValue();
		rates.at(i) = curva(i, 1).NumericValue();
	}

	shared_ptr<QCCurve<long>> crvPtr(new QCCurve<long>{ tenors, rates });
	shared_ptr<QCInterpolator> interpol(new QCLinearInterpolator{ crvPtr });
	//definir un interest rate y meterlo al constructior
	QCZrCpnCrvShrdPtr zrCrvPtr(new QCZeroCouponInterestRateCurve{ interpol, intRate });

	//El shared_ptr de QCTimeSeries lo dejamos nulo
	QCTimeSeriesShrdPtr timeSrsPtr;

	//Construimos el objeto QCDate con la fecha de valorizacion
	QCDate valDate{ fecha }; //constructor que toma un Excel serial

	//Finalmente contruimos el payoff
	QCFixedRatePayoff fxRtPff{ intRatePtr, intLegPtr, zrCrvPtr, valDate, timeSrsPtr };
	QCIntrstRtPffShrdPtr fxRtPffPtr = make_shared<QCFixedRatePayoff>(fxRtPff);

	return fxRtPffPtr->presentValue();
}

double pvFixed(CellMatrix tablaDesarrollo,
	CellMatrix curva,
	double tasa,
	int fecha,
	string yf,
	string wf,
	string curveYf,
	string curveWf)
{
	QCInterestRateLeg::QCInterestRatePeriods periods;
	unsigned int filas = tablaDesarrollo.RowsInStructure();

	for (int i = 0; i < filas; ++i)
	{
		periods.push_back(
			make_tuple(tablaDesarrollo(i, 0).NumericValue(),
			(bool)tablaDesarrollo(i, 1).NumericValue(),
			tablaDesarrollo(i, 2).NumericValue(),
			(bool)tablaDesarrollo(i, 3).NumericValue(),
			tablaDesarrollo(i, 4).NumericValue(),
			QCDate{ (long)tablaDesarrollo(i, 5).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 6).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 7).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 8).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 9).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 10).NumericValue() }
		));
	}

	//QCInterestRate, QCInterestRateLeg, QCDate, QCZeroCouponCurve y QCTimeSeriesShrdPointer

	//Construimos un shared pointer de QCInterestRateLeg
	QCInterestRateLeg intLeg{ periods, filas - 1 };
	auto intLegPtr = make_shared<QCInterestRateLeg>(intLeg);

	//Contruimos shared_ptr de QCYearFraction.
	shared_ptr<QCYearFraction> yfShrdPtr = QCFactoryFunctions::yfSharedPtr(yf);

	//Construimos un shared pointer de QCWealthFactor.
	shared_ptr<QCWealthFactor> wfShrdPtr = QCFactoryFunctions::wfSharedPtr(wf);

	//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
	//Este se usara para instanciar la QCFixedRateLeg
	QCInterestRate intRate{ tasa, yfShrdPtr, wfShrdPtr };
	QCIntrstRtShrdPtr intRatePtr = make_shared<QCInterestRate>(intRate);

	//Vamos a construir el shared_ptr de QCZeroCouponCurve
	int puntosCurva = curva.RowsInStructure();
	vector<long> tenors;
	tenors.resize(puntosCurva);
	vector<double> rates;
	rates.resize(puntosCurva);
	for (int i = 0; i < puntosCurva; ++i)
	{
		tenors.at(i) = curva(i, 0).NumericValue();
		rates.at(i) = curva(i, 1).NumericValue();
	}

	//definir la curva cero cupon de descuento
	QCZrCpnCrvShrdPtr zrCrvPtr = QCFactoryFunctions::zrCpnCrvShrdPtr(tenors,
		rates, "linear", curveYf, curveWf);

	//El shared_ptr de QCTimeSeries lo dejamos nulo
	QCTimeSeriesShrdPtr timeSrsPtr;

	//Construimos el objeto QCDate con la fecha de valorizacion
	QCDate valDate{ fecha }; //constructor que toma un Excel serial

	//Finalmente contruimos el payoff
	QCFixedRatePayoff fxRtPff{ intRatePtr, intLegPtr, zrCrvPtr, valDate, timeSrsPtr };
	QCIntrstRtPffShrdPtr fxRtPffPtr = make_shared<QCFixedRatePayoff>(fxRtPff);

	return fxRtPffPtr->presentValue();
}

double pvFloat1(CellMatrix tablaDesarrollo,
	CellMatrix curvaProy,
	CellMatrix curvaDesc,
	CellMatrix pastFixings,
	double addSpread, double multSpread,
	int fecha, string yf, string wf)
{
	QCInterestRateLeg::QCInterestRatePeriods periods;
	unsigned int filas = tablaDesarrollo.RowsInStructure();

	for (int i = 0; i < filas; ++i)
	{
		periods.push_back(
			make_tuple(tablaDesarrollo(i, 0).NumericValue(),
			(bool)tablaDesarrollo(i, 1).NumericValue(),
			tablaDesarrollo(i, 2).NumericValue(),
			(bool)tablaDesarrollo(i, 3).NumericValue(),
			tablaDesarrollo(i, 4).NumericValue(),
			QCDate{ (long)tablaDesarrollo(i, 5).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 6).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 7).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 8).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 9).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 10).NumericValue() }
		));
	}

	/*QCInterestRate, QCInterestRateLeg, QCDate, QCZeroCouponCurve y QCTimeSeriesShrdPointer*/

	//Construimos un shared pointer de QCInterestRateLeg
	QCInterestRateLeg intLeg{ periods, filas - 1 };
	auto intLegPtr = make_shared<QCInterestRateLeg>(intLeg);

	//Contruimos shared_ptr de QCYearFraction. Por ahora solo QCAct360
	shared_ptr<QCYearFraction> act360Ptr(new QCAct360);

	//Construimos un shared pointer de QCWealthFactor. Por ahora solo lineal.
	shared_ptr<QCWealthFactor> linPtr(new QCLinearWf);

	//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
	QCInterestRate intRate{ 0.0, act360Ptr, linPtr };
	QCIntrstRtShrdPtr intRatePtr = make_shared<QCInterestRate>(intRate);

	//Vamos a construir el shared_ptr de QCZeroCouponCurve para la curva de descuento
	int puntosCurva = curvaDesc.RowsInStructure();
	vector<long> tenors;
	tenors.resize(puntosCurva);
	vector<double> rates;
	rates.resize(puntosCurva);
	for (int i = 0; i < puntosCurva; ++i)
	{
		tenors.at(i) = curvaDesc(i, 0).NumericValue();
		rates.at(i) = curvaDesc(i, 1).NumericValue();
	}

	shared_ptr<QCCurve<long>> crvPtr(new QCCurve<long>{ tenors, rates });
	shared_ptr<QCInterpolator> interpol(new QCLinearInterpolator{ crvPtr });
	//definir un interest rate y meterlo al constructior
	QCZrCpnCrvShrdPtr discZrCrvPtr(new QCZeroCouponInterestRateCurve{ interpol, intRate });

	//Vamos a construir el shared_ptr de QCZeroCouponCurve para la curva de proyeccion
	int puntosCurva2 = curvaProy.RowsInStructure();
	vector<long> tenors2;
	tenors2.resize(puntosCurva2);
	vector<double> rates2;
	rates2.resize(puntosCurva2);
	for (int i = 0; i < puntosCurva2; ++i)
	{
		tenors2.at(i) = curvaProy(i, 0).NumericValue();
		rates2.at(i) = curvaProy(i, 1).NumericValue();
	}
	
	shared_ptr<QCCurve<long>> crvPtr2(new QCCurve<long>{ tenors2, rates2 });
	shared_ptr<QCInterpolator> interpol2(new QCLinearInterpolator{ crvPtr2 });
	//definir un interest rate y meterlo al constructior
	QCIntRtCrvShrdPtr intRtProjCrvPtr(new QCProjectingInterestRateCurve{ interpol2, intRate });

	//El shared_ptr de QCTimeSeries lo dejamos nulo
	QCTimeSeriesShrdPtr timeSrsPtr;

	//Construimos el objeto QCDate con la fecha de valorizacion
	QCDate valDate{ fecha }; //constructor que toma un Excel serial

	//Construimos el objeto con los fixing dates
	//typedef shared_ptr<map<QCDate, double>> QCTimeSeriesShrdPtr;
	int puntosFixing = pastFixings.RowsInStructure();
	map<QCDate, double> fixings;
	for (int i = 0; i < puntosFixing; ++i)
	{
		QCDate fch = QCDate{ (long)pastFixings(i, 0).NumericValue() };
		fixings.insert(pair<QCDate, double>(fch, pastFixings(i, 1).NumericValue()));
	}
	QCTimeSeriesShrdPtr fixingsShrdPtr = make_shared<map<QCDate, double>>(fixings);
	
	//Finalmente contruimos el payoff
	QCFloatingRatePayoff fltRtPff{ intRatePtr, addSpread, multSpread, intLegPtr,
		intRtProjCrvPtr, discZrCrvPtr, valDate, fixingsShrdPtr };
	QCIntrstRtPffShrdPtr fltRtPffPtr = make_shared<QCFloatingRatePayoff>(fltRtPff);

	return  fltRtPffPtr->presentValue();
}

/*double //Valor presente de una pata a tasa flotante
	pvFloat(CellMatrix schedule	//Tabla de desarrollo de la pata
	, CellMatrix projCurve		//Curva de proyeccion
	, string projYf				//Nombre de la fraccion de agno de la curva de proyeccion
	, string projWf				//Nombre del factor de capitalizacion de la curva de proyeccion
	, CellMatrix discCurve		//Curva de descuento
	, string discYf				//Nombre de la fraccion de agno de la curva de descuento
	, string discWf				//Nombre del factor de capitalizacion de la curva de descuento
	, CellMatrix pastFixings	//Fixings anteriores
	, double addSpread			//Spread aditivo
	, double multSpread			//Spread multiplicativo
	, int valueDate				//Fecha de valorizacion
	, string floatRateYf		//Yf de la tasa flotante
	, string floatRateWf		//Wf de la tasa flotante
	)
{
	//Se construye un shared pointer de QCInterestRateLeg
	//Primero se registran los contenidos de la CellMatrix schedule en un 
	//objeto periods.

	QCInterestRatePeriods periods;
	int filas = schedule.RowsInStructure();

	for (int i = 0; i < filas; ++i)
	{
		periods.push_back(
			make_tuple(schedule(i, 0).NumericValue(),
			(bool)schedule(i, 1).NumericValue(),
			schedule(i, 2).NumericValue(),
			(bool)schedule(i, 3).NumericValue(),
			schedule(i, 4).NumericValue(),
			QCDate{ (long)schedule(i, 5).NumericValue() },
			QCDate{ (long)schedule(i, 6).NumericValue() },
			QCDate{ (long)schedule(i, 7).NumericValue() },
			QCDate{ (long)schedule(i, 8).NumericValue() },
			QCDate{ (long)schedule(i, 9).NumericValue() },
			QCDate{ (long)schedule(i, 10).NumericValue() }
		));
	}

	QCIntrstRtPrdsShrdPntr periodsPtr = make_shared<QCInterestRatePeriods>(periods);
	QCInterestRateLeg intLeg{ periodsPtr, filas - 1 };
	QCIntrstRtLgShrdPtr intLegPtr = make_shared<QCInterestRateLeg>(intLeg);

	//Se construye la curvas de descuento
	//Contruimos shared_ptr de QCYearFraction
	shared_ptr<QCYearFraction> projCurveYfShrdPtr = QCFactoryFunctions::yfSharedPtr(projYf);
	//Construimos un shared pointer de QCWealthFactor.
	shared_ptr<QCWealthFactor> projCurveWfShrdPtr = QCFactoryFunctions::wfSharedPtr(projWf);

	//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
	QCInterestRate projIntRate{ 0.0, projCurveYfShrdPtr, projCurveWfShrdPtr };
	QCIntrstRtShrdPtr projIntRatePtr = make_shared<QCInterestRate>(projIntRate);

	//ESTOY AQUI TRABAJANDO 

	int puntosCurva2 = projCurve.RowsInStructure();
	vector<long> tenors2;
	tenors2.resize(puntosCurva2);
	vector<double> rates2;
	rates2.resize(puntosCurva2);
	for (int i = 0; i < puntosCurva2; ++i)
	{
		tenors2.at(i) = projCurve(i, 0).NumericValue();
		rates2.at(i) = projCurve(i, 1).NumericValue();
	}

	shared_ptr<QCCurve<long>> crvPtr2(new QCCurve<long>{ tenors2, rates2 });
	shared_ptr<QCInterpolator> interpol2(new QCLinearInterpolator{ crvPtr2 });
	//definir un interest rate y meterlo al constructior
	QCIntRtCrvShrdPtr intRtProjCrvPtr(new QCZeroCouponInterestRateCurve{ interpol2, intRate });

	//Para el intRateLeg
	//Contruimos shared_ptr de QCYearFraction.
	shared_ptr<QCYearFraction> yfShrdPtr = QCFactoryFunctions::yfSharedPtr(yf);

	//Construimos un shared pointer de QCWealthFactor.
	shared_ptr<QCWealthFactor> wfShrdPtr = QCFactoryFunctions::wfSharedPtr(wf);

	//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
	QCInterestRate intRate2{ 0.0, yfShrdPtr, wfShrdPtr };
	QCIntrstRtShrdPtr intRate2Ptr = make_shared<QCInterestRate>(intRate2);

	//Vamos a construir el shared_ptr de QCZeroCouponCurve para la curva de descuento
	int puntosCurva = discCurve.RowsInStructure();
	vector<long> tenors;
	tenors.resize(puntosCurva);
	vector<double> rates;
	rates.resize(puntosCurva);
	for (int i = 0; i < puntosCurva; ++i)
	{
		tenors.at(i) = discCurve(i, 0).NumericValue();
		rates.at(i) = discCurve(i, 1).NumericValue();
	}

	shared_ptr<QCCurve<long>> crvPtr(new QCCurve<long>{ tenors, rates });
	shared_ptr<QCInterpolator> interpol(new QCLinearInterpolator{ crvPtr });
	//definir un interest rate y meterlo al constructior
	QCZrCpnCrvShrdPtr discZrCrvPtr(new QCZeroCouponInterestRateCurve{ interpol, intRate });
	
	//El shared_ptr de QCTimeSeries lo dejamos nulo
	QCTimeSeriesShrdPtr timeSrsPtr;

	//Construimos el objeto QCDate con la fecha de valorizacion
	QCDate valDate{ valueDate }; //constructor que toma un Excel serial

	//Construimos el objeto con los fixing dates
	//typedef shared_ptr<map<QCDate, double>> QCTimeSeriesShrdPtr;
	int puntosFixing = pastFixings.RowsInStructure();
	map<QCDate, double> fixings;
	for (int i = 0; i < puntosFixing; ++i)
	{
	QCDate fch = QCDate{ (long)pastFixings(i, 0).NumericValue() };
	fixings.insert(pair<QCDate, double>(fch, pastFixings(i, 1).NumericValue()));
	}
	QCTimeSeriesShrdPtr fixingsShrdPtr; // = make_shared<map<QCDate, double>>(fixings);

	//Finalmente contruimos el payoff
	QCIcpClpPayoff fltRtPff{ intRate2Ptr, addSpread, multSpread, intLegPtr,
		intRtProjCrvPtr, discZrCrvPtr, valDate, fixingsShrdPtr };
	QCIntrstRtPffShrdPtr fltRtPffPtr = make_shared<QCIcpClpPayoff>(fltRtPff);
	return  fltRtPffPtr->presentValue();

	return 0.0;
}*/

double pvIcpClp1(CellMatrix tablaDesarrollo //Tabla de desarrollo de la pata
	, CellMatrix curvaProy		//Curva de proyeccion
	, CellMatrix curvaDesc		//Curva de descuento
	, CellMatrix pastFixings	//Fixings pasados de ICP
	, double addSpread			//Spread aditivo
	, double multSpread			//Spread multiplicativo
	, int fecha					//Fecha de valorizacion
	, string yf					//Fraccion de año de la tasa flotante
	, string wf					//Factor de capitalizacion de la tasa flotante
	)
	{
		QCInterestRateLeg::QCInterestRatePeriods periods;
		unsigned int filas = tablaDesarrollo.RowsInStructure();

		for (int i = 0; i < filas; ++i)
		{
			periods.push_back(
				make_tuple(tablaDesarrollo(i, 0).NumericValue(),
				(bool)tablaDesarrollo(i, 1).NumericValue(),
				tablaDesarrollo(i, 2).NumericValue(),
				(bool)tablaDesarrollo(i, 3).NumericValue(),
				tablaDesarrollo(i, 4).NumericValue(),
				QCDate{ (long)tablaDesarrollo(i, 5).NumericValue() },
				QCDate{ (long)tablaDesarrollo(i, 6).NumericValue() },
				QCDate{ (long)tablaDesarrollo(i, 7).NumericValue() },
				QCDate{ (long)tablaDesarrollo(i, 8).NumericValue() },
				QCDate{ (long)tablaDesarrollo(i, 9).NumericValue() },
				QCDate{ (long)tablaDesarrollo(i, 10).NumericValue() }
			));
		}

		/*QCInterestRate, QCInterestRateLeg, QCDate, QCZeroCouponCurve y QCTimeSeriesShrdPointer*/

		//Construimos un shared pointer de QCInterestRateLeg
		QCInterestRateLeg intLeg{ periods, filas - 1 };
		auto intLegPtr = make_shared<QCInterestRateLeg>(intLeg);

		//Para las curvas
		//Contruimos shared_ptr de QCYearFraction.
		shared_ptr<QCYearFraction> act365Ptr(new QCAct365);

		//Construimos un shared pointer de QCWealthFactor.
		shared_ptr<QCWealthFactor> comPtr(new QCCompoundWf);

		//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
		QCInterestRate intRate{ 0.0, act365Ptr, comPtr };
		QCIntrstRtShrdPtr intRatePtr = make_shared<QCInterestRate>(intRate);

		//Para el intRateLeg
		//Contruimos shared_ptr de QCYearFraction.
		shared_ptr<QCYearFraction> act360Ptr(new QCAct360);

		//Construimos un shared pointer de QCWealthFactor.
		shared_ptr<QCWealthFactor> linPtr(new QCLinearWf);

		//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
		QCInterestRate intRate2{ 0.0, act360Ptr, linPtr };
		QCIntrstRtShrdPtr intRate2Ptr = make_shared<QCInterestRate>(intRate2);
		//Vamos a construir el shared_ptr de QCZeroCouponCurve para la curva de descuento
		int puntosCurva = curvaDesc.RowsInStructure();
		vector<long> tenors;
		tenors.resize(puntosCurva);
		vector<double> rates;
		rates.resize(puntosCurva);
		for (int i = 0; i < puntosCurva; ++i)
		{
			tenors.at(i) = curvaDesc(i, 0).NumericValue();
			rates.at(i) = curvaDesc(i, 1).NumericValue();
		}

		shared_ptr<QCCurve<long>> crvPtr(new QCCurve<long>{ tenors, rates });
		shared_ptr<QCInterpolator> interpol(new QCLinearInterpolator{ crvPtr });
		//definir un interest rate y meterlo al constructior
		QCZrCpnCrvShrdPtr discZrCrvPtr(new QCZeroCouponInterestRateCurve{ interpol, intRate });

		//Vamos a construir el shared_ptr de QCZeroCouponCurve para la curva de proyeccion
		int puntosCurva2 = curvaProy.RowsInStructure();
		vector<long> tenors2;
		tenors2.resize(puntosCurva2);
		vector<double> rates2;
		rates2.resize(puntosCurva2);
		for (int i = 0; i < puntosCurva2; ++i)
		{
			tenors2.at(i) = curvaProy(i, 0).NumericValue();
			rates2.at(i) = curvaProy(i, 1).NumericValue();
		}

		shared_ptr<QCCurve<long>> crvPtr2(new QCCurve<long>{ tenors2, rates2 });
		shared_ptr<QCInterpolator> interpol2(new QCLinearInterpolator{ crvPtr2 });
		//definir un interest rate y meterlo al constructior
		QCIntRtCrvShrdPtr intRtProjCrvPtr(new QCZeroCouponInterestRateCurve{ interpol2, intRate });

		//El shared_ptr de QCTimeSeries lo dejamos nulo
		QCTimeSeriesShrdPtr timeSrsPtr;

		//Construimos el objeto QCDate con la fecha de valorizacion
		QCDate valDate{ fecha }; //constructor que toma un Excel serial

		//Construimos el objeto con los fixing dates
		//typedef shared_ptr<map<QCDate, double>> QCTimeSeriesShrdPtr;
		int puntosFixing = pastFixings.RowsInStructure();
		map<QCDate, double> fixings;
		for (int i = 0; i < puntosFixing; ++i)
		{
			QCDate fch = QCDate{ (long)pastFixings(i, 0).NumericValue() };
			fixings.insert(pair<QCDate, double>(fch, pastFixings(i, 1).NumericValue()));
		}
		QCTimeSeriesShrdPtr fixingsShrdPtr = make_shared<map<QCDate, double>>(fixings);

		//Finalmente contruimos el payoff
		QCIcpClpPayoff fltRtPff{ intRate2Ptr, addSpread, multSpread, intLegPtr,
			intRtProjCrvPtr, discZrCrvPtr, valDate, fixingsShrdPtr };
		QCIntrstRtPffShrdPtr fltRtPffPtr = make_shared<QCIcpClpPayoff>(fltRtPff);

		return  fltRtPffPtr->presentValue();
	}

double pvIcpClp(CellMatrix tablaDesarrollo //Tabla de desarrollo de la pata
	, CellMatrix curvaProy		//Curva de proyeccion
	, CellMatrix curvaDesc		//Curva de descuento
	, CellMatrix pastFixings	//Fixings pasados de ICP
	, double addSpread			//Spread aditivo
	, double multSpread			//Spread multiplicativo
	, int fecha					//Fecha de valorizacion
	, string yf					//Fraccion de año de la tasa flotante
	, string wf					//Factor de capitalizacion de la tasa flotante
	, string curveYf			//Fraccion de agno de la curva
	, string curveWf			//Factor de capitalizacion de la curva
	)
{
	QCInterestRateLeg::QCInterestRatePeriods periods;
	unsigned int filas = tablaDesarrollo.RowsInStructure();

	for (int i = 0; i < filas; ++i)
	{
		periods.push_back(
			make_tuple(tablaDesarrollo(i, 0).NumericValue(),
			(bool)tablaDesarrollo(i, 1).NumericValue(),
			tablaDesarrollo(i, 2).NumericValue(),
			(bool)tablaDesarrollo(i, 3).NumericValue(),
			tablaDesarrollo(i, 4).NumericValue(),
			QCDate{ (long)tablaDesarrollo(i, 5).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 6).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 7).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 8).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 9).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 10).NumericValue() }
		));
	}

		
	/*QCInterestRate, QCInterestRateLeg, QCDate, QCZeroCouponCurve y QCTimeSeriesShrdPointer*/

	//Construimos un shared pointer de QCInterestRateLeg
	QCInterestRateLeg intLeg{ periods, filas - 1 };
	auto intLegPtr = make_shared<QCInterestRateLeg>(intLeg);

	//Para las curvas
	//Contruimos shared_ptr de QCYearFraction.
	shared_ptr<QCYearFraction> yfCurveShrdPtr = QCFactoryFunctions::yfSharedPtr(curveYf);
	//Construimos un shared pointer de QCWealthFactor.
	shared_ptr<QCWealthFactor> wfCurveShrdPtr = QCFactoryFunctions::wfSharedPtr(curveWf);

	//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
	QCInterestRate intRate{ 0.0, yfCurveShrdPtr, wfCurveShrdPtr };
	QCIntrstRtShrdPtr intRatePtr = make_shared<QCInterestRate>(intRate);

	//Para el intRateLeg
	//Contruimos shared_ptr de QCYearFraction.
	shared_ptr<QCYearFraction> yfShrdPtr = QCFactoryFunctions::yfSharedPtr(yf);

	//Construimos un shared pointer de QCWealthFactor.
	shared_ptr<QCWealthFactor> wfShrdPtr = QCFactoryFunctions::wfSharedPtr(wf);

	//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
	QCInterestRate intRate2{ 0.0, yfShrdPtr, wfShrdPtr };
	QCIntrstRtShrdPtr intRate2Ptr = make_shared<QCInterestRate>(intRate2);
	
	//Vamos a construir el shared_ptr de QCZeroCouponCurve para la curva de descuento
	int puntosCurva = curvaDesc.RowsInStructure();
	vector<long> tenors;
	tenors.resize(puntosCurva);
	vector<double> rates;
	rates.resize(puntosCurva);
	for (int i = 0; i < puntosCurva; ++i)
	{
		tenors.at(i) = curvaDesc(i, 0).NumericValue();
		rates.at(i) = curvaDesc(i, 1).NumericValue();
	}

	shared_ptr<QCCurve<long>> crvPtr(new QCCurve<long>{ tenors, rates });
	shared_ptr<QCInterpolator> interpol(new QCLinearInterpolator{ crvPtr });
	//definir un interest rate y meterlo al constructor
	QCZrCpnCrvShrdPtr discZrCrvPtr(new QCZeroCouponInterestRateCurve{ interpol, intRate });

	//Vamos a construir el shared_ptr de QCZeroCouponCurve para la curva de proyeccion
	int puntosCurva2 = curvaProy.RowsInStructure();
	vector<long> tenors2;
	tenors2.resize(puntosCurva2);
	vector<double> rates2;
	rates2.resize(puntosCurva2);
	for (int i = 0; i < puntosCurva2; ++i)
	{
		tenors2.at(i) = curvaProy(i, 0).NumericValue();
		rates2.at(i) = curvaProy(i, 1).NumericValue();
	}

	shared_ptr<QCCurve<long>> crvPtr2(new QCCurve<long>{ tenors2, rates2 });
	shared_ptr<QCInterpolator> interpol2(new QCLinearInterpolator{ crvPtr2 });
	//definir un interest rate y meterlo al constructior
	QCIntRtCrvShrdPtr intRtProjCrvPtr(new QCZeroCouponInterestRateCurve{ interpol2, intRate });
	//El shared_ptr de QCTimeSeries lo dejamos nulo
	QCTimeSeriesShrdPtr timeSrsPtr;

	//Construimos el objeto QCDate con la fecha de valorizacion
	QCDate valDate{ fecha }; //constructor que toma un Excel serial

	//Construimos el objeto con los fixing dates
	//typedef shared_ptr<map<QCDate, double>> QCTimeSeriesShrdPtr;
	/*int puntosFixing = pastFixings.RowsInStructure();
	map<QCDate, double> fixings;
	for (int i = 0; i < puntosFixing; ++i)
	{
		QCDate fch = QCDate{ (long)pastFixings(i, 0).NumericValue() };
		fixings.insert(pair<QCDate, double>(fch, pastFixings(i, 1).NumericValue()));
	}*/
	QCTimeSeriesShrdPtr fixingsShrdPtr; // = make_shared<map<QCDate, double>>(fixings);

	//Finalmente contruimos el payoff
	QCIcpClpPayoff fltRtPff{ intRate2Ptr, addSpread, multSpread, intLegPtr,
		intRtProjCrvPtr, discZrCrvPtr, valDate, fixingsShrdPtr };
	QCIntrstRtPffShrdPtr fltRtPffPtr = make_shared<QCIcpClpPayoff>(fltRtPff);
	return  fltRtPffPtr->presentValue();
}

double pvIcpClf(CellMatrix tablaDesarrollo //Tabla de desarrollo de la pata
	, CellMatrix curvaProy		//Curva de proyeccion
	, CellMatrix curvaDesc		//Curva de descuento
	, CellMatrix pastFixings	//Fixings pasados de ICP
	, CellMatrix ufFixings		//Fixings pasado de UF
	, double addSpread			//Spread aditivo
	, double multSpread			//Spread multiplicativo
	, int fecha					//Fecha de valorizacion
	, string yf					//Fraccion de año de la tasa flotante
	, string wf					//Factor de capitalizacion de la tasa flotante
	)
	{
		QCInterestRateLeg::QCInterestRatePeriods periods;
		unsigned int filas = tablaDesarrollo.RowsInStructure();

		for (int i = 0; i < filas; ++i)
		{
			periods.push_back(
				make_tuple(tablaDesarrollo(i, 0).NumericValue(),
				(bool)tablaDesarrollo(i, 1).NumericValue(),
				tablaDesarrollo(i, 2).NumericValue(),
				(bool)tablaDesarrollo(i, 3).NumericValue(),
				tablaDesarrollo(i, 4).NumericValue(),
				QCDate{ (long)tablaDesarrollo(i, 5).NumericValue() },
				QCDate{ (long)tablaDesarrollo(i, 6).NumericValue() },
				QCDate{ (long)tablaDesarrollo(i, 7).NumericValue() },
				QCDate{ (long)tablaDesarrollo(i, 8).NumericValue() },
				QCDate{ (long)tablaDesarrollo(i, 9).NumericValue() },
				QCDate{ (long)tablaDesarrollo(i, 10).NumericValue() }
			));
		}

		/*QCInterestRate, QCInterestRateLeg, QCDate, QCZeroCouponCurve y QCTimeSeriesShrdPointer*/

		//Construimos un shared pointer de QCInterestRateLeg
		QCInterestRateLeg intLeg{ periods, filas - 1 };
		auto intLegPtr = make_shared<QCInterestRateLeg>(intLeg);

		//Para las curvas
		//Contruimos shared_ptr de QCYearFraction.
		shared_ptr<QCYearFraction> act365Ptr(new QCAct365);

		//Construimos un shared pointer de QCWealthFactor.
		shared_ptr<QCWealthFactor> comPtr(new QCCompoundWf);

		//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
		QCInterestRate intRate{ 0.0, act365Ptr, comPtr };
		QCIntrstRtShrdPtr intRatePtr = make_shared<QCInterestRate>(intRate);

		//Para el intRateLeg
		//Contruimos shared_ptr de QCYearFraction.
		shared_ptr<QCYearFraction> act360Ptr(new QCAct360);

		//Construimos un shared pointer de QCWealthFactor.
		shared_ptr<QCWealthFactor> linPtr(new QCLinearWf);

		//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
		QCInterestRate intRate2{ 0.0, act360Ptr, linPtr };
		QCIntrstRtShrdPtr intRate2Ptr = make_shared<QCInterestRate>(intRate2);
		//Vamos a construir el shared_ptr de QCZeroCouponCurve para la curva de descuento
		int puntosCurva = curvaDesc.RowsInStructure();
		vector<long> tenors;
		tenors.resize(puntosCurva);
		vector<double> rates;
		rates.resize(puntosCurva);
		for (int i = 0; i < puntosCurva; ++i)
		{
			tenors.at(i) = curvaDesc(i, 0).NumericValue();
			rates.at(i) = curvaDesc(i, 1).NumericValue();
		}

		shared_ptr<QCCurve<long>> crvPtr(new QCCurve<long>{ tenors, rates });
		shared_ptr<QCInterpolator> interpol(new QCLinearInterpolator{ crvPtr });
		//definir un interest rate y meterlo al constructior
		QCZrCpnCrvShrdPtr discZrCrvPtr(new QCZeroCouponInterestRateCurve{ interpol, intRate });

		//Vamos a construir el shared_ptr de QCZeroCouponCurve para la curva de proyeccion
		int puntosCurva2 = curvaProy.RowsInStructure();
		vector<long> tenors2;
		tenors2.resize(puntosCurva2);
		vector<double> rates2;
		rates2.resize(puntosCurva2);
		for (int i = 0; i < puntosCurva2; ++i)
		{
			tenors2.at(i) = curvaProy(i, 0).NumericValue();
			rates2.at(i) = curvaProy(i, 1).NumericValue();
		}

		shared_ptr<QCCurve<long>> crvPtr2(new QCCurve<long>{ tenors2, rates2 });
		shared_ptr<QCInterpolator> interpol2(new QCLinearInterpolator{ crvPtr2 });
		//definir un interest rate y meterlo al constructior
		QCIntRtCrvShrdPtr intRtProjCrvPtr(new QCZeroCouponInterestRateCurve{ interpol2, intRate });

		//El shared_ptr de QCTimeSeries lo dejamos nulo
		QCTimeSeriesShrdPtr timeSrsPtr;

		//Construimos el objeto QCDate con la fecha de valorizacion
		QCDate valDate{ fecha }; //constructor que toma un Excel serial

		//Construimos el objeto con los fixing de ICP
		//typedef shared_ptr<map<QCDate, double>> QCTimeSeriesShrdPtr;
		int puntosFixing = pastFixings.RowsInStructure();
		map<QCDate, double> fixings;
		for (int i = 0; i < puntosFixing; ++i)
		{
			QCDate fch = QCDate{ (long)pastFixings(i, 0).NumericValue() };
			fixings.insert(pair<QCDate, double>(fch, pastFixings(i, 1).NumericValue()));
		}
		QCTimeSeriesShrdPtr fixingsShrdPtr = make_shared<map<QCDate, double>>(fixings);

		//Construimos el objeto con los fixing de UF
		//typedef shared_ptr<map<QCDate, double>> QCTimeSeriesShrdPtr;
		int puntosFixingUF = ufFixings.RowsInStructure();
		map<QCDate, double> mapFixingsUF;
		for (int i = 0; i < puntosFixingUF; ++i)
		{
			QCDate fch = QCDate{ (long)ufFixings(i, 0).NumericValue() };
			mapFixingsUF.insert(pair<QCDate, double>(fch, ufFixings(i, 1).NumericValue()));
		}
		QCTimeSeriesShrdPtr ufFixingsShrdPtr = make_shared<map<QCDate, double>>(mapFixingsUF);

		//Finalmente contruimos el payoff
		QCIcpClfPayoff fltRtPff{ intRate2Ptr, addSpread, multSpread, intLegPtr,
			intRtProjCrvPtr, discZrCrvPtr, valDate, fixingsShrdPtr, ufFixingsShrdPtr };
		QCIntrstRtPffShrdPtr fltRtPffPtr = make_shared<QCIcpClfPayoff>(fltRtPff);

		return  fltRtPffPtr->presentValue();
	}

CellMatrix pvFixedLegs(double valueDate,
		CellMatrix holidays,
		CellMatrix curveValues,
		CellMatrix curveCharacteristics,
		CellMatrix legCharacteristics,
		CellMatrix customAmort)
{
	//holidays: nombre, fecha. Se construye un map<string, vector<QCDate>> que para cada nombre de calendario
	//tenga todas las fechas
	map<string, vector<QCDate>> mapHolidays;
	HelperFunctions::buildHolidays(holidays, mapHolidays);

	//curveValues: nombre, tenor (en dias), valor (tasa o df)
	//Se construye un map<string, pair<vector<long>, vector<double>>> que contiene las curvas
	map<string, pair<vector<long>, vector<double>>> crvValues;
	HelperFunctions::buildCurveValues(curveValues, crvValues);

	//Toca ahora construir las curvas con su interpolador y sus convenciones de tasas
	//Primero metemos las caracteristicas de las curvas en un
	//map<string, tuple<string, string, string, string>> = HelperFunctions::string4
	map<string, HelperFunctions::string4> crvChars;
	HelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

	//Para cada uno de los elementos de crvValues debo terminar de dar de alta una curva cero
	//Tengo los plazos, las tasas y los nombres del interpolador, yf, wf y tipo de curva.
	//Necesito entonces llamar una funcion factory que con estos argumentos me de de alta un curva.
	//las curvas que vaya dando de alta las meto en un map
	
	map<string, shared_ptr<QCZeroCouponCurve>> allCurves;

	//Loopeo sobre los keys de crvValues
	for (const auto &curva : crvValues)
	{
		vector<long> tmpLng{ curva.second.first };
		vector<double> tmpDbl{ curva.second.second };
		
		string wf = get<1>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(wf);
		
		string yf = get<2>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(yf);
		
		QCZrCpnCrvShrdPtr tmpCrv = QCFactoryFunctions::zrCpnCrvShrdPtr(
			tmpLng,
			tmpDbl,
			get<0>(crvChars.at(curva.first)),
			wf, yf, get<3>(crvChars.at(curva.first)));
			allCurves.insert(pair <string, shared_ptr<QCZeroCouponCurve>>(curva.first, tmpCrv));
	}

	//Guardaremos el CellMatrix customAmort con los datos de nominal vigente y amortizacion en
	//esta estructura.
	map<unsigned long, vector<tuple<QCDate, double, double>>> dateNotionalAndAmortByIdLeg;
	HelperFunctions::buildCustomAmortization(customAmort, dateNotionalAndAmortByIdLeg);

	//Ahora hay que construir QCInterestRatePayoff para cada operacion
	//Se requiere:
	//	shared_ptr<QCInterestRate>
	//	shared_ptr<QCInterestRateLeg>
	//	QCDate (valueDate)
	//	shared_ptr<QCZeroCouponCurve> (la curva de descuento)
	//	shared_ptr<QCTimeSeries> (los fixings)

	//La info esta en la CellMatriz legCharacteristics:
	//	0:	id_leg
	//	1:	rec_pay
	//	2:	start_date
	//	3:	end_date
	//	4:	settlement_calendar
	//	5:	settlement_lag
	//	6:	stub_period
	//	7:	periodicity
	//	8:	end_date_adjustment
	//	9:	amortization
	//	10: fixed_rate
	//	11: notional
	//	12: wealth_factor
	//	13: year_fraction
	//	14: discount_curve

	QCDate allValueDate{ (long)valueDate };
	map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
	for (unsigned long i = 0; i < legCharacteristics.RowsInStructure(); ++i)
	{
		string wf = legCharacteristics(i, 12).StringValue();
		QCHelperFunctions::lowerCase(wf);
		string yf = legCharacteristics(i, 13).StringValue();
		QCHelperFunctions::lowerCase(yf);
		shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
			(double)legCharacteristics(i, 10).NumericValue(), yf, wf);

		long numOp = (long)legCharacteristics(i, 0).NumericValue();
		double x = 0;
		vector<tuple<QCDate, double, double>> amortIfCustom;
		if (legCharacteristics(i, 9).StringValue() == "CUSTOM")
		{
			amortIfCustom = dateNotionalAndAmortByIdLeg.at(
				(long)legCharacteristics(i, 0).NumericValue());
		}
		QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildFixedRateLeg(
			legCharacteristics(i, 1).StringValue(),					 //receive or pay
			QCDate{ (long)legCharacteristics(i, 2).NumericValue() }, //start date
			QCDate{ (long)legCharacteristics(i, 3).NumericValue() }, //end date
			mapHolidays.at(legCharacteristics(i, 4).StringValue()),  //settlement calendar
			(int)legCharacteristics(i, 5).NumericValue(),			 //settlement lag
			QCHelperFunctions::stringToQCStubPeriod(
			legCharacteristics(i, 6).StringValue()),				 //stub period
			legCharacteristics(i, 7).StringValue(),					 //periodicity
			QCHelperFunctions::stringToQCBusDayAdjRule(
			legCharacteristics(i, 8).StringValue()),				 //end date adjustment
			QCHelperFunctions::stringToQCAmortization(
			legCharacteristics(i, 9).StringValue()),					 //amortization
			amortIfCustom,										//amortization and notional by end date
			(double)legCharacteristics(i, 11).NumericValue()		 //notional
			);
		shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
			new QCFixedRatePayoff{tmpIntRate, make_shared<QCInterestRateLeg>(tmpIntRateLeg),
		allCurves.at(legCharacteristics(i, 14).StringValue()), allValueDate, nullptr});
		payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(
			(long)legCharacteristics(i, 0).NumericValue(),
			tmpIntRatePayoff));
	}

	//Calcular los valores presentes
	vector<tuple<long, double, vector<double>>> result;
	result.resize(payoffs.size());
	double m2m;
	vector<double> der;
	unsigned long counter = 0;
	unsigned int longestCurve = 0;
	for (const auto& payoff : payoffs)
	{
		m2m = payoff.second->presentValue();
		unsigned int vertices = payoff.second->discountCurveLength();
		if (vertices > longestCurve)
			longestCurve = vertices;
		der.resize(vertices);
		for (unsigned long i = 0; i < vertices; ++i)
		{
			der.at(i) = payoff.second->getPvRateDerivativeAt(i);
		}
		result.at(counter) = make_tuple(payoff.first, m2m, der);
		der.clear();
		++counter;
	}

	long cuantosM2M = legCharacteristics.RowsInStructure();
	CellMatrix legM2M(cuantosM2M, longestCurve + 2);
	for (long i = 0; i < cuantosM2M; ++i)
	{
		legM2M(i, 0) = get<0>(result.at(i));
		legM2M(i, 1) = get<1>(result.at(i));
		unsigned int vertices = get<2>(result.at(i)).size();
		for (unsigned int j = 0; j < vertices; ++j)
		{
			legM2M(i, j + 2) = get<2>(result.at(i)).at(j) * BASIS_POINT;
		}
	}
	return legM2M;
}

CellMatrix pvFixedLegs2(double valueDate,
	CellMatrix holidays,
	CellMatrix curveValues,
	CellMatrix curveCharacteristics,
	CellMatrix legCharacteristics,
	CellMatrix customAmort)
{
	//holidays: nombre, fecha. Se construye un map<string, vector<QCDate>> que para cada nombre de calendario
	//tenga todas las fechas
	map<string, vector<QCDate>> mapHolidays;
	HelperFunctions::buildHolidays(holidays, mapHolidays);

	//curveValues: nombre, tenor (en dias), valor (tasa o df)
	//Se construye un map<string, pair<vector<long>, vector<double>>> que contiene las curvas
	map<string, pair<vector<long>, vector<double>>> crvValues;
	HelperFunctions::buildCurveValues(curveValues, crvValues);

	//Toca ahora construir las curvas con su interpolador y sus convenciones de tasas
	//Primero metemos las caracteristicas de las curvas en un
	//map<string, tuple<string, string, string, string>> = HelperFunctions::string4
	map<string, HelperFunctions::string4> crvChars;
	HelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

	//Para cada uno de los elementos de crvValues debo terminar de dar de alta una curva cero
	//Tengo los plazos, las tasas y los nombres del interpolador, yf, wf y tipo de curva.
	//Necesito entonces llamar una funcion factory que con estos argumentos me de de alta un curva.
	//las curvas que vaya dando de alta las meto en un map

	map<string, shared_ptr<QCZeroCouponCurve>> allCurves;

	//Loopeo sobre los keys de crvValues
	for (const auto &curva : crvValues)
	{
		vector<long> tmpLng{ curva.second.first };
		vector<double> tmpDbl{ curva.second.second };

		string wf = get<1>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(wf);

		string yf = get<2>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(yf);

		QCZrCpnCrvShrdPtr tmpCrv = QCFactoryFunctions::zrCpnCrvShrdPtr(
			tmpLng,
			tmpDbl,
			get<0>(crvChars.at(curva.first)),
			wf, yf, get<3>(crvChars.at(curva.first)));
		allCurves.insert(pair <string, shared_ptr<QCZeroCouponCurve>>(curva.first, tmpCrv));
	}

	//Guardaremos el CellMatrix customAmort con los datos de nominal vigente y amortizacion en
	//esta estructura.
	map<unsigned long, vector<tuple<QCDate, double, double>>> dateNotionalAndAmortByIdLeg;
	HelperFunctions::buildCustomAmortization(customAmort, dateNotionalAndAmortByIdLeg);

	//Ahora hay que construir QCInterestRatePayoff para cada operacion
	//Se requiere:
	//	shared_ptr<QCInterestRate>
	//	shared_ptr<QCInterestRateLeg>
	//	QCDate (valueDate)
	//	shared_ptr<QCZeroCouponCurve> (la curva de descuento)
	//	shared_ptr<QCTimeSeries> (los fixings)

	//La info esta en la CellMatriz legCharacteristics:
	//	0:	id_leg
	//	1:	rec_pay
	//	2:	start_date
	//	3:	end_date
	//	4:	settlement_calendar
	//	5:	settlement_lag
	//	6:	stub_period
	//	7:	periodicity
	//	8:	end_date_adjustment
	//	9:	amortization
	//	10: fixed_rate
	//	11: notional
	//	12: wealth_factor
	//	13: year_fraction
	//	14: discount_curve

	QCDate allValueDate{ (long)valueDate };
	map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
	for (unsigned long i = 0; i < legCharacteristics.RowsInStructure(); ++i)
	{
		string wf = legCharacteristics(i, 12).StringValue();
		QCHelperFunctions::lowerCase(wf);
		string yf = legCharacteristics(i, 13).StringValue();
		QCHelperFunctions::lowerCase(yf);
		shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
			(double)legCharacteristics(i, 10).NumericValue(), yf, wf);

		long numOp = (long)legCharacteristics(i, 0).NumericValue();
		double x = 0;
		vector<tuple<QCDate, double, double>> amortIfCustom;
		if (legCharacteristics(i, 9).StringValue() != "BULLET")
		{
			amortIfCustom = dateNotionalAndAmortByIdLeg.at(
				(long)legCharacteristics(i, 0).NumericValue());
		}
		QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildFixedRateLeg2(
			legCharacteristics(i, 1).StringValue(),					 //receive or pay
			QCDate{ (long)legCharacteristics(i, 2).NumericValue() }, //start date
			QCDate{ (long)legCharacteristics(i, 3).NumericValue() }, //end date
			mapHolidays.at(legCharacteristics(i, 4).StringValue()),  //settlement calendar
			(int)legCharacteristics(i, 5).NumericValue(),			 //settlement lag
			QCHelperFunctions::stringToQCStubPeriod(
			legCharacteristics(i, 6).StringValue()),				 //stub period
			legCharacteristics(i, 7).StringValue(),					 //periodicity
			QCHelperFunctions::stringToQCBusDayAdjRule(
			legCharacteristics(i, 8).StringValue()),				 //end date adjustment
			QCHelperFunctions::stringToQCAmortization(
			legCharacteristics(i, 9).StringValue()),					 //amortization
			amortIfCustom,										//amortization and notional by end date
			(double)legCharacteristics(i, 11).NumericValue()		 //notional
			);
		shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
			new QCFixedRatePayoff{ tmpIntRate, make_shared<QCInterestRateLeg>(tmpIntRateLeg),
			allCurves.at(legCharacteristics(i, 14).StringValue()), allValueDate, nullptr });
		payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(
			(long)legCharacteristics(i, 0).NumericValue(),
			tmpIntRatePayoff));
	}

	//Calcular los valores presentes
	vector<tuple<long, double, vector<double>>> result;
	result.resize(payoffs.size());
	double m2m;
	vector<double> der;
	unsigned long counter = 0;
	unsigned int longestCurve = 0;
	for (const auto& payoff : payoffs)
	{
		m2m = payoff.second->presentValue();
		unsigned int vertices = payoff.second->discountCurveLength();
		if (vertices > longestCurve)
			longestCurve = vertices;
		der.resize(vertices);
		for (unsigned long i = 0; i < vertices; ++i)
		{
			der.at(i) = payoff.second->getPvRateDerivativeAt(i);
		}
		result.at(counter) = make_tuple(payoff.first, m2m, der);
		der.clear();
		++counter;
	}

	long cuantosM2M = legCharacteristics.RowsInStructure();
	CellMatrix legM2M(cuantosM2M, longestCurve + 2);
	for (long i = 0; i < cuantosM2M; ++i)
	{
		legM2M(i, 0) = get<0>(result.at(i));
		legM2M(i, 1) = get<1>(result.at(i));
		unsigned int vertices = get<2>(result.at(i)).size();
		for (unsigned int j = 0; j < vertices; ++j)
		{
			legM2M(i, j + 2) = get<2>(result.at(i)).at(j) * BASIS_POINT;
		}
	}
	return legM2M;
}

CellMatrix pvIcpClpLegs(double valueDate,
	CellMatrix holidays,
	CellMatrix curveValues,
	CellMatrix curveCharacteristics,
	CellMatrix legCharacteristics,
	CellMatrix customAmort,
	CellMatrix fixings)
{
	//holidays: nombre, fecha. Se construye un map<string, vector<QCDate>> que para cada nombre de calendario
	//tenga todas las fechas
	map<string, vector<QCDate>> mapHolidays;
	HelperFunctions::buildHolidays(holidays, mapHolidays);

	//curveValues: nombre, tenor (en dias), valor (tasa o df)
	//Se construye un map<string, pair<vector<long>, vector<double>>> que contiene las curvas
	map<string, pair<vector<long>, vector<double>>> crvValues;
	HelperFunctions::buildCurveValues(curveValues, crvValues);

	//Toca ahora construir las curvas con su interpolador y sus convenciones de tasas
	//Primero metemos las caracteristicas de las curvas en un
	//map<string, tuple<string, string, string, string>> = HelperFunctions::string4
	map<string, HelperFunctions::string4> crvChars;
	HelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

	//Para cada uno de los elementos de crvValues debo terminar de dar de alta una curva cero
	//Tengo los plazos, las tasas y los nombres del interpolador, yf, wf y tipo de curva.
	//Necesito entonces llamar una funcion factory que con estos argumentos me de de alta un curva.
	//las curvas que vaya dando de alta las meto en un map

	map<string, shared_ptr<QCInterestRateCurve>> allCurves;

	//Loopeo sobre los keys de crvValues
	for (const auto &curva : crvValues)
	{
		vector<long> tmpLng{ curva.second.first };
		vector<double> tmpDbl{ curva.second.second };

		string wf = get<1>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(wf);

		string yf = get<2>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(yf);

		//Tengo que mirar esta funcion
		shared_ptr<QCInterestRateCurve> tmpCrv = QCFactoryFunctions::intRtCrvShrdPtr(
			tmpLng,
			tmpDbl,
			get<0>(crvChars.at(curva.first)),
			wf, yf, QCHelperFunctions::stringToQCIntRateCurve(get<3>(crvChars.at(curva.first))));
		allCurves.insert(pair<string, shared_ptr<QCInterestRateCurve>>(curva.first, tmpCrv));
	}

	//Guardaremos el CellMatrix customAmort con los datos de nominal vigente y amortizacion en
	//esta estructura.
	map<unsigned long, vector<tuple<QCDate, double, double>>> dateNotionalAndAmortByIdLeg;
	HelperFunctions::buildCustomAmortization(customAmort, dateNotionalAndAmortByIdLeg);

	//Metemos los fixings de ICP en esta estructura
	map<QCDate, double> mapFixings;
	HelperFunctions::buildFixings(fixings, mapFixings);

	//Ahora hay que construir QCInterestRatePayoff para cada operacion
	//Se requiere:
	//	shared_ptr<QCInterestRate>
	//	shared_ptr<QCInterestRateLeg>
	//	QCDate (valueDate)
	//	shared_ptr<QCZeroCouponCurve> (la curva de proyeccion)
	//	shared_ptr<QCZeroCouponCurve> (la curva de descuento)
	//	shared_ptr<QCTimeSeries> (los fixings)

	//La info esta en la CellMatrix legCharacteristics:
	//0:	id_leg					long
	//1:	rec_pay					string
	//2:	start_date				QCDate
	//3:	end_date				QCDate
	//4:	settlement_calendar		string
	//5:	settlement_lag			int
	//6:	stub_period				string
	//7:	periodicity				string
	//8:	end_date_adjustment		string
	//9:	amortization			string
	//10:	interest_rate_index		string
	//11:	rate_value				double
	//12:	spread					double
	//13:	notional_currency		string
	//14:	notional				double
	//15:	wealth_factor			string
	//16:	year_fraction			string
	//17:	projecting_curve		string
	//18:	discount_curve			string
	//19:	fixing_calendar			string

	QCDate allValueDate{ (long)valueDate };
	map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
	for (unsigned long i = 0; i < legCharacteristics.RowsInStructure(); ++i)
	{
		string wf = legCharacteristics(i, 15).StringValue();
		QCHelperFunctions::lowerCase(wf);
		string yf = legCharacteristics(i, 16).StringValue();
		QCHelperFunctions::lowerCase(yf);
		shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
			(double)legCharacteristics(i, 11).NumericValue(), yf, wf);

		long numOp = (long)legCharacteristics(i, 0).NumericValue();
		vector<tuple<QCDate, double, double>> amortIfCustom;
		if (legCharacteristics(i, 9).StringValue() == "CUSTOM")
		{
			amortIfCustom = dateNotionalAndAmortByIdLeg.at(
				(long)legCharacteristics(i, 0).NumericValue());
		}
		QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildIcpLeg(
			legCharacteristics(i, 1).StringValue(),					 //receive or pay
			QCDate{ (long)legCharacteristics(i, 2).NumericValue() }, //start date
			QCDate{ (long)legCharacteristics(i, 3).NumericValue() }, //end date
			mapHolidays.at(legCharacteristics(i, 4).StringValue()),  //settlement calendar
			(int)legCharacteristics(i, 5).NumericValue(),			 //settlement lag
			QCHelperFunctions::stringToQCStubPeriod(
			legCharacteristics(i, 6).StringValue()),					 //stub period
			legCharacteristics(i, 7).StringValue(),					 //periodicity
			QCHelperFunctions::stringToQCBusDayAdjRule(
			legCharacteristics(i, 8).StringValue()),				 //end date adjustment
			QCHelperFunctions::stringToQCAmortization(
			legCharacteristics(i, 9).StringValue()),					 //amortization
			amortIfCustom,											 //amortization and notional by end date
			(double)legCharacteristics(i, 14).NumericValue()		 //notional
			);
		shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
			new QCIcpClpPayoff{ tmpIntRate, legCharacteristics(i, 12).NumericValue(), 1.0,
			make_shared<QCInterestRateLeg>(tmpIntRateLeg), allCurves.at(legCharacteristics(i, 17).StringValue()),
			allCurves.at(legCharacteristics(i, 18).StringValue()), allValueDate,
			make_shared<map<QCDate, double>>(mapFixings) });
		payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(
			(long)legCharacteristics(i, 0).NumericValue(),
			tmpIntRatePayoff));
	}

	//Calcular los valores presentes
	vector<tuple<long, double, vector<double>, vector<double>>> result;
	result.resize(payoffs.size());
	double m2m;
	vector<double> discDer;
	vector<double> projDer;
	unsigned long counter = 0;
	unsigned int longestDiscCurve = 0;
	unsigned int longestProjCurve = 0;
	for (const auto& payoff : payoffs)
	{
		m2m = payoff.second->presentValue();
		unsigned int discVertices = payoff.second->discountCurveLength();
		if (discVertices > longestDiscCurve)
			longestDiscCurve = discVertices;
		discDer.resize(discVertices);
		for (unsigned long i = 0; i < discVertices; ++i)
		{
			discDer.at(i) = payoff.second->getPvRateDerivativeAt(i);
		}

		unsigned int projVertices = payoff.second->projectingCurveLength();
		if (projVertices > longestProjCurve)
			longestProjCurve = projVertices;
		projDer.resize(projVertices);
		for (unsigned long i = 0; i < projVertices; ++i)
		{
			projDer.at(i) = payoff.second->getPvProjRateDerivativeAt(i);
		}
		result.at(counter) = make_tuple(payoff.first, m2m, discDer, projDer);
		++counter;
	}

	long cuantosResultados = legCharacteristics.RowsInStructure();
	CellMatrix legM2MAndDelta(cuantosResultados, longestDiscCurve + longestProjCurve + 2);
	for (long i = 0; i < cuantosResultados; ++i)
	{
		legM2MAndDelta(i, 0) = get<0>(result.at(i));
		legM2MAndDelta(i, 1) = get<1>(result.at(i));
		unsigned int discVertices = get<2>(result.at(i)).size();
		for (unsigned int j = 0; j < discVertices; ++j)
		{
			legM2MAndDelta(i, j + 2) = get<2>(result.at(i)).at(j) * BASIS_POINT;
		}
		unsigned int projVertices = get<3>(result.at(i)).size();
		for (unsigned int j = 0; j < projVertices; ++j)
		{
			legM2MAndDelta(i, j + discVertices + 2) = get<3>(result.at(i)).at(j) * BASIS_POINT;
		}
	}
	return legM2MAndDelta;
}

CellMatrix pvIcpClpLegs2(double valueDate,
	CellMatrix holidays,
	CellMatrix curveValues,
	CellMatrix curveCharacteristics,
	CellMatrix legCharacteristics,
	CellMatrix customAmort,
	CellMatrix fixings)
{
	//holidays: nombre, fecha. Se construye un map<string, vector<QCDate>> que para cada nombre de calendario
	//tenga todas las fechas
	map<string, vector<QCDate>> mapHolidays;
	HelperFunctions::buildHolidays(holidays, mapHolidays);

	//curveValues: nombre, tenor (en dias), valor (tasa o df)
	//Se construye un map<string, pair<vector<long>, vector<double>>> que contiene las curvas
	map<string, pair<vector<long>, vector<double>>> crvValues;
	HelperFunctions::buildCurveValues(curveValues, crvValues);

	//Toca ahora construir las curvas con su interpolador y sus convenciones de tasas
	//Primero metemos las caracteristicas de las curvas en un
	//map<string, tuple<string, string, string, string>> = HelperFunctions::string4
	map<string, HelperFunctions::string4> crvChars;
	HelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

	//Para cada uno de los elementos de crvValues debo terminar de dar de alta una curva cero
	//Tengo los plazos, las tasas y los nombres del interpolador, yf, wf y tipo de curva.
	//Necesito entonces llamar una funcion factory que con estos argumentos me de de alta un curva.
	//las curvas que vaya dando de alta las meto en un map

	map<string, shared_ptr<QCInterestRateCurve>> allCurves;

	//Loopeo sobre los keys de crvValues
	for (const auto &curva : crvValues)
	{
		vector<long> tmpLng{ curva.second.first };
		vector<double> tmpDbl{ curva.second.second };

		string wf = get<1>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(wf);

		string yf = get<2>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(yf);

		//Tengo que mirar esta funcion
		shared_ptr<QCInterestRateCurve> tmpCrv = QCFactoryFunctions::intRtCrvShrdPtr(
			tmpLng,
			tmpDbl,
			get<0>(crvChars.at(curva.first)),
			wf, yf, QCHelperFunctions::stringToQCIntRateCurve(get<3>(crvChars.at(curva.first))));
		allCurves.insert(pair<string, shared_ptr<QCInterestRateCurve>>(curva.first, tmpCrv));
	}

	//Guardaremos el CellMatrix customAmort con los datos de nominal vigente y amortizacion en
	//esta estructura.
	map<unsigned long, vector<tuple<QCDate, double, double>>> dateNotionalAndAmortByIdLeg;
	HelperFunctions::buildCustomAmortization(customAmort, dateNotionalAndAmortByIdLeg);

	//Metemos los fixings de ICP en esta estructura
	map<QCDate, double> mapFixings;
	HelperFunctions::buildFixings(fixings, mapFixings);

	//Ahora hay que construir QCInterestRatePayoff para cada operacion
	//Se requiere:
	//	shared_ptr<QCInterestRate>
	//	shared_ptr<QCInterestRateLeg>
	//	QCDate (valueDate)
	//	shared_ptr<QCZeroCouponCurve> (la curva de proyeccion)
	//	shared_ptr<QCZeroCouponCurve> (la curva de descuento)
	//	shared_ptr<QCTimeSeries> (los fixings)

	//La info esta en la CellMatrix legCharacteristics:
	//0:	id_leg					long
	//1:	rec_pay					string
	//2:	start_date				QCDate
	//3:	end_date				QCDate
	//4:	settlement_calendar		string
	//5:	settlement_lag			int
	//6:	stub_period				string
	//7:	periodicity				string
	//8:	end_date_adjustment		string
	//9:	amortization			string
	//10:	interest_rate_index		string
	//11:	rate_value				double
	//12:	spread					double
	//13:	notional_currency		string
	//14:	notional				double
	//15:	wealth_factor			string
	//16:	year_fraction			string
	//17:	projecting_curve		string
	//18:	discount_curve			string
	//19:	fixing_calendar			string

	QCDate allValueDate{ (long)valueDate };
	map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
	for (unsigned long i = 0; i < legCharacteristics.RowsInStructure(); ++i)
	{
		string wf = legCharacteristics(i, 15).StringValue();
		QCHelperFunctions::lowerCase(wf);
		string yf = legCharacteristics(i, 16).StringValue();
		QCHelperFunctions::lowerCase(yf);
		shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
			(double)legCharacteristics(i, 11).NumericValue(), yf, wf);

		long numOp = (long)legCharacteristics(i, 0).NumericValue();
		vector<tuple<QCDate, double, double>> amortIfCustom;
		if (legCharacteristics(i, 9).StringValue() != "BULLET")
		{
			amortIfCustom = dateNotionalAndAmortByIdLeg.at(
				(long)legCharacteristics(i, 0).NumericValue());
		}
		QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildIcpLeg2(
			legCharacteristics(i, 1).StringValue(),					 //receive or pay
			QCDate{ (long)legCharacteristics(i, 2).NumericValue() }, //start date
			QCDate{ (long)legCharacteristics(i, 3).NumericValue() }, //end date
			mapHolidays.at(legCharacteristics(i, 4).StringValue()),  //settlement calendar
			(int)legCharacteristics(i, 5).NumericValue(),			 //settlement lag
			QCHelperFunctions::stringToQCStubPeriod(
			legCharacteristics(i, 6).StringValue()),					 //stub period
			legCharacteristics(i, 7).StringValue(),					 //periodicity
			QCHelperFunctions::stringToQCBusDayAdjRule(
			legCharacteristics(i, 8).StringValue()),				 //end date adjustment
			QCHelperFunctions::stringToQCAmortization(
			legCharacteristics(i, 9).StringValue()),					 //amortization
			amortIfCustom,											 //amortization and notional by end date
			(double)legCharacteristics(i, 14).NumericValue()		 //notional
			);
		shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
			new QCIcpClpPayoff{ tmpIntRate, legCharacteristics(i, 12).NumericValue(), 1.0,
			make_shared<QCInterestRateLeg>(tmpIntRateLeg), allCurves.at(legCharacteristics(i, 17).StringValue()),
			allCurves.at(legCharacteristics(i, 18).StringValue()), allValueDate,
			make_shared<map<QCDate, double>>(mapFixings) });
		payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(
			(long)legCharacteristics(i, 0).NumericValue(),
			tmpIntRatePayoff));
	}

	//Calcular los valores presentes
	vector<tuple<long, double, vector<double>, vector<double>>> result;
	result.resize(payoffs.size());
	double m2m;
	vector<double> discDer;
	vector<double> projDer;
	unsigned long counter = 0;
	unsigned int longestDiscCurve = 0;
	unsigned int longestProjCurve = 0;
	for (const auto& payoff : payoffs)
	{
		m2m = payoff.second->presentValue();
		unsigned int discVertices = payoff.second->discountCurveLength();
		if (discVertices > longestDiscCurve)
			longestDiscCurve = discVertices;
		discDer.resize(discVertices);
		for (unsigned long i = 0; i < discVertices; ++i)
		{
			discDer.at(i) = payoff.second->getPvRateDerivativeAt(i);
		}

		unsigned int projVertices = payoff.second->projectingCurveLength();
		if (projVertices > longestProjCurve)
			longestProjCurve = projVertices;
		projDer.resize(projVertices);
		for (unsigned long i = 0; i < projVertices; ++i)
		{
			projDer.at(i) = payoff.second->getPvProjRateDerivativeAt(i);
		}
		result.at(counter) = make_tuple(payoff.first, m2m, discDer, projDer);
		++counter;
	}

	long cuantosResultados = legCharacteristics.RowsInStructure();
	CellMatrix legM2MAndDelta(cuantosResultados, longestDiscCurve + longestProjCurve + 2);
	for (long i = 0; i < cuantosResultados; ++i)
	{
		legM2MAndDelta(i, 0) = get<0>(result.at(i));
		legM2MAndDelta(i, 1) = get<1>(result.at(i));
		unsigned int discVertices = get<2>(result.at(i)).size();
		for (unsigned int j = 0; j < discVertices; ++j)
		{
			legM2MAndDelta(i, j + 2) = get<2>(result.at(i)).at(j) * BASIS_POINT;
		}
		unsigned int projVertices = get<3>(result.at(i)).size();
		for (unsigned int j = 0; j < projVertices; ++j)
		{
			legM2MAndDelta(i, j + discVertices + 2) = get<3>(result.at(i)).at(j) * BASIS_POINT;
		}
	}
	return legM2MAndDelta;
}

CellMatrix pvIcpClfLegs(double valueDate,
	CellMatrix holidays,
	CellMatrix curveValues,
	CellMatrix curveCharacteristics,
	CellMatrix legCharacteristics,
	CellMatrix customAmort,
	CellMatrix icpFixings,
	CellMatrix ufFixings)
{
	//holidays: nombre, fecha. Se construye un map<string, vector<QCDate>> que para cada nombre de calendario
	//tenga todas las fechas
	map<string, vector<QCDate>> mapHolidays;
	HelperFunctions::buildHolidays(holidays, mapHolidays);

	//curveValues: nombre, tenor (en dias), valor (tasa o df)
	//Se construye un map<string, pair<vector<long>, vector<double>>> que contiene las curvas
	map<string, pair<vector<long>, vector<double>>> crvValues;
	HelperFunctions::buildCurveValues(curveValues, crvValues);

	//Toca ahora construir las curvas con su interpolador y sus convenciones de tasas
	//Primero metemos las caracteristicas de las curvas en un
	//map<string, tuple<string, string, string, string>> = HelperFunctions::string4
	map<string, HelperFunctions::string4> crvChars;
	HelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

	//Para cada uno de los elementos de crvValues debo terminar de dar de alta una curva cero
	//Tengo los plazos, las tasas y los nombres del interpolador, yf, wf y tipo de curva.
	//Necesito entonces llamar una funcion factory que con estos argumentos me de de alta un curva.
	//las curvas que vaya dando de alta las meto en un map

	map<string, shared_ptr<QCInterestRateCurve>> allCurves;

	//Loopeo sobre los keys de crvValues
	for (const auto &curva : crvValues)
	{
		vector<long> tmpLng{ curva.second.first };
		vector<double> tmpDbl{ curva.second.second };

		string wf = get<1>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(wf);

		string yf = get<2>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(yf);

		//Tengo que mirar esta funcion
		shared_ptr<QCInterestRateCurve> tmpCrv = QCFactoryFunctions::intRtCrvShrdPtr(
			tmpLng,
			tmpDbl,
			get<0>(crvChars.at(curva.first)),
			wf, yf, QCHelperFunctions::stringToQCIntRateCurve(get<3>(crvChars.at(curva.first))));
		allCurves.insert(pair<string, shared_ptr<QCInterestRateCurve>>(curva.first, tmpCrv));
	}

	//Guardaremos el CellMatrix customAmort con los datos de nominal vigente y amortizacion en
	//esta estructura.
	map<unsigned long, vector<tuple<QCDate, double, double>>> dateNotionalAndAmortByIdLeg;
	HelperFunctions::buildCustomAmortization(customAmort, dateNotionalAndAmortByIdLeg);

	//Metemos los fixings de ICP en esta estructura
	map<QCDate, double> mapIcpFixings;
	HelperFunctions::buildFixings(icpFixings, mapIcpFixings);

	//Metemos los fixings de UF en esta estructura
	map<QCDate, double> mapUfFixings;
	HelperFunctions::buildFixings(ufFixings, mapUfFixings);

	//Ahora hay que construir QCInterestRatePayoff para cada operacion
	//Se requiere:
	//	shared_ptr<QCInterestRate>
	//	shared_ptr<QCInterestRateLeg>
	//	QCDate (valueDate)
	//	shared_ptr<QCZeroCouponCurve> (la curva de proyeccion)
	//	shared_ptr<QCZeroCouponCurve> (la curva de descuento)
	//	shared_ptr<QCTimeSeries> (los fixings)

	//La info esta en la CellMatrix legCharacteristics:
	//0:	id_leg					long
	//1:	rec_pay					string
	//2:	start_date				QCDate
	//3:	end_date				QCDate
	//4:	settlement_calendar		string
	//5:	settlement_lag			int
	//6:	stub_period				string
	//7:	periodicity				string
	//8:	end_date_adjustment		string
	//9:	amortization			string
	//10:	interest_rate_index		string
	//11:	rate_value				double
	//12:	spread					double
	//13:	notional_currency		string
	//14:	notional				double
	//15:	wealth_factor			string
	//16:	year_fraction			string
	//17:	projecting_curve		string
	//18:	discount_curve			string
	//19:	fixing_calendar			string

	QCDate allValueDate{ (long)valueDate };
	map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
	for (unsigned long i = 0; i < legCharacteristics.RowsInStructure(); ++i)
	{
		string wf = legCharacteristics(i, 15).StringValue();
		QCHelperFunctions::lowerCase(wf);
		string yf = legCharacteristics(i, 16).StringValue();
		QCHelperFunctions::lowerCase(yf);
		shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
			(double)legCharacteristics(i, 11).NumericValue(), yf, wf);

		long numOp = (long)legCharacteristics(i, 0).NumericValue();
		vector<tuple<QCDate, double, double>> amortIfCustom;
		if (legCharacteristics(i, 9).StringValue() == "CUSTOM")
		{
			amortIfCustom = dateNotionalAndAmortByIdLeg.at(
				(long)legCharacteristics(i, 0).NumericValue());
		}
		QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildIcpLeg(
			legCharacteristics(i, 1).StringValue(),						//receive or pay
			QCDate{ (long)legCharacteristics(i, 2).NumericValue() },	//start date
			QCDate{ (long)legCharacteristics(i, 3).NumericValue() },	//end date
			mapHolidays.at(legCharacteristics(i, 4).StringValue()),		//settlement calendar
			(int)legCharacteristics(i, 5).NumericValue(),		//settlement lag
			QCHelperFunctions::stringToQCStubPeriod(
			legCharacteristics(i, 6).StringValue()),			//stub period
			legCharacteristics(i, 7).StringValue(),				//periodicity
			QCHelperFunctions::stringToQCBusDayAdjRule(
			legCharacteristics(i, 8).StringValue()),			//end date adjustment
			QCHelperFunctions::stringToQCAmortization(
			legCharacteristics(i, 9).StringValue()),			//amortization
			amortIfCustom,										//amortization and notional by end date
			(double)legCharacteristics(i, 14).NumericValue()	//notional
			);
		shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
			new QCIcpClfPayoff{ tmpIntRate, legCharacteristics(i, 12).NumericValue(), 1.0,
			make_shared<QCInterestRateLeg>(tmpIntRateLeg), allCurves.at(legCharacteristics(i, 17).StringValue()),
			allCurves.at(legCharacteristics(i, 18).StringValue()), allValueDate,
			make_shared<map<QCDate, double>>(mapIcpFixings),
			make_shared<map<QCDate, double>>(mapUfFixings) });
		payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(
			(long)legCharacteristics(i, 0).NumericValue(),
			tmpIntRatePayoff));
	}

	//Calcular los valores presentes
	vector<tuple<long, double, vector<double>, vector<double>>> result;
	result.resize(payoffs.size());
	double m2m;
	vector<double> discDer;
	vector<double> projDer;
	unsigned long counter = 0;
	unsigned int longestDiscCurve = 0;
	unsigned int longestProjCurve = 0;
	for (const auto& payoff : payoffs)
	{
		m2m = payoff.second->presentValue();
		unsigned int discVertices = payoff.second->discountCurveLength();
		if (discVertices > longestDiscCurve)
			longestDiscCurve = discVertices;
		discDer.resize(discVertices);
		for (unsigned long i = 0; i < discVertices; ++i)
		{
			discDer.at(i) = payoff.second->getPvRateDerivativeAt(i);
		}

		unsigned int projVertices = payoff.second->projectingCurveLength();
		if (projVertices > longestProjCurve)
			longestProjCurve = projVertices;
		projDer.resize(projVertices);
		for (unsigned long i = 0; i < projVertices; ++i)
		{
			projDer.at(i) = payoff.second->getPvProjRateDerivativeAt(i);
		}
		result.at(counter) = make_tuple(payoff.first, m2m, discDer, projDer);
		++counter;
	}

	long cuantosResultados = legCharacteristics.RowsInStructure();
	CellMatrix legM2MAndDelta(cuantosResultados, longestDiscCurve + longestProjCurve + 2);
	for (long i = 0; i < cuantosResultados; ++i)
	{
		legM2MAndDelta(i, 0) = get<0>(result.at(i));
		legM2MAndDelta(i, 1) = get<1>(result.at(i));
		unsigned int discVertices = get<2>(result.at(i)).size();
		for (unsigned int j = 0; j < discVertices; ++j)
		{
			legM2MAndDelta(i, j + 2) = get<2>(result.at(i)).at(j) * BASIS_POINT;
		}
		unsigned int projVertices = get<3>(result.at(i)).size();
		for (unsigned int j = 0; j < projVertices; ++j)
		{
			legM2MAndDelta(i, j + discVertices + 2) = get<3>(result.at(i)).at(j) * BASIS_POINT;
		}
	}
	return legM2MAndDelta;
}

CellMatrix pvIcpClfLegs2(double valueDate,
	CellMatrix holidays,
	CellMatrix curveValues,
	CellMatrix curveCharacteristics,
	CellMatrix legCharacteristics,
	CellMatrix customAmort,
	CellMatrix icpFixings,
	CellMatrix ufFixings)
{
	//holidays: nombre, fecha. Se construye un map<string, vector<QCDate>> que para cada nombre de calendario
	//tenga todas las fechas
	map<string, vector<QCDate>> mapHolidays;
	HelperFunctions::buildHolidays(holidays, mapHolidays);

	//curveValues: nombre, tenor (en dias), valor (tasa o df)
	//Se construye un map<string, pair<vector<long>, vector<double>>> que contiene las curvas
	map<string, pair<vector<long>, vector<double>>> crvValues;
	HelperFunctions::buildCurveValues(curveValues, crvValues);

	//Toca ahora construir las curvas con su interpolador y sus convenciones de tasas
	//Primero metemos las caracteristicas de las curvas en un
	//map<string, tuple<string, string, string, string>> = HelperFunctions::string4
	map<string, HelperFunctions::string4> crvChars;
	HelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

	//Para cada uno de los elementos de crvValues debo terminar de dar de alta una curva cero
	//Tengo los plazos, las tasas y los nombres del interpolador, yf, wf y tipo de curva.
	//Necesito entonces llamar una funcion factory que con estos argumentos me de de alta un curva.
	//las curvas que vaya dando de alta las meto en un map

	map<string, shared_ptr<QCInterestRateCurve>> allCurves;

	//Loopeo sobre los keys de crvValues
	for (const auto &curva : crvValues)
	{
		vector<long> tmpLng{ curva.second.first };
		vector<double> tmpDbl{ curva.second.second };

		string wf = get<1>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(wf);

		string yf = get<2>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(yf);

		//Tengo que mirar esta funcion
		shared_ptr<QCInterestRateCurve> tmpCrv = QCFactoryFunctions::intRtCrvShrdPtr(
			tmpLng,
			tmpDbl,
			get<0>(crvChars.at(curva.first)),
			wf, yf, QCHelperFunctions::stringToQCIntRateCurve(get<3>(crvChars.at(curva.first))));
		allCurves.insert(pair<string, shared_ptr<QCInterestRateCurve>>(curva.first, tmpCrv));
	}

	//Guardaremos el CellMatrix customAmort con los datos de nominal vigente y amortizacion en
	//esta estructura.
	map<unsigned long, vector<tuple<QCDate, double, double>>> dateNotionalAndAmortByIdLeg;
	HelperFunctions::buildCustomAmortization(customAmort, dateNotionalAndAmortByIdLeg);

	//Metemos los fixings de ICP en esta estructura
	map<QCDate, double> mapIcpFixings;
	HelperFunctions::buildFixings(icpFixings, mapIcpFixings);

	//Metemos los fixings de UF en esta estructura
	map<QCDate, double> mapUfFixings;
	HelperFunctions::buildFixings(ufFixings, mapUfFixings);

	//Ahora hay que construir QCInterestRatePayoff para cada operacion
	//Se requiere:
	//	shared_ptr<QCInterestRate>
	//	shared_ptr<QCInterestRateLeg>
	//	QCDate (valueDate)
	//	shared_ptr<QCZeroCouponCurve> (la curva de proyeccion)
	//	shared_ptr<QCZeroCouponCurve> (la curva de descuento)
	//	shared_ptr<QCTimeSeries> (los fixings)

	//La info esta en la CellMatrix legCharacteristics:
	//0:	id_leg					long
	//1:	rec_pay					string
	//2:	start_date				QCDate
	//3:	end_date				QCDate
	//4:	settlement_calendar		string
	//5:	settlement_lag			int
	//6:	stub_period				string
	//7:	periodicity				string
	//8:	end_date_adjustment		string
	//9:	amortization			string
	//10:	interest_rate_index		string
	//11:	rate_value				double
	//12:	spread					double
	//13:	notional_currency		string
	//14:	notional				double
	//15:	wealth_factor			string
	//16:	year_fraction			string
	//17:	projecting_curve		string
	//18:	discount_curve			string
	//19:	fixing_calendar			string

	QCDate allValueDate{ (long)valueDate };
	map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
	for (unsigned long i = 0; i < legCharacteristics.RowsInStructure(); ++i)
	{
		string wf = legCharacteristics(i, 15).StringValue();
		QCHelperFunctions::lowerCase(wf);
		string yf = legCharacteristics(i, 16).StringValue();
		QCHelperFunctions::lowerCase(yf);
		shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
			(double)legCharacteristics(i, 11).NumericValue(), yf, wf);

		long numOp = (long)legCharacteristics(i, 0).NumericValue();
		vector<tuple<QCDate, double, double>> amortIfCustom;
		if (legCharacteristics(i, 9).StringValue() != "BULLET")
		{
			amortIfCustom = dateNotionalAndAmortByIdLeg.at(
				(long)legCharacteristics(i, 0).NumericValue());
		}
		QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildIcpLeg2(
			legCharacteristics(i, 1).StringValue(),						//receive or pay
			QCDate{ (long)legCharacteristics(i, 2).NumericValue() },	//start date
			QCDate{ (long)legCharacteristics(i, 3).NumericValue() },	//end date
			mapHolidays.at(legCharacteristics(i, 4).StringValue()),		//settlement calendar
			(int)legCharacteristics(i, 5).NumericValue(),		//settlement lag
			QCHelperFunctions::stringToQCStubPeriod(
			legCharacteristics(i, 6).StringValue()),			//stub period
			legCharacteristics(i, 7).StringValue(),				//periodicity
			QCHelperFunctions::stringToQCBusDayAdjRule(
			legCharacteristics(i, 8).StringValue()),			//end date adjustment
			QCHelperFunctions::stringToQCAmortization(
			legCharacteristics(i, 9).StringValue()),			//amortization
			amortIfCustom,										//amortization and notional by end date
			(double)legCharacteristics(i, 14).NumericValue()	//notional
			);
		shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
			new QCIcpClfPayoff{ tmpIntRate, legCharacteristics(i, 12).NumericValue(), 1.0,
			make_shared<QCInterestRateLeg>(tmpIntRateLeg), allCurves.at(legCharacteristics(i, 17).StringValue()),
			allCurves.at(legCharacteristics(i, 18).StringValue()), allValueDate,
			make_shared<map<QCDate, double>>(mapIcpFixings),
			make_shared<map<QCDate, double>>(mapUfFixings) });
		payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(
			(long)legCharacteristics(i, 0).NumericValue(),
			tmpIntRatePayoff));
	}

	//Calcular los valores presentes
	vector<tuple<long, double, vector<double>, vector<double>>> result;
	result.resize(payoffs.size());
	double m2m;
	vector<double> discDer;
	vector<double> projDer;
	unsigned long counter = 0;
	unsigned int longestDiscCurve = 0;
	unsigned int longestProjCurve = 0;
	for (const auto& payoff : payoffs)
	{
		m2m = payoff.second->presentValue();
		unsigned int discVertices = payoff.second->discountCurveLength();
		if (discVertices > longestDiscCurve)
			longestDiscCurve = discVertices;
		discDer.resize(discVertices);
		for (unsigned long i = 0; i < discVertices; ++i)
		{
			discDer.at(i) = payoff.second->getPvRateDerivativeAt(i);
		}

		unsigned int projVertices = payoff.second->projectingCurveLength();
		if (projVertices > longestProjCurve)
			longestProjCurve = projVertices;
		projDer.resize(projVertices);
		for (unsigned long i = 0; i < projVertices; ++i)
		{
			projDer.at(i) = payoff.second->getPvProjRateDerivativeAt(i);
		}
		result.at(counter) = make_tuple(payoff.first, m2m, discDer, projDer);
		++counter;
	}

	long cuantosResultados = legCharacteristics.RowsInStructure();
	CellMatrix legM2MAndDelta(cuantosResultados, longestDiscCurve + longestProjCurve + 2);
	for (long i = 0; i < cuantosResultados; ++i)
	{
		legM2MAndDelta(i, 0) = get<0>(result.at(i));
		legM2MAndDelta(i, 1) = get<1>(result.at(i));
		unsigned int discVertices = get<2>(result.at(i)).size();
		for (unsigned int j = 0; j < discVertices; ++j)
		{
			legM2MAndDelta(i, j + 2) = get<2>(result.at(i)).at(j) * BASIS_POINT;
		}
		unsigned int projVertices = get<3>(result.at(i)).size();
		for (unsigned int j = 0; j < projVertices; ++j)
		{
			legM2MAndDelta(i, j + discVertices + 2) = get<3>(result.at(i)).at(j) * BASIS_POINT;
		}
	}
	return legM2MAndDelta;
}

CellMatrix pvFloatingRateLegs(double valueDate,
	CellMatrix holidays,
	CellMatrix curveValues,
	CellMatrix curveCharacteristics,
	CellMatrix legCharacteristics,
	CellMatrix customAmort,
	CellMatrix fixings,
	CellMatrix intRateIndexChars)
{
	//holidays: nombre, fecha. Se construye un map<string, vector<QCDate>> que para cada nombre de calendario
	//tenga todas las fechas
	map<string, vector<QCDate>> mapHolidays;
	HelperFunctions::buildHolidays(holidays, mapHolidays);

	//curveValues: nombre, tenor (en dias), valor (tasa o df)
	//Se construye un map<string, pair<vector<long>, vector<double>>> que contiene las curvas
	map<string, pair<vector<long>, vector<double>>> crvValues;
	HelperFunctions::buildCurveValues(curveValues, crvValues);

	//Toca ahora construir las curvas con su interpolador y sus convenciones de tasas
	//Primero metemos las caracteristicas de las curvas en un
	//map<string, tuple<string, string, string, string>> = HelperFunctions::string4
	map<string, HelperFunctions::string4> crvChars;
	HelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

	//Para cada uno de los elementos de crvValues debo terminar de dar de alta una curva cero
	//Tengo los plazos, las tasas y los nombres del interpolador, yf, wf y tipo de curva.
	//Necesito entonces llamar una funcion factory que con estos argumentos me de de alta un curva.
	//las curvas que vaya dando de alta las meto en un map

	map<string, shared_ptr<QCInterestRateCurve>> allCurves;

	//Loopeo sobre los keys de crvValues
	for (const auto &curva : crvValues)
	{
		vector<long> tmpLng{ curva.second.first };
		vector<double> tmpDbl{ curva.second.second };

		string wf = get<1>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(wf);

		string yf = get<2>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(yf);

		//Tengo que mirar esta funcion
		shared_ptr<QCInterestRateCurve> tmpCrv = QCFactoryFunctions::intRtCrvShrdPtr(
			tmpLng,
			tmpDbl,
			get<0>(crvChars.at(curva.first)),
			wf, yf, QCHelperFunctions::stringToQCIntRateCurve(get<3>(crvChars.at(curva.first))));
		allCurves.insert(pair<string, shared_ptr<QCInterestRateCurve>>(curva.first, tmpCrv));
	}

	//Guardaremos el CellMatrix customAmort con los datos de nominal vigente y amortizacion en
	//esta estructura.
	map<unsigned long, vector<tuple<QCDate, double, double>>> dateNotionalAndAmortByIdLeg;
	HelperFunctions::buildCustomAmortization(customAmort, dateNotionalAndAmortByIdLeg);

	//Metemos los fixings de los indices de tasa flotante en esta estructura
	map<string, map<QCDate, double>> mapManyFixings;
	HelperFunctions::buildManyFixings(fixings, mapManyFixings);

	//Metemos las caracteristicas de los indices en esta estructura
	map<string, pair<string, string>> indexChars; //en el pair viene el tenor y el start date rule
	HelperFunctions::buildStringPairStringMap(intRateIndexChars, indexChars);

	//Ahora hay que construir QCInterestRatePayoff para cada operacion
	//Se requiere:
	//	shared_ptr<QCInterestRate>
	//	shared_ptr<QCInterestRateLeg>
	//	QCDate (valueDate)
	//	shared_ptr<QCInterestRateCurve> (la curva de proyeccion)
	//	shared_ptr<QCInterestCurve> (la curva de descuento)
	//	shared_ptr<QCTimeSeries> (los fixings)

	//La info esta en la CellMatrix legCharacteristics:
	//0:	id_leg
	//1:	rec_pay
	//2:	start_date
	//3:	end_date
	//4:	settlement_calendar
	//5:	settlement_lag
	//6:	stub_period
	//7:	periodicity
	//8:	end_date_adjustment
	//9:	amortization
	//10:	interest_rate_index
	//11:	rate_value
	//12:	spread
	//13:	fixing_stub_period
	//14:	fixing_periodicity
	//15:	fixing_calendar
	//16:	fixing_lag
	//17:	notional_currency
	//18:	notional
	//19:	wealth_factor
	//20:	year_fraction
	//21:	projecting_curve
	//22:	discount_curve

	QCDate allValueDate{ (long)valueDate };
	map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
	for (unsigned long i = 0; i < legCharacteristics.RowsInStructure(); ++i)
	{
		string wf = legCharacteristics(i, 19).StringValue();
		QCHelperFunctions::lowerCase(wf);
		string yf = legCharacteristics(i, 20).StringValue();
		QCHelperFunctions::lowerCase(yf);
		shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
			(double)legCharacteristics(i, 11).NumericValue(), yf, wf);

		long numOp = (long)legCharacteristics(i, 0).NumericValue();
		vector<tuple<QCDate, double, double>> amortIfCustom;
		if (legCharacteristics(i, 9).StringValue() == "CUSTOM")
		{
			amortIfCustom = dateNotionalAndAmortByIdLeg.at(
				(long)legCharacteristics(i, 0).NumericValue());
		}
		QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildFloatingRateLeg(
			legCharacteristics(i, 1).StringValue(),					 //receive or pay
			QCDate{ (long)legCharacteristics(i, 2).NumericValue() }, //start date
			QCDate{ (long)legCharacteristics(i, 3).NumericValue() }, //end date
			mapHolidays.at(legCharacteristics(i, 4).StringValue()),  //settlement calendar
			(int)legCharacteristics(i, 5).NumericValue(),			 //settlement lag
			QCHelperFunctions::stringToQCStubPeriod(
			legCharacteristics(i, 6).StringValue()),				 //stub period
			legCharacteristics(i, 7).StringValue(),					 //periodicity
			QCHelperFunctions::stringToQCBusDayAdjRule(
			legCharacteristics(i, 8).StringValue()),				 //end date adjustment
			QCHelperFunctions::stringToQCAmortization(
			legCharacteristics(i, 9).StringValue()),				 //amortization
			amortIfCustom,											 //amortization and notional by end date
			(int)legCharacteristics(i, 16).NumericValue(),			 //fixing lag
			QCHelperFunctions::stringToQCStubPeriod(
			legCharacteristics(i, 13).StringValue()),				 //fixing stub period
			legCharacteristics(i, 14).StringValue(),				 //fixing periodicity
			mapHolidays.at(legCharacteristics(i, 15).StringValue()), //fixing calendar
			indexChars.at(legCharacteristics(i, 10).StringValue()),  //interest rate index tenor
			(double)legCharacteristics(i, 18).NumericValue()		 //notional
			);
		shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
			new QCFloatingRatePayoff{tmpIntRate,
			legCharacteristics(i, 12).NumericValue(), 1.0,
			make_shared<QCInterestRateLeg>(tmpIntRateLeg),
			allCurves.at(legCharacteristics(i, 21).StringValue()),
			allCurves.at(legCharacteristics(i, 22).StringValue()),
			allValueDate,
			make_shared<map<QCDate, double>>(mapManyFixings.at(legCharacteristics(i, 10).StringValue()))});
		
		payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(
			(long)legCharacteristics(i, 0).NumericValue(),
			tmpIntRatePayoff));
	}

	//Calcular los valores presentes
	vector<tuple<long, double, vector<double>, vector<double>>> result;
	result.resize(payoffs.size());
	double m2m;
	vector<double> discDer;
	vector<double> projDer;
	unsigned long counter = 0;
	unsigned int longestDiscCurve = 0;
	unsigned int longestProjCurve = 0;
	for (const auto& payoff : payoffs)
	{
		m2m = payoff.second->presentValue();
		unsigned int discVertices = payoff.second->discountCurveLength();
		if (discVertices > longestDiscCurve)
			longestDiscCurve = discVertices;
		discDer.resize(discVertices);
		for (unsigned long i = 0; i < discVertices; ++i)
		{
			discDer.at(i) = payoff.second->getPvRateDerivativeAt(i);
		}

		unsigned int projVertices = payoff.second->projectingCurveLength();
		if (projVertices > longestProjCurve)
			longestProjCurve = projVertices;
		projDer.resize(projVertices);
		for (unsigned long i = 0; i < projVertices; ++i)
		{
			projDer.at(i) = payoff.second->getPvProjRateDerivativeAt(i);
		}
		result.at(counter) = make_tuple(payoff.first, m2m, discDer, projDer);
		++counter;
	}

	long cuantosResultados = legCharacteristics.RowsInStructure();
	CellMatrix legM2MAndDelta(cuantosResultados, longestDiscCurve + longestProjCurve + 2);
	for (long i = 0; i < cuantosResultados; ++i)
	{
		legM2MAndDelta(i, 0) = get<0>(result.at(i));
		legM2MAndDelta(i, 1) = get<1>(result.at(i));
		unsigned int discVertices = get<2>(result.at(i)).size();
		for (unsigned int j = 0; j < discVertices; ++j)
		{
			legM2MAndDelta(i, j + 2) = get<2>(result.at(i)).at(j) * BASIS_POINT;
		}
		unsigned int projVertices = get<3>(result.at(i)).size();
		for (unsigned int j = 0; j < projVertices; ++j)
		{
			legM2MAndDelta(i, j + discVertices + 2) = get<3>(result.at(i)).at(j) * BASIS_POINT;
		}
	}
	return legM2MAndDelta;

}

CellMatrix pvFloatingRateLegs2(double valueDate,
	CellMatrix holidays,
	CellMatrix curveValues,
	CellMatrix curveCharacteristics,
	CellMatrix legCharacteristics,
	CellMatrix customAmort,
	CellMatrix fixings,
	CellMatrix intRateIndexChars)
{
	//holidays: nombre, fecha. Se construye un map<string, vector<QCDate>> que para cada nombre de calendario
	//tenga todas las fechas
	map<string, vector<QCDate>> mapHolidays;
	HelperFunctions::buildHolidays(holidays, mapHolidays);

	//curveValues: nombre, tenor (en dias), valor (tasa o df)
	//Se construye un map<string, pair<vector<long>, vector<double>>> que contiene las curvas
	map<string, pair<vector<long>, vector<double>>> crvValues;
	HelperFunctions::buildCurveValues(curveValues, crvValues);

	//Toca ahora construir las curvas con su interpolador y sus convenciones de tasas
	//Primero metemos las caracteristicas de las curvas en un
	//map<string, tuple<string, string, string, string>> = HelperFunctions::string4
	map<string, HelperFunctions::string4> crvChars;
	HelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

	//Para cada uno de los elementos de crvValues debo terminar de dar de alta una curva cero
	//Tengo los plazos, las tasas y los nombres del interpolador, yf, wf y tipo de curva.
	//Necesito entonces llamar una funcion factory que con estos argumentos me de de alta un curva.
	//las curvas que vaya dando de alta las meto en un map

	map<string, shared_ptr<QCInterestRateCurve>> allCurves;

	//Loopeo sobre los keys de crvValues
	for (const auto &curva : crvValues)
	{
		vector<long> tmpLng{ curva.second.first };
		vector<double> tmpDbl{ curva.second.second };

		string wf = get<1>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(wf);

		string yf = get<2>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(yf);

		//Tengo que mirar esta funcion
		shared_ptr<QCInterestRateCurve> tmpCrv = QCFactoryFunctions::intRtCrvShrdPtr(
			tmpLng,
			tmpDbl,
			get<0>(crvChars.at(curva.first)),
			wf, yf, QCHelperFunctions::stringToQCIntRateCurve(get<3>(crvChars.at(curva.first))));
		allCurves.insert(pair<string, shared_ptr<QCInterestRateCurve>>(curva.first, tmpCrv));
	}

	//Guardaremos el CellMatrix customAmort con los datos de nominal vigente y amortizacion en
	//esta estructura.
	map<unsigned long, vector<tuple<QCDate, double, double>>> dateNotionalAndAmortByIdLeg;
	HelperFunctions::buildCustomAmortization(customAmort, dateNotionalAndAmortByIdLeg);

	//Metemos los fixings de los indices de tasa flotante en esta estructura
	map<string, map<QCDate, double>> mapManyFixings;
	HelperFunctions::buildManyFixings(fixings, mapManyFixings);

	//Metemos las caracteristicas de los indices en esta estructura
	map<string, pair<string, string>> indexChars; //en el pair viene el tenor y el start date rule
	HelperFunctions::buildStringPairStringMap(intRateIndexChars, indexChars);

	//Ahora hay que construir QCInterestRatePayoff para cada operacion
	//Se requiere:
	//	shared_ptr<QCInterestRate>
	//	shared_ptr<QCInterestRateLeg>
	//	QCDate (valueDate)
	//	shared_ptr<QCInterestRateCurve> (la curva de proyeccion)
	//	shared_ptr<QCInterestCurve> (la curva de descuento)
	//	shared_ptr<QCTimeSeries> (los fixings)

	//La info esta en la CellMatrix legCharacteristics:
	//0:	id_leg
	//1:	rec_pay
	//2:	start_date
	//3:	end_date
	//4:	settlement_calendar
	//5:	settlement_lag
	//6:	stub_period
	//7:	periodicity
	//8:	end_date_adjustment
	//9:	amortization
	//10:	interest_rate_index
	//11:	rate_value
	//12:	spread
	//13:	fixing_stub_period
	//14:	fixing_periodicity
	//15:	fixing_calendar
	//16:	fixing_lag
	//17:	notional_currency
	//18:	notional
	//19:	wealth_factor
	//20:	year_fraction
	//21:	projecting_curve
	//22:	discount_curve

	QCDate allValueDate{ (long)valueDate };
	map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
	for (unsigned long i = 0; i < legCharacteristics.RowsInStructure(); ++i)
	{
		string wf = legCharacteristics(i, 19).StringValue();
		QCHelperFunctions::lowerCase(wf);
		string yf = legCharacteristics(i, 20).StringValue();
		QCHelperFunctions::lowerCase(yf);
		shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
			(double)legCharacteristics(i, 11).NumericValue(), yf, wf);

		long numOp = (long)legCharacteristics(i, 0).NumericValue();
		vector<tuple<QCDate, double, double>> amortIfCustom;
		if (legCharacteristics(i, 9).StringValue() != "BULLET")
		{
			amortIfCustom = dateNotionalAndAmortByIdLeg.at(
				(long)legCharacteristics(i, 0).NumericValue());
		}
		QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildFloatingRateLeg2(
			legCharacteristics(i, 1).StringValue(),					 //receive or pay
			QCDate{ (long)legCharacteristics(i, 2).NumericValue() }, //start date
			QCDate{ (long)legCharacteristics(i, 3).NumericValue() }, //end date
			mapHolidays.at(legCharacteristics(i, 4).StringValue()),  //settlement calendar
			(int)legCharacteristics(i, 5).NumericValue(),			 //settlement lag
			QCHelperFunctions::stringToQCStubPeriod(
			legCharacteristics(i, 6).StringValue()),				 //stub period
			legCharacteristics(i, 7).StringValue(),					 //periodicity
			QCHelperFunctions::stringToQCBusDayAdjRule(
			legCharacteristics(i, 8).StringValue()),				 //end date adjustment
			QCHelperFunctions::stringToQCAmortization(
			legCharacteristics(i, 9).StringValue()),				 //amortization
			amortIfCustom,											 //amortization and notional by end date
			(int)legCharacteristics(i, 16).NumericValue(),			 //fixing lag
			QCHelperFunctions::stringToQCStubPeriod(
			legCharacteristics(i, 13).StringValue()),				 //fixing stub period
			legCharacteristics(i, 14).StringValue(),				 //fixing periodicity
			mapHolidays.at(legCharacteristics(i, 15).StringValue()), //fixing calendar
			indexChars.at(legCharacteristics(i, 10).StringValue()),  //interest rate index tenor
			(double)legCharacteristics(i, 18).NumericValue()		 //notional
			);
		shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
			new QCFloatingRatePayoff{ tmpIntRate,
			legCharacteristics(i, 12).NumericValue(), 1.0,
			make_shared<QCInterestRateLeg>(tmpIntRateLeg),
			allCurves.at(legCharacteristics(i, 21).StringValue()),
			allCurves.at(legCharacteristics(i, 22).StringValue()),
			allValueDate,
			make_shared<map<QCDate, double>>(mapManyFixings.at(legCharacteristics(i, 10).StringValue())) });

		payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(
			(long)legCharacteristics(i, 0).NumericValue(),
			tmpIntRatePayoff));
	}

	//Calcular los valores presentes
	vector<tuple<long, double, vector<double>, vector<double>>> result;
	result.resize(payoffs.size());
	double m2m;
	vector<double> discDer;
	vector<double> projDer;
	unsigned long counter = 0;
	unsigned int longestDiscCurve = 0;
	unsigned int longestProjCurve = 0;
	for (const auto& payoff : payoffs)
	{
		m2m = payoff.second->presentValue();
		unsigned int discVertices = payoff.second->discountCurveLength();
		if (discVertices > longestDiscCurve)
			longestDiscCurve = discVertices;
		discDer.resize(discVertices);
		for (unsigned long i = 0; i < discVertices; ++i)
		{
			discDer.at(i) = payoff.second->getPvRateDerivativeAt(i);
		}

		unsigned int projVertices = payoff.second->projectingCurveLength();
		if (projVertices > longestProjCurve)
			longestProjCurve = projVertices;
		projDer.resize(projVertices);
		for (unsigned long i = 0; i < projVertices; ++i)
		{
			projDer.at(i) = payoff.second->getPvProjRateDerivativeAt(i);
		}
		result.at(counter) = make_tuple(payoff.first, m2m, discDer, projDer);
		++counter;
	}

	long cuantosResultados = legCharacteristics.RowsInStructure();
	CellMatrix legM2MAndDelta(cuantosResultados, longestDiscCurve + longestProjCurve + 2);
	for (long i = 0; i < cuantosResultados; ++i)
	{
		legM2MAndDelta(i, 0) = get<0>(result.at(i));
		legM2MAndDelta(i, 1) = get<1>(result.at(i));
		unsigned int discVertices = get<2>(result.at(i)).size();
		for (unsigned int j = 0; j < discVertices; ++j)
		{
			legM2MAndDelta(i, j + 2) = get<2>(result.at(i)).at(j) * BASIS_POINT;
		}
		unsigned int projVertices = get<3>(result.at(i)).size();
		for (unsigned int j = 0; j < projVertices; ++j)
		{
			legM2MAndDelta(i, j + discVertices + 2) = get<3>(result.at(i)).at(j) * BASIS_POINT;
		}
	}
	return legM2MAndDelta;

}

CellMatrix buildInterestRateLeg(double startDate, double endDate, CellMatrix calendars,
	string settlementStubPeriod, string settlementPeriodicity, string endDateAdjustment,
	string settlementCalendar, int settlementLag,
	string fixingStubPeriod, string fixingPeriodicity, int fixingLag,
	string fixingCalendar, int fixingStartDateRule, string fixingTenor)
{
	//holidays: nombre, fecha. Se construye un map<string, vector<QCDate>> que para cada nombre de calendario
	//tenga todas las fechas
	map<string, vector<QCDate>> mapHolidays;
	HelperFunctions::buildHolidays(calendars, mapHolidays);

	QCInterestRatePeriodsFactory factory{ QCDate{ (long)startDate }, QCDate{ (long)endDate },
		QCHelperFunctions::stringToQCBusDayAdjRule(endDateAdjustment), settlementPeriodicity,
		QCHelperFunctions::stringToQCStubPeriod(settlementStubPeriod),
		make_shared<vector<QCDate>>(mapHolidays.at(settlementCalendar)),
		(unsigned int)settlementLag, fixingPeriodicity, QCHelperFunctions::stringToQCStubPeriod(fixingStubPeriod),
		make_shared<vector<QCDate>>(mapHolidays.at(fixingCalendar)), (unsigned int)fixingLag,
		(unsigned int)fixingStartDateRule, fixingTenor };

	auto periods = factory.getPeriods();

	//Hay que transformar periods a CellMatrix y retornar
	CellMatrix result(periods.size(), 11);

	for (unsigned int i = 0; i < periods.size(); ++i)
	{
		result(i, 0) = get<0>(periods.at(i));
		result(i, 1) = QCHelperFunctions::boolToZeroOne(get<1>(periods.at(i)));
		result(i, 2) = get<2>(periods.at(i));
		result(i, 3) = QCHelperFunctions::boolToZeroOne(get<3>(periods.at(i)));
		result(i, 4) = get<4>(periods.at(i));
		result(i, 5) = get<5>(periods.at(i)).excelSerial();
		result(i, 6) = get<6>(periods.at(i)).excelSerial();
		result(i, 7) = get<7>(periods.at(i)).excelSerial();
		result(i, 8) = get<8>(periods.at(i)).excelSerial();
		result(i, 9) = get<9>(periods.at(i)).excelSerial();
		result(i, 10) = get<10>(periods.at(i)).excelSerial();
	}

	return result;
}
