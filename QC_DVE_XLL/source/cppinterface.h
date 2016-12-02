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

short // echoes a short
EchoShort(short x // number to be echoed
       );

double //Devuelve la suma de 2 numeros
suuma(double x //primer numero
	, double y //segundo numero
	);

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

double pv(CellMatrix tablaDesarrollo //Tabla de desarrollo de la pata
	, CellMatrix curva	//Curva cero cupon (rango con tenors y tasas)
	, double tasa		//valor de la tasa fija de la pata
	, int fecha			//fecha de valorizacion
	, string yf			//nombre de la fraccion de agno de la tasa fija
	, string wf			//nombre del tipo de factor de capitalizacion
	);


#endif
