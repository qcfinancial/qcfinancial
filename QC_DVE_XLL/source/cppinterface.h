//
//
//                                                                    Test.h
//

#ifndef TEST_H
#define TEST_H

#include "xlw/MyContainers.h"
#include <xlw/CellMatrix.h>
#include <xlw/DoubleOrNothing.h>
#include <xlw/ArgList.h>
#include <string>

using namespace xlw;
using namespace std;

//<xlw:libraryname=QC_DVE

CellMatrix checkBoostrapping(int xlValueDate,
	CellMatrix xlInputRates,
	CellMatrix xlInputFixedLegs,
	CellMatrix calendar,
	string interpolator);

CellMatrix checkBoostrappingFwds(int xlValueDate,
	CellMatrix xlInputRates,
	CellMatrix xlInputForwards,
	int whichLeg,
	CellMatrix xlInputFixedLegs,
	CellMatrix calendar,
	CellMatrix auxCurve,
	double fx,
	string interpolator);

CellMatrix checkBoostrappingFwdsFloating(int xlValueDate,
	CellMatrix xlInputRates,
	CellMatrix xlInputForwards,
	int whichLeg,
	CellMatrix xlInputFloatingLegs,
	CellMatrix holidays,
	CellMatrix intRateIndexChars,
	CellMatrix auxCurveFwd,
	CellMatrix auxCurveFloating,
	double fx,
	double fixing,
	string interpolator);

double qcFecha(string f);

double //Devuelve la fraccion de agno entre dos fechas
	qcYearFraction(int startDate //Fecha inicial
				 , int endDate   //Fecha final
				 , string yf     //Nombre de la fraccion de agno
					);

double //Devuelve el factor de capitalizacion de una tasa entre 2 fechas
	qcWealthFactor(double rate	//Valor de la tasa
				 , int stDt		//Fecha inicial
				 , int endDt	//Fecha final
				 , string yf	//Tipo de fraccion de agno
				 , string wf	//Tipo de factor de capitalacion
				 );

CellMatrix cashFlow(CellMatrix tablaDesarrollo,
	double tasa, int fecha, string yf, string wf);

double pvFixed1(CellMatrix tablaDesarrollo //Tabla de desarrollo de la pata
	, CellMatrix curva	//Curva cero cupon (rango con tenors y tasas)
	, double tasa		//valor de la tasa fija de la pata
	, int fecha			//fecha de valorizacion
	, string yf			//nombre de la fraccion de agno de la tasa fija
	, string wf			//nombre del tipo de factor de capitalizacion
	);

/*double pvFixed(CellMatrix tablaDesarrollo //Tabla de desarrollo de la pata
	, CellMatrix curva		//Curve cero cupon de descuento
	, double tasa			//valor de la tasa fija de la pata
	, int fecha				//fecha de valorizacion
	, string yf				//nombre de la fraccion de agno de la tasa fija
	, string wf				//nombre del tipo de factor de capitalizacion
	, string curveYf		//fraccion de las tasas de la curva
	, string curveWf		//factor de capitalizacion de la curva
	);*/

double pvFloat1(CellMatrix tablaDesarrollo //Tabla de desarrollo de la pata
	, CellMatrix curvaProy		//Curva de proyeccion
	, CellMatrix curvaDesc		//Curva de descuento
	, CellMatrix pastFixings	//Fixings anteriores
	, double addSpread			//Spread aditivo
	, double multSpread			//Spread multiplicativo
	, int fecha					//Fecha de valorizacion
	, string yf					//Yf de la tasa flotante
	, string wf					//Wf de la tasa flotante
	);

/*double //Valor presente de una pata a tasa flotante
pvFloat(CellMatrix schedule		//Tabla de desarrollo de la pata
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
	);*/

double pvIcpClp1(CellMatrix tablaDesarrollo //Tabla de desarrollo de la pata
	, CellMatrix curvaProy		//Curva de proyeccion
	, CellMatrix curvaDesc		//Curva de descuento
	, CellMatrix pastFixings	//Fixings pasados de ICP
	, double addSpread			//Spread aditivo
	, double multSpread			//Spread multiplicativo
	, int fecha					//Fecha de valorizacion
	, string yf					//Fraccion de año de la tasa flotante
	, string wf					//Factor de capitalizacion de la tasa flotante
	);

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
	);

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
	);

CellMatrix pvFixedLegs(double valueDate,
	CellMatrix holidays,
	CellMatrix curveValues,
	CellMatrix curveCharacteristics,
	CellMatrix legCharacteristics,
	CellMatrix customAmort);

CellMatrix pvFixedLegs2(double valueDate,
	CellMatrix holidays,
	CellMatrix curveValues,
	CellMatrix curveCharacteristics,
	CellMatrix legCharacteristics,
	CellMatrix customAmort);

CellMatrix pvIcpClpLegs(double valueDate,
	CellMatrix holidays,
	CellMatrix curveValues,
	CellMatrix curveCharacteristics,
	CellMatrix legCharacteristics,
	CellMatrix customAmort,
	CellMatrix fixings);

CellMatrix pvIcpClpLegs2(double valueDate,
	CellMatrix holidays,
	CellMatrix curveValues,
	CellMatrix curveCharacteristics,
	CellMatrix legCharacteristics,
	CellMatrix customAmort,
	CellMatrix fixings);

CellMatrix pvIcpClfLegs(double valueDate,
	CellMatrix holidays,
	CellMatrix curveValues,
	CellMatrix curveCharacteristics,
	CellMatrix legCharacteristics,
	CellMatrix customAmort,
	CellMatrix icpFixings,
	CellMatrix ufFixings);

CellMatrix pvIcpClfLegs2(double valueDate,
	CellMatrix holidays,
	CellMatrix curveValues,
	CellMatrix curveCharacteristics,
	CellMatrix legCharacteristics,
	CellMatrix customAmort,
	CellMatrix icpFixings,
	CellMatrix ufFixings);

CellMatrix buildInterestRateLeg(double startDate,
	double endDate,
	CellMatrix calendars,
	string settlementStubPeriod,
	string settlementPeriodicity,
	string endDateAdjustment,
	string settlementCalendar,
	int settlementLag,
	string fixingStubPeriod,
	string fixingPeriodicity,
	int fixingLag,
	string fixingCalendar,
	int fixingStartDateRule,
	string fixingTenor);

CellMatrix pvFloatingRateLegs(double valueDate,
	CellMatrix holidays,
	CellMatrix curveValues,
	CellMatrix curveCharacteristics,
	CellMatrix legCharacteristics,
	CellMatrix customAmort,
	CellMatrix fixings,
	CellMatrix intRateIndexChars);

CellMatrix pvFloatingRateLegs2(double valueDate,
	CellMatrix holidays,
	CellMatrix curveValues,
	CellMatrix curveCharacteristics,
	CellMatrix legCharacteristics,
	CellMatrix customAmort,
	CellMatrix fixings,
	CellMatrix intRateIndexChars);

#endif
