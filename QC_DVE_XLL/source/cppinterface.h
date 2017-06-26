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

string QCGetMacAddress();
string QCSha256(string input);
string QCGetAuthKey();
string QCGenerateKey(string password);

CellMatrix //Realiza el bootstrapping a partir de tasas cero y patas fijas
QCBootZeroRatesFixedLegs(int xlValueDate		//Fecha de calculo
					, CellMatrix xlInputRates	//Rango con datos de tasas
					, CellMatrix xlInputFixedLegs //Rango con datos de patas fijas
					, CellMatrix calendar		//Rango con feriados a aplicar
					, string interpolator		//Forma de interpolacion de la curva
					, string curveWf			//Factor de capitalizacion de la curva
					, string curveYf			//Fraccion de año de las tasas de la curva
					);

CellMatrix //Realiza el bootstrapping a partir de tasas cero, fx fwds y patas fijas
QCBootZeroRatesFwdsFixedLegs(double xlValueDate //Fecha de calculo
					, double fx					//Tipo de cambio spot
					, CellMatrix xlInputRates	//Rango con input de tasas
					, CellMatrix xlInputForwards//Rango con input de forwards
					, int whichLeg				//Indica que pata de los forwards se usa
					, CellMatrix xlInputFixedLegs //Input de patas fijas
					, string curveInterpolator	//Interpolacion de la curva a construir
					, string curveYf			//Fraccion de año de las tasas de la curva a construir
					, string curveWf			//Factor de capitalizacion de las tasas de la curva a construir
					, CellMatrix holidays		//Vector con feriados aplicables
					, CellMatrix auxCurve		//Plazos y tasas curva auxiliar (de los forwards)
					, string auxCurveInterpolator //Interpolacion de la curva auxiliar
					, string auxCurveYf			//Fraccion de año de las tasas de la curva auxiliar
					, string auxCurveWf			//Factor de capitalizacion de las tasas de la curva auxiliar
					);

CellMatrix //Realiza el bootstrapping a partir de tasas cero, fx fwds y patas flotantes
QCBootZeroRatesFwdsFloatingLegs(double xlValueDate	//Fecha de calculo
					, double fx						//Tipo de cambio spot
					, double fixing					//Fixing de la primera tasa flotante
					, CellMatrix xlInputRates		//Rango con input de tasas
					, CellMatrix xlInputForwards	//Rango con input de forwards
					, string fwdPoints				//Debe decir si o no. Indica si los precios son o no puntos fwd
					, string fwdsHolidays			//Nombre del calendario de sett de fwds
					, int whichLeg					//Indica que pata de los fwds se usa
					, CellMatrix xlInputFloatingLegs//Rango con input de patas flotantes
					, CellMatrix xlInputIndexChars	//Rango con las caracteristicas del indice flotante
					, string curveInterpolator		//Interpolacion de la curva a construir
					, string curveYf				//Fraccion de año de las tasas de la curva a construir
					, string curveWf				//Factor de capitalizacion de las tasas de la curva a construir
					, CellMatrix holidays			//Rango con todos los feriados
					, CellMatrix fwdsCurve			//Plazos y tasas de la curva auxiliar de los fwds
					, string fwdsCurveInterpolator	//Interpolacion de la curva de los fwds
					, string fwdsCurveYf			//Fraccion de agno de las tasas de la curva de los fwds
					, string fwdsCurveWf			//Factor de capitalizacion de las tasas de la curva de los fwds
					, CellMatrix floatCurve			//Plazos y tasas de la curva auxiliar de las patas flotantes
					, string floatCurveInterpolator	//Interpolacion de la curva de proyeccion de las patas flotantes
					, string floatCurveYf			//Fraccion de agno de las tasas de la curva de proyeccion de las patas flotantes
					, string floatCurveWf			//Factor de capitalizacion de las tasas de la curva de proyeccion de las patas flotantes
					);

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

double //Suma un tenor a una fecha considerando feriados
QCAddTenorToDate(string tenor //tenor 1M, 1Y, etc
				, double startDate //Fecha inicial
				, CellMatrix holidays //Vector vertical con feriados
				, string adjRule //Regla para ajustar en caso de dias inhabiles
				);

double //Suma un tenor a la chilena (termina dia 9 PREV) a una fecha considerando feriados
QCAddChileTenorToDate(string tenor //tenor 1M, 1Y, etc
					 , double startDate //Fecha inicial
					 , CellMatrix holidays //Vector vertical con feriados
					 );

double //Calcula proxima o previa fecha habil segun regla y calendarios
QCBusinessDate(double startDate //Fecha inicial
               , CellMatrix holidays //Vector vertical de feriados
			   , string adjRule //Regla de ajuste
			   );

double //Retorna una fecha como numero a partir de un string
QCGetDateFromString(string f //String que representa la fecha
				   );

double //Devuelve la fraccion de agno entre dos fechas
QCGetYearFraction(int startDate //Fecha inicial
			   , int endDate   //Fecha final
			   , string yf     //Nombre de la fraccion de agno
			   );

double //Devuelve tasa forward a partir de una curva
QCGetFwdRateFromCurve(CellMatrix curva //Plazos y tasas de la curva
				, string curveInterpolator //Tipo de interpolacion de la curva
				, string curveYf	//Fraccion de año de las tasas de la curva
				, string curveWf	//Factor de capitalizacion de las tasas de la curva
				, double plazo1		//Plazo corto en dias
				, double plazo2		//Plazo largo en dias
				, string rateYf		//Fraccion de año de la tasa forward
				, string rateWf		//Factor de capitalizacion de la tasa forward
				);

double //Devuelve el factor de capitalizacion de una tasa entre 2 fechas
QCGetWealthFactor(double rate	//Valor de la tasa
				 , double stDt		//Fecha inicial
				 , double endDt	//Fecha final
				 , string yf	//Tipo de fraccion de agno
				 , string wf	//Tipo de factor de capitalizacion
				 );

double //Devuelve el factor de descuento al plazo a partir de una curva cero
QCGetDiscountFactorFromCurve(CellMatrix curva//rango columna con plazos y tasas de la curva de descuento
						 , string curveInterpolator //tipo de interpolacion para la curva
						 , string curveYf //fraccion de año de las tasas de la curva
						 , string curveWf //factor de capitalizacion de las tasas de la curva
						 , double plazo //plazo a interpolar
						 );

double //Devuelve el factor de capitalizacion forward entre los plazos a partir de una curva cero
QCGetWealthFactorFwdFromCurve(CellMatrix curva//rango columna con plazos y tasas de la curva de descuento
						, string curveInterpolator //tipo de interpolacion para la curva
						, string curveYf //fraccion de año de las tasas de la curva
						, string curveWf //factor de capitalizacion de las tasas de la curva
						, double plazo1 //plazo a interpolar 1
						, double plazo2 //plazo a interpolar 2
						);

double //Devuelve el factor de descuento forward entre los plazos a partir de una curva cero
QCGetDiscountFactorFwdFromCurve(CellMatrix curva//rango columna con plazos y tasas de la curva de descuento
							, string curveInterpolator //tipo de interpolacion para la curva
							, string curveYf //fraccion de año de las tasas de la curva
							, string curveWf //factor de capitalizacion de las tasas de la curva
							, double plazo1 //plazo a interpolar 1
							, double plazo2 //plazo a interpolar 2
							);

CellMatrix cashFlow(CellMatrix tablaDesarrollo,
	double tasa, int fecha, string yf, string wf);

double //Calcula el valor presente de una pata fija
QCPvFixedRateLeg(CellMatrix tablaDesarrollo //vector de QCInterestRatePeriods de la pata
			 , CellMatrix nominalAmortizacion //vector columna con nominal vigente y amortizacion
			 , double tasa //valor de la tasa fija de la operacion
			 , double fecha //fecha de valorizacion
			 , string yf //fraccion de año de la tasa fija
			 , string wf //factor de capitalizacion de la tasa fija
			 , CellMatrix curva //rango columna con plazos y tasas de la curva de descuento
			 , string curveInterpolator //tipo de interpolacion para la curva
			 , string curveYf //fraccion de año de las tasas de la curva
			 , string curveWf //factor de capitalizacion de las tasas de la curva
			 );

double QCPvIcpClp(CellMatrix tablaDesarrollo //Tabla de desarrollo de la pata
	, CellMatrix nominalAmortizacion //vector columna con nominal vigente y amortizacion
	, double fecha				//fecha de valorizacion
	, double addSpread			//Spread aditivo
	, CellMatrix curva			//Curva de proyeccion
	, string curveInterpolator	//tipo de interpolacion para la curva
	, string curveYf					//fraccion de año de las tasas de la curva
	, string curveWf					//factor de capitalizacion de las tasas de la curva
	);

double QCPvFloatingLeg(CellMatrix tablaDesarrollo //Tabla de desarrollo de la pata
	, CellMatrix nominalAmortizacion //Rango con dos columnas con nominal vigente y amortizacion por periodo
	, double fecha			//Fecha de valorizacion
	, CellMatrix curvaProy	//Rango con dos columnas con plazos y tasas curva proyeccion
	, string projCurveInterpolator	//Metodo de interpolacion de la curva de proyeccion
	, string projCurveYf	//Fraccion de ano de las tasas de la curva de proyeccion
	, string projCurveWf	//Factor de capitalizacion de las tasas de la curva de proyeccion
	, CellMatrix curvaDesc	//Rango con dos columnas con plazos y tasas de la curva de descuento
	, string discCurveInterpolator	//Metodo de interpolacion de la curva de descuento
	, string discCurveYf	//Fraccion de ano de las tasas de la curva de descuento
	, string discCurveWf	//Factor de capitalizacion de las tasas de la curva de descuento
	, CellMatrix pastFixings//Rango con dos columnas con fechas y tasas para fixing
	, double addSpread		//Valor del spread sobre tasa flotante
	, double multSpread		//Valor del spread multiplicativo
	, string yf				//Fraccion de ano de la tasa flotante
	, string wf				//Factor de capitalizacion de la tasa flotante
	);

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
