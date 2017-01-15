//// 
//// Autogenerated by xlw 
//// Do not edit this file, it will be overwritten 
//// by InterfaceGenerator 
////

#include "xlw/MyContainers.h"
#include <xlw/CellMatrix.h>
#include "cppinterface.h"
#include <xlw/xlw.h>
#include <xlw/XlFunctionRegistration.h>
#include <stdexcept>
#include <xlw/XlOpenClose.h>
#include <xlw/HiResTimer.h>
using namespace xlw;

namespace {
const char* LibraryName = "QC_DVE";
};


// registrations start here


namespace
{
XLRegistration::Arg
qcFechaArgs[]=
{
{ "f","too lazy to comment this one ","XLF_OPER"}
};
  XLRegistration::XLFunctionRegistrationHelper
registerqcFecha("xlqcFecha",
"qcFecha",
"too lazy to comment this function ",
LibraryName,
qcFechaArgs,
1
,false
,false
,""
,""
,false
,false
,false
);
}



extern "C"
{
LPXLFOPER EXCEL_EXPORT
xlqcFecha(
LPXLFOPER fa)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

XlfOper fb(
	(fa));
string f(
	fb.AsString("f"));

double result(
	qcFecha(
		f)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
qcYearFractionArgs[]=
{
{ "startDate","Fecha inicial ","B"},
{ "endDate","Fecha final ","B"},
{ "yf","Nombre de la fraccion de agno ","XLF_OPER"}
};
  XLRegistration::XLFunctionRegistrationHelper
registerqcYearFraction("xlqcYearFraction",
"qcYearFraction",
"Devuelve la fraccion de agno entre dos fechas ",
LibraryName,
qcYearFractionArgs,
3
,false
,false
,""
,""
,false
,false
,false
);
}



extern "C"
{
LPXLFOPER EXCEL_EXPORT
xlqcYearFraction(
double startDatea,
double endDatea,
LPXLFOPER yfa)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

int startDate(
	static_cast<int>(startDatea));

int endDate(
	static_cast<int>(endDatea));

XlfOper yfb(
	(yfa));
string yf(
	yfb.AsString("yf"));

double result(
	qcYearFraction(
		startDate,
		endDate,
		yf)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
qcWealthFactorArgs[]=
{
{ "rate","Valor de la tasa ","B"},
{ "stDt","Fecha inicial ","B"},
{ "endDt","Fecha final ","B"},
{ "yf","Tipo de fraccion de agno ","XLF_OPER"},
{ "wf","Tipo de factor de capitalacion ","XLF_OPER"}
};
  XLRegistration::XLFunctionRegistrationHelper
registerqcWealthFactor("xlqcWealthFactor",
"qcWealthFactor",
"Devuelve el factor de capitalizacion de una tasa entre 2 fechas ",
LibraryName,
qcWealthFactorArgs,
5
,false
,false
,""
,""
,false
,false
,false
);
}



extern "C"
{
LPXLFOPER EXCEL_EXPORT
xlqcWealthFactor(
double rate,
double stDta,
double endDta,
LPXLFOPER yfa,
LPXLFOPER wfa)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);


int stDt(
	static_cast<int>(stDta));

int endDt(
	static_cast<int>(endDta));

XlfOper yfb(
	(yfa));
string yf(
	yfb.AsString("yf"));

XlfOper wfb(
	(wfa));
string wf(
	wfb.AsString("wf"));

double result(
	qcWealthFactor(
		rate,
		stDt,
		endDt,
		yf,
		wf)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
cashFlowArgs[]=
{
{ "tablaDesarrollo","too lazy to comment this one ","XLF_OPER"},
{ "tasa","too lazy to comment this one ","B"},
{ "fecha","too lazy to comment this one ","B"},
{ "yf","too lazy to comment this one ","XLF_OPER"},
{ "wf","too lazy to comment this one ","XLF_OPER"}
};
  XLRegistration::XLFunctionRegistrationHelper
registercashFlow("xlcashFlow",
"cashFlow",
"too lazy to comment this function ",
LibraryName,
cashFlowArgs,
5
,false
,false
,""
,""
,false
,false
,false
);
}



extern "C"
{
LPXLFOPER EXCEL_EXPORT
xlcashFlow(
LPXLFOPER tablaDesarrolloa,
double tasa,
double fechaa,
LPXLFOPER yfa,
LPXLFOPER wfa)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

XlfOper tablaDesarrollob(
	(tablaDesarrolloa));
CellMatrix tablaDesarrollo(
	tablaDesarrollob.AsCellMatrix("tablaDesarrollo"));


int fecha(
	static_cast<int>(fechaa));

XlfOper yfb(
	(yfa));
string yf(
	yfb.AsString("yf"));

XlfOper wfb(
	(wfa));
string wf(
	wfb.AsString("wf"));

CellMatrix result(
	cashFlow(
		tablaDesarrollo,
		tasa,
		fecha,
		yf,
		wf)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
pvFixed1Args[]=
{
{ "tablaDesarrollo","Tabla de desarrollo de la pata ","XLF_OPER"},
{ "curva","Curva cero cupon (rango con tenors y tasas) ","XLF_OPER"},
{ "tasa","valor de la tasa fija de la pata ","B"},
{ "fecha","fecha de valorizacion ","B"},
{ "yf","nombre de la fraccion de agno de la tasa fija ","XLF_OPER"},
{ "wf","nombre del tipo de factor de capitalizacion ","XLF_OPER"}
};
  XLRegistration::XLFunctionRegistrationHelper
registerpvFixed1("xlpvFixed1",
"pvFixed1",
"too lazy to comment this function ",
LibraryName,
pvFixed1Args,
6
,false
,false
,""
,""
,false
,false
,false
);
}



extern "C"
{
LPXLFOPER EXCEL_EXPORT
xlpvFixed1(
LPXLFOPER tablaDesarrolloa,
LPXLFOPER curvaa,
double tasa,
double fechaa,
LPXLFOPER yfa,
LPXLFOPER wfa)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

XlfOper tablaDesarrollob(
	(tablaDesarrolloa));
CellMatrix tablaDesarrollo(
	tablaDesarrollob.AsCellMatrix("tablaDesarrollo"));

XlfOper curvab(
	(curvaa));
CellMatrix curva(
	curvab.AsCellMatrix("curva"));


int fecha(
	static_cast<int>(fechaa));

XlfOper yfb(
	(yfa));
string yf(
	yfb.AsString("yf"));

XlfOper wfb(
	(wfa));
string wf(
	wfb.AsString("wf"));

double result(
	pvFixed1(
		tablaDesarrollo,
		curva,
		tasa,
		fecha,
		yf,
		wf)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
pvFloat1Args[]=
{
{ "tablaDesarrollo","Tabla de desarrollo de la pata ","XLF_OPER"},
{ "curvaProy","Curva de proyeccion ","XLF_OPER"},
{ "curvaDesc","Curva de descuento ","XLF_OPER"},
{ "pastFixings","Fixings anteriores ","XLF_OPER"},
{ "addSpread","Spread aditivo ","B"},
{ "multSpread","Spread multiplicativo ","B"},
{ "fecha","Fecha de valorizacion ","B"},
{ "yf","Yf de la tasa flotante ","XLF_OPER"},
{ "wf","Wf de la tasa flotante ","XLF_OPER"}
};
  XLRegistration::XLFunctionRegistrationHelper
registerpvFloat1("xlpvFloat1",
"pvFloat1",
"too lazy to comment this function ",
LibraryName,
pvFloat1Args,
9
,false
,false
,""
,""
,false
,false
,false
);
}



extern "C"
{
LPXLFOPER EXCEL_EXPORT
xlpvFloat1(
LPXLFOPER tablaDesarrolloa,
LPXLFOPER curvaProya,
LPXLFOPER curvaDesca,
LPXLFOPER pastFixingsa,
double addSpread,
double multSpread,
double fechaa,
LPXLFOPER yfa,
LPXLFOPER wfa)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

XlfOper tablaDesarrollob(
	(tablaDesarrolloa));
CellMatrix tablaDesarrollo(
	tablaDesarrollob.AsCellMatrix("tablaDesarrollo"));

XlfOper curvaProyb(
	(curvaProya));
CellMatrix curvaProy(
	curvaProyb.AsCellMatrix("curvaProy"));

XlfOper curvaDescb(
	(curvaDesca));
CellMatrix curvaDesc(
	curvaDescb.AsCellMatrix("curvaDesc"));

XlfOper pastFixingsb(
	(pastFixingsa));
CellMatrix pastFixings(
	pastFixingsb.AsCellMatrix("pastFixings"));



int fecha(
	static_cast<int>(fechaa));

XlfOper yfb(
	(yfa));
string yf(
	yfb.AsString("yf"));

XlfOper wfb(
	(wfa));
string wf(
	wfb.AsString("wf"));

double result(
	pvFloat1(
		tablaDesarrollo,
		curvaProy,
		curvaDesc,
		pastFixings,
		addSpread,
		multSpread,
		fecha,
		yf,
		wf)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
pvIcpClp1Args[]=
{
{ "tablaDesarrollo","Tabla de desarrollo de la pata ","XLF_OPER"},
{ "curvaProy","Curva de proyeccion ","XLF_OPER"},
{ "curvaDesc","Curva de descuento ","XLF_OPER"},
{ "pastFixings","Fixings pasados de ICP ","XLF_OPER"},
{ "addSpread","Spread aditivo ","B"},
{ "multSpread","Spread multiplicativo ","B"},
{ "fecha","Fecha de valorizacion ","B"},
{ "yf","Fraccion de a o de la tasa flotante ","XLF_OPER"},
{ "wf","Factor de capitalizacion de la tasa flotante ","XLF_OPER"}
};
  XLRegistration::XLFunctionRegistrationHelper
registerpvIcpClp1("xlpvIcpClp1",
"pvIcpClp1",
"too lazy to comment this function ",
LibraryName,
pvIcpClp1Args,
9
,false
,false
,""
,""
,false
,false
,false
);
}



extern "C"
{
LPXLFOPER EXCEL_EXPORT
xlpvIcpClp1(
LPXLFOPER tablaDesarrolloa,
LPXLFOPER curvaProya,
LPXLFOPER curvaDesca,
LPXLFOPER pastFixingsa,
double addSpread,
double multSpread,
double fechaa,
LPXLFOPER yfa,
LPXLFOPER wfa)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

XlfOper tablaDesarrollob(
	(tablaDesarrolloa));
CellMatrix tablaDesarrollo(
	tablaDesarrollob.AsCellMatrix("tablaDesarrollo"));

XlfOper curvaProyb(
	(curvaProya));
CellMatrix curvaProy(
	curvaProyb.AsCellMatrix("curvaProy"));

XlfOper curvaDescb(
	(curvaDesca));
CellMatrix curvaDesc(
	curvaDescb.AsCellMatrix("curvaDesc"));

XlfOper pastFixingsb(
	(pastFixingsa));
CellMatrix pastFixings(
	pastFixingsb.AsCellMatrix("pastFixings"));



int fecha(
	static_cast<int>(fechaa));

XlfOper yfb(
	(yfa));
string yf(
	yfb.AsString("yf"));

XlfOper wfb(
	(wfa));
string wf(
	wfb.AsString("wf"));

double result(
	pvIcpClp1(
		tablaDesarrollo,
		curvaProy,
		curvaDesc,
		pastFixings,
		addSpread,
		multSpread,
		fecha,
		yf,
		wf)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
pvIcpClpArgs[]=
{
{ "tablaDesarrollo","Tabla de desarrollo de la pata ","XLF_OPER"},
{ "curvaProy","Curva de proyeccion ","XLF_OPER"},
{ "curvaDesc","Curva de descuento ","XLF_OPER"},
{ "pastFixings","Fixings pasados de ICP ","XLF_OPER"},
{ "addSpread","Spread aditivo ","B"},
{ "multSpread","Spread multiplicativo ","B"},
{ "fecha","Fecha de valorizacion ","B"},
{ "yf","Fraccion de a o de la tasa flotante ","XLF_OPER"},
{ "wf","Factor de capitalizacion de la tasa flotante ","XLF_OPER"},
{ "curveYf","Fraccion de agno de la curva ","XLF_OPER"},
{ "curveWf","Factor de capitalizacion de la curva ","XLF_OPER"}
};
  XLRegistration::XLFunctionRegistrationHelper
registerpvIcpClp("xlpvIcpClp",
"pvIcpClp",
"too lazy to comment this function ",
LibraryName,
pvIcpClpArgs,
11
,false
,false
,""
,""
,false
,false
,false
);
}



extern "C"
{
LPXLFOPER EXCEL_EXPORT
xlpvIcpClp(
LPXLFOPER tablaDesarrolloa,
LPXLFOPER curvaProya,
LPXLFOPER curvaDesca,
LPXLFOPER pastFixingsa,
double addSpread,
double multSpread,
double fechaa,
LPXLFOPER yfa,
LPXLFOPER wfa,
LPXLFOPER curveYfa,
LPXLFOPER curveWfa)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

XlfOper tablaDesarrollob(
	(tablaDesarrolloa));
CellMatrix tablaDesarrollo(
	tablaDesarrollob.AsCellMatrix("tablaDesarrollo"));

XlfOper curvaProyb(
	(curvaProya));
CellMatrix curvaProy(
	curvaProyb.AsCellMatrix("curvaProy"));

XlfOper curvaDescb(
	(curvaDesca));
CellMatrix curvaDesc(
	curvaDescb.AsCellMatrix("curvaDesc"));

XlfOper pastFixingsb(
	(pastFixingsa));
CellMatrix pastFixings(
	pastFixingsb.AsCellMatrix("pastFixings"));



int fecha(
	static_cast<int>(fechaa));

XlfOper yfb(
	(yfa));
string yf(
	yfb.AsString("yf"));

XlfOper wfb(
	(wfa));
string wf(
	wfb.AsString("wf"));

XlfOper curveYfb(
	(curveYfa));
string curveYf(
	curveYfb.AsString("curveYf"));

XlfOper curveWfb(
	(curveWfa));
string curveWf(
	curveWfb.AsString("curveWf"));

double result(
	pvIcpClp(
		tablaDesarrollo,
		curvaProy,
		curvaDesc,
		pastFixings,
		addSpread,
		multSpread,
		fecha,
		yf,
		wf,
		curveYf,
		curveWf)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
pvIcpClfArgs[]=
{
{ "tablaDesarrollo","Tabla de desarrollo de la pata ","XLF_OPER"},
{ "curvaProy","Curva de proyeccion ","XLF_OPER"},
{ "curvaDesc","Curva de descuento ","XLF_OPER"},
{ "pastFixings","Fixings pasados de ICP ","XLF_OPER"},
{ "ufFixings","Fixings pasado de UF ","XLF_OPER"},
{ "addSpread","Spread aditivo ","B"},
{ "multSpread","Spread multiplicativo ","B"},
{ "fecha","Fecha de valorizacion ","B"},
{ "yf","Fraccion de a o de la tasa flotante ","XLF_OPER"},
{ "wf","Factor de capitalizacion de la tasa flotante ","XLF_OPER"}
};
  XLRegistration::XLFunctionRegistrationHelper
registerpvIcpClf("xlpvIcpClf",
"pvIcpClf",
"too lazy to comment this function ",
LibraryName,
pvIcpClfArgs,
10
,false
,false
,""
,""
,false
,false
,false
);
}



extern "C"
{
LPXLFOPER EXCEL_EXPORT
xlpvIcpClf(
LPXLFOPER tablaDesarrolloa,
LPXLFOPER curvaProya,
LPXLFOPER curvaDesca,
LPXLFOPER pastFixingsa,
LPXLFOPER ufFixingsa,
double addSpread,
double multSpread,
double fechaa,
LPXLFOPER yfa,
LPXLFOPER wfa)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

XlfOper tablaDesarrollob(
	(tablaDesarrolloa));
CellMatrix tablaDesarrollo(
	tablaDesarrollob.AsCellMatrix("tablaDesarrollo"));

XlfOper curvaProyb(
	(curvaProya));
CellMatrix curvaProy(
	curvaProyb.AsCellMatrix("curvaProy"));

XlfOper curvaDescb(
	(curvaDesca));
CellMatrix curvaDesc(
	curvaDescb.AsCellMatrix("curvaDesc"));

XlfOper pastFixingsb(
	(pastFixingsa));
CellMatrix pastFixings(
	pastFixingsb.AsCellMatrix("pastFixings"));

XlfOper ufFixingsb(
	(ufFixingsa));
CellMatrix ufFixings(
	ufFixingsb.AsCellMatrix("ufFixings"));



int fecha(
	static_cast<int>(fechaa));

XlfOper yfb(
	(yfa));
string yf(
	yfb.AsString("yf"));

XlfOper wfb(
	(wfa));
string wf(
	wfb.AsString("wf"));

double result(
	pvIcpClf(
		tablaDesarrollo,
		curvaProy,
		curvaDesc,
		pastFixings,
		ufFixings,
		addSpread,
		multSpread,
		fecha,
		yf,
		wf)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
pvFixedLegsArgs[]=
{
{ "valueDate","too lazy to comment this one ","B"},
{ "holidays","too lazy to comment this one ","XLF_OPER"},
{ "curveValues","too lazy to comment this one ","XLF_OPER"},
{ "curveCharacteristics","too lazy to comment this one ","XLF_OPER"},
{ "legCharacteristics","too lazy to comment this one ","XLF_OPER"},
{ "customAmort","too lazy to comment this one ","XLF_OPER"}
};
  XLRegistration::XLFunctionRegistrationHelper
registerpvFixedLegs("xlpvFixedLegs",
"pvFixedLegs",
"too lazy to comment this function ",
LibraryName,
pvFixedLegsArgs,
6
,false
,false
,""
,""
,false
,false
,false
);
}



extern "C"
{
LPXLFOPER EXCEL_EXPORT
xlpvFixedLegs(
double valueDate,
LPXLFOPER holidaysa,
LPXLFOPER curveValuesa,
LPXLFOPER curveCharacteristicsa,
LPXLFOPER legCharacteristicsa,
LPXLFOPER customAmorta)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);


XlfOper holidaysb(
	(holidaysa));
CellMatrix holidays(
	holidaysb.AsCellMatrix("holidays"));

XlfOper curveValuesb(
	(curveValuesa));
CellMatrix curveValues(
	curveValuesb.AsCellMatrix("curveValues"));

XlfOper curveCharacteristicsb(
	(curveCharacteristicsa));
CellMatrix curveCharacteristics(
	curveCharacteristicsb.AsCellMatrix("curveCharacteristics"));

XlfOper legCharacteristicsb(
	(legCharacteristicsa));
CellMatrix legCharacteristics(
	legCharacteristicsb.AsCellMatrix("legCharacteristics"));

XlfOper customAmortb(
	(customAmorta));
CellMatrix customAmort(
	customAmortb.AsCellMatrix("customAmort"));

CellMatrix result(
	pvFixedLegs(
		valueDate,
		holidays,
		curveValues,
		curveCharacteristics,
		legCharacteristics,
		customAmort)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
pvFixedLegs2Args[]=
{
{ "valueDate","too lazy to comment this one ","B"},
{ "holidays","too lazy to comment this one ","XLF_OPER"},
{ "curveValues","too lazy to comment this one ","XLF_OPER"},
{ "curveCharacteristics","too lazy to comment this one ","XLF_OPER"},
{ "legCharacteristics","too lazy to comment this one ","XLF_OPER"},
{ "customAmort","too lazy to comment this one ","XLF_OPER"}
};
  XLRegistration::XLFunctionRegistrationHelper
registerpvFixedLegs2("xlpvFixedLegs2",
"pvFixedLegs2",
"too lazy to comment this function ",
LibraryName,
pvFixedLegs2Args,
6
,false
,false
,""
,""
,false
,false
,false
);
}



extern "C"
{
LPXLFOPER EXCEL_EXPORT
xlpvFixedLegs2(
double valueDate,
LPXLFOPER holidaysa,
LPXLFOPER curveValuesa,
LPXLFOPER curveCharacteristicsa,
LPXLFOPER legCharacteristicsa,
LPXLFOPER customAmorta)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);


XlfOper holidaysb(
	(holidaysa));
CellMatrix holidays(
	holidaysb.AsCellMatrix("holidays"));

XlfOper curveValuesb(
	(curveValuesa));
CellMatrix curveValues(
	curveValuesb.AsCellMatrix("curveValues"));

XlfOper curveCharacteristicsb(
	(curveCharacteristicsa));
CellMatrix curveCharacteristics(
	curveCharacteristicsb.AsCellMatrix("curveCharacteristics"));

XlfOper legCharacteristicsb(
	(legCharacteristicsa));
CellMatrix legCharacteristics(
	legCharacteristicsb.AsCellMatrix("legCharacteristics"));

XlfOper customAmortb(
	(customAmorta));
CellMatrix customAmort(
	customAmortb.AsCellMatrix("customAmort"));

CellMatrix result(
	pvFixedLegs2(
		valueDate,
		holidays,
		curveValues,
		curveCharacteristics,
		legCharacteristics,
		customAmort)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
pvIcpClpLegsArgs[]=
{
{ "valueDate","too lazy to comment this one ","B"},
{ "holidays","too lazy to comment this one ","XLF_OPER"},
{ "curveValues","too lazy to comment this one ","XLF_OPER"},
{ "curveCharacteristics","too lazy to comment this one ","XLF_OPER"},
{ "legCharacteristics","too lazy to comment this one ","XLF_OPER"},
{ "customAmort","too lazy to comment this one ","XLF_OPER"},
{ "fixings","too lazy to comment this one ","XLF_OPER"}
};
  XLRegistration::XLFunctionRegistrationHelper
registerpvIcpClpLegs("xlpvIcpClpLegs",
"pvIcpClpLegs",
"too lazy to comment this function ",
LibraryName,
pvIcpClpLegsArgs,
7
,false
,false
,""
,""
,false
,false
,false
);
}



extern "C"
{
LPXLFOPER EXCEL_EXPORT
xlpvIcpClpLegs(
double valueDate,
LPXLFOPER holidaysa,
LPXLFOPER curveValuesa,
LPXLFOPER curveCharacteristicsa,
LPXLFOPER legCharacteristicsa,
LPXLFOPER customAmorta,
LPXLFOPER fixingsa)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);


XlfOper holidaysb(
	(holidaysa));
CellMatrix holidays(
	holidaysb.AsCellMatrix("holidays"));

XlfOper curveValuesb(
	(curveValuesa));
CellMatrix curveValues(
	curveValuesb.AsCellMatrix("curveValues"));

XlfOper curveCharacteristicsb(
	(curveCharacteristicsa));
CellMatrix curveCharacteristics(
	curveCharacteristicsb.AsCellMatrix("curveCharacteristics"));

XlfOper legCharacteristicsb(
	(legCharacteristicsa));
CellMatrix legCharacteristics(
	legCharacteristicsb.AsCellMatrix("legCharacteristics"));

XlfOper customAmortb(
	(customAmorta));
CellMatrix customAmort(
	customAmortb.AsCellMatrix("customAmort"));

XlfOper fixingsb(
	(fixingsa));
CellMatrix fixings(
	fixingsb.AsCellMatrix("fixings"));

CellMatrix result(
	pvIcpClpLegs(
		valueDate,
		holidays,
		curveValues,
		curveCharacteristics,
		legCharacteristics,
		customAmort,
		fixings)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
pvIcpClpLegs2Args[]=
{
{ "valueDate","too lazy to comment this one ","B"},
{ "holidays","too lazy to comment this one ","XLF_OPER"},
{ "curveValues","too lazy to comment this one ","XLF_OPER"},
{ "curveCharacteristics","too lazy to comment this one ","XLF_OPER"},
{ "legCharacteristics","too lazy to comment this one ","XLF_OPER"},
{ "customAmort","too lazy to comment this one ","XLF_OPER"},
{ "fixings","too lazy to comment this one ","XLF_OPER"}
};
  XLRegistration::XLFunctionRegistrationHelper
registerpvIcpClpLegs2("xlpvIcpClpLegs2",
"pvIcpClpLegs2",
"too lazy to comment this function ",
LibraryName,
pvIcpClpLegs2Args,
7
,false
,false
,""
,""
,false
,false
,false
);
}



extern "C"
{
LPXLFOPER EXCEL_EXPORT
xlpvIcpClpLegs2(
double valueDate,
LPXLFOPER holidaysa,
LPXLFOPER curveValuesa,
LPXLFOPER curveCharacteristicsa,
LPXLFOPER legCharacteristicsa,
LPXLFOPER customAmorta,
LPXLFOPER fixingsa)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);


XlfOper holidaysb(
	(holidaysa));
CellMatrix holidays(
	holidaysb.AsCellMatrix("holidays"));

XlfOper curveValuesb(
	(curveValuesa));
CellMatrix curveValues(
	curveValuesb.AsCellMatrix("curveValues"));

XlfOper curveCharacteristicsb(
	(curveCharacteristicsa));
CellMatrix curveCharacteristics(
	curveCharacteristicsb.AsCellMatrix("curveCharacteristics"));

XlfOper legCharacteristicsb(
	(legCharacteristicsa));
CellMatrix legCharacteristics(
	legCharacteristicsb.AsCellMatrix("legCharacteristics"));

XlfOper customAmortb(
	(customAmorta));
CellMatrix customAmort(
	customAmortb.AsCellMatrix("customAmort"));

XlfOper fixingsb(
	(fixingsa));
CellMatrix fixings(
	fixingsb.AsCellMatrix("fixings"));

CellMatrix result(
	pvIcpClpLegs2(
		valueDate,
		holidays,
		curveValues,
		curveCharacteristics,
		legCharacteristics,
		customAmort,
		fixings)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
pvIcpClfLegsArgs[]=
{
{ "valueDate","too lazy to comment this one ","B"},
{ "holidays","too lazy to comment this one ","XLF_OPER"},
{ "curveValues","too lazy to comment this one ","XLF_OPER"},
{ "curveCharacteristics","too lazy to comment this one ","XLF_OPER"},
{ "legCharacteristics","too lazy to comment this one ","XLF_OPER"},
{ "customAmort","too lazy to comment this one ","XLF_OPER"},
{ "icpFixings","too lazy to comment this one ","XLF_OPER"},
{ "ufFixings","too lazy to comment this one ","XLF_OPER"}
};
  XLRegistration::XLFunctionRegistrationHelper
registerpvIcpClfLegs("xlpvIcpClfLegs",
"pvIcpClfLegs",
"too lazy to comment this function ",
LibraryName,
pvIcpClfLegsArgs,
8
,false
,false
,""
,""
,false
,false
,false
);
}



extern "C"
{
LPXLFOPER EXCEL_EXPORT
xlpvIcpClfLegs(
double valueDate,
LPXLFOPER holidaysa,
LPXLFOPER curveValuesa,
LPXLFOPER curveCharacteristicsa,
LPXLFOPER legCharacteristicsa,
LPXLFOPER customAmorta,
LPXLFOPER icpFixingsa,
LPXLFOPER ufFixingsa)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);


XlfOper holidaysb(
	(holidaysa));
CellMatrix holidays(
	holidaysb.AsCellMatrix("holidays"));

XlfOper curveValuesb(
	(curveValuesa));
CellMatrix curveValues(
	curveValuesb.AsCellMatrix("curveValues"));

XlfOper curveCharacteristicsb(
	(curveCharacteristicsa));
CellMatrix curveCharacteristics(
	curveCharacteristicsb.AsCellMatrix("curveCharacteristics"));

XlfOper legCharacteristicsb(
	(legCharacteristicsa));
CellMatrix legCharacteristics(
	legCharacteristicsb.AsCellMatrix("legCharacteristics"));

XlfOper customAmortb(
	(customAmorta));
CellMatrix customAmort(
	customAmortb.AsCellMatrix("customAmort"));

XlfOper icpFixingsb(
	(icpFixingsa));
CellMatrix icpFixings(
	icpFixingsb.AsCellMatrix("icpFixings"));

XlfOper ufFixingsb(
	(ufFixingsa));
CellMatrix ufFixings(
	ufFixingsb.AsCellMatrix("ufFixings"));

CellMatrix result(
	pvIcpClfLegs(
		valueDate,
		holidays,
		curveValues,
		curveCharacteristics,
		legCharacteristics,
		customAmort,
		icpFixings,
		ufFixings)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
pvIcpClfLegs2Args[]=
{
{ "valueDate","too lazy to comment this one ","B"},
{ "holidays","too lazy to comment this one ","XLF_OPER"},
{ "curveValues","too lazy to comment this one ","XLF_OPER"},
{ "curveCharacteristics","too lazy to comment this one ","XLF_OPER"},
{ "legCharacteristics","too lazy to comment this one ","XLF_OPER"},
{ "customAmort","too lazy to comment this one ","XLF_OPER"},
{ "icpFixings","too lazy to comment this one ","XLF_OPER"},
{ "ufFixings","too lazy to comment this one ","XLF_OPER"}
};
  XLRegistration::XLFunctionRegistrationHelper
registerpvIcpClfLegs2("xlpvIcpClfLegs2",
"pvIcpClfLegs2",
"too lazy to comment this function ",
LibraryName,
pvIcpClfLegs2Args,
8
,false
,false
,""
,""
,false
,false
,false
);
}



extern "C"
{
LPXLFOPER EXCEL_EXPORT
xlpvIcpClfLegs2(
double valueDate,
LPXLFOPER holidaysa,
LPXLFOPER curveValuesa,
LPXLFOPER curveCharacteristicsa,
LPXLFOPER legCharacteristicsa,
LPXLFOPER customAmorta,
LPXLFOPER icpFixingsa,
LPXLFOPER ufFixingsa)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);


XlfOper holidaysb(
	(holidaysa));
CellMatrix holidays(
	holidaysb.AsCellMatrix("holidays"));

XlfOper curveValuesb(
	(curveValuesa));
CellMatrix curveValues(
	curveValuesb.AsCellMatrix("curveValues"));

XlfOper curveCharacteristicsb(
	(curveCharacteristicsa));
CellMatrix curveCharacteristics(
	curveCharacteristicsb.AsCellMatrix("curveCharacteristics"));

XlfOper legCharacteristicsb(
	(legCharacteristicsa));
CellMatrix legCharacteristics(
	legCharacteristicsb.AsCellMatrix("legCharacteristics"));

XlfOper customAmortb(
	(customAmorta));
CellMatrix customAmort(
	customAmortb.AsCellMatrix("customAmort"));

XlfOper icpFixingsb(
	(icpFixingsa));
CellMatrix icpFixings(
	icpFixingsb.AsCellMatrix("icpFixings"));

XlfOper ufFixingsb(
	(ufFixingsa));
CellMatrix ufFixings(
	ufFixingsb.AsCellMatrix("ufFixings"));

CellMatrix result(
	pvIcpClfLegs2(
		valueDate,
		holidays,
		curveValues,
		curveCharacteristics,
		legCharacteristics,
		customAmort,
		icpFixings,
		ufFixings)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
buildInterestRateLegArgs[]=
{
{ "startDate","too lazy to comment this one ","B"},
{ "endDate","too lazy to comment this one ","B"},
{ "calendars","too lazy to comment this one ","XLF_OPER"},
{ "settlementStubPeriod","too lazy to comment this one ","XLF_OPER"},
{ "settlementPeriodicity","too lazy to comment this one ","XLF_OPER"},
{ "endDateAdjustment","too lazy to comment this one ","XLF_OPER"},
{ "settlementCalendar","too lazy to comment this one ","XLF_OPER"},
{ "settlementLag","too lazy to comment this one ","B"},
{ "fixingStubPeriod","too lazy to comment this one ","XLF_OPER"},
{ "fixingPeriodicity","too lazy to comment this one ","XLF_OPER"},
{ "fixingLag","too lazy to comment this one ","B"},
{ "fixingCalendar","too lazy to comment this one ","XLF_OPER"},
{ "fixingStartDateRule","too lazy to comment this one ","B"},
{ "fixingTenor","too lazy to comment this one ","XLF_OPER"}
};
  XLRegistration::XLFunctionRegistrationHelper
registerbuildInterestRateLeg("xlbuildInterestRateLeg",
"buildInterestRateLeg",
"too lazy to comment this function ",
LibraryName,
buildInterestRateLegArgs,
14
,false
,false
,""
,""
,false
,false
,false
);
}



extern "C"
{
LPXLFOPER EXCEL_EXPORT
xlbuildInterestRateLeg(
double startDate,
double endDate,
LPXLFOPER calendarsa,
LPXLFOPER settlementStubPerioda,
LPXLFOPER settlementPeriodicitya,
LPXLFOPER endDateAdjustmenta,
LPXLFOPER settlementCalendara,
double settlementLaga,
LPXLFOPER fixingStubPerioda,
LPXLFOPER fixingPeriodicitya,
double fixingLaga,
LPXLFOPER fixingCalendara,
double fixingStartDateRulea,
LPXLFOPER fixingTenora)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);



XlfOper calendarsb(
	(calendarsa));
CellMatrix calendars(
	calendarsb.AsCellMatrix("calendars"));

XlfOper settlementStubPeriodb(
	(settlementStubPerioda));
string settlementStubPeriod(
	settlementStubPeriodb.AsString("settlementStubPeriod"));

XlfOper settlementPeriodicityb(
	(settlementPeriodicitya));
string settlementPeriodicity(
	settlementPeriodicityb.AsString("settlementPeriodicity"));

XlfOper endDateAdjustmentb(
	(endDateAdjustmenta));
string endDateAdjustment(
	endDateAdjustmentb.AsString("endDateAdjustment"));

XlfOper settlementCalendarb(
	(settlementCalendara));
string settlementCalendar(
	settlementCalendarb.AsString("settlementCalendar"));

int settlementLag(
	static_cast<int>(settlementLaga));

XlfOper fixingStubPeriodb(
	(fixingStubPerioda));
string fixingStubPeriod(
	fixingStubPeriodb.AsString("fixingStubPeriod"));

XlfOper fixingPeriodicityb(
	(fixingPeriodicitya));
string fixingPeriodicity(
	fixingPeriodicityb.AsString("fixingPeriodicity"));

int fixingLag(
	static_cast<int>(fixingLaga));

XlfOper fixingCalendarb(
	(fixingCalendara));
string fixingCalendar(
	fixingCalendarb.AsString("fixingCalendar"));

int fixingStartDateRule(
	static_cast<int>(fixingStartDateRulea));

XlfOper fixingTenorb(
	(fixingTenora));
string fixingTenor(
	fixingTenorb.AsString("fixingTenor"));

CellMatrix result(
	buildInterestRateLeg(
		startDate,
		endDate,
		calendars,
		settlementStubPeriod,
		settlementPeriodicity,
		endDateAdjustment,
		settlementCalendar,
		settlementLag,
		fixingStubPeriod,
		fixingPeriodicity,
		fixingLag,
		fixingCalendar,
		fixingStartDateRule,
		fixingTenor)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
pvFloatingRateLegsArgs[]=
{
{ "valueDate","too lazy to comment this one ","B"},
{ "holidays","too lazy to comment this one ","XLF_OPER"},
{ "curveValues","too lazy to comment this one ","XLF_OPER"},
{ "curveCharacteristics","too lazy to comment this one ","XLF_OPER"},
{ "legCharacteristics","too lazy to comment this one ","XLF_OPER"},
{ "customAmort","too lazy to comment this one ","XLF_OPER"},
{ "fixings","too lazy to comment this one ","XLF_OPER"},
{ "intRateIndexChars","too lazy to comment this one ","XLF_OPER"}
};
  XLRegistration::XLFunctionRegistrationHelper
registerpvFloatingRateLegs("xlpvFloatingRateLegs",
"pvFloatingRateLegs",
"too lazy to comment this function ",
LibraryName,
pvFloatingRateLegsArgs,
8
,false
,false
,""
,""
,false
,false
,false
);
}



extern "C"
{
LPXLFOPER EXCEL_EXPORT
xlpvFloatingRateLegs(
double valueDate,
LPXLFOPER holidaysa,
LPXLFOPER curveValuesa,
LPXLFOPER curveCharacteristicsa,
LPXLFOPER legCharacteristicsa,
LPXLFOPER customAmorta,
LPXLFOPER fixingsa,
LPXLFOPER intRateIndexCharsa)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);


XlfOper holidaysb(
	(holidaysa));
CellMatrix holidays(
	holidaysb.AsCellMatrix("holidays"));

XlfOper curveValuesb(
	(curveValuesa));
CellMatrix curveValues(
	curveValuesb.AsCellMatrix("curveValues"));

XlfOper curveCharacteristicsb(
	(curveCharacteristicsa));
CellMatrix curveCharacteristics(
	curveCharacteristicsb.AsCellMatrix("curveCharacteristics"));

XlfOper legCharacteristicsb(
	(legCharacteristicsa));
CellMatrix legCharacteristics(
	legCharacteristicsb.AsCellMatrix("legCharacteristics"));

XlfOper customAmortb(
	(customAmorta));
CellMatrix customAmort(
	customAmortb.AsCellMatrix("customAmort"));

XlfOper fixingsb(
	(fixingsa));
CellMatrix fixings(
	fixingsb.AsCellMatrix("fixings"));

XlfOper intRateIndexCharsb(
	(intRateIndexCharsa));
CellMatrix intRateIndexChars(
	intRateIndexCharsb.AsCellMatrix("intRateIndexChars"));

CellMatrix result(
	pvFloatingRateLegs(
		valueDate,
		holidays,
		curveValues,
		curveCharacteristics,
		legCharacteristics,
		customAmort,
		fixings,
		intRateIndexChars)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
pvFloatingRateLegs2Args[]=
{
{ "valueDate","too lazy to comment this one ","B"},
{ "holidays","too lazy to comment this one ","XLF_OPER"},
{ "curveValues","too lazy to comment this one ","XLF_OPER"},
{ "curveCharacteristics","too lazy to comment this one ","XLF_OPER"},
{ "legCharacteristics","too lazy to comment this one ","XLF_OPER"},
{ "customAmort","too lazy to comment this one ","XLF_OPER"},
{ "fixings","too lazy to comment this one ","XLF_OPER"},
{ "intRateIndexChars","too lazy to comment this one ","XLF_OPER"}
};
  XLRegistration::XLFunctionRegistrationHelper
registerpvFloatingRateLegs2("xlpvFloatingRateLegs2",
"pvFloatingRateLegs2",
"too lazy to comment this function ",
LibraryName,
pvFloatingRateLegs2Args,
8
,false
,false
,""
,""
,false
,false
,false
);
}



extern "C"
{
LPXLFOPER EXCEL_EXPORT
xlpvFloatingRateLegs2(
double valueDate,
LPXLFOPER holidaysa,
LPXLFOPER curveValuesa,
LPXLFOPER curveCharacteristicsa,
LPXLFOPER legCharacteristicsa,
LPXLFOPER customAmorta,
LPXLFOPER fixingsa,
LPXLFOPER intRateIndexCharsa)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);


XlfOper holidaysb(
	(holidaysa));
CellMatrix holidays(
	holidaysb.AsCellMatrix("holidays"));

XlfOper curveValuesb(
	(curveValuesa));
CellMatrix curveValues(
	curveValuesb.AsCellMatrix("curveValues"));

XlfOper curveCharacteristicsb(
	(curveCharacteristicsa));
CellMatrix curveCharacteristics(
	curveCharacteristicsb.AsCellMatrix("curveCharacteristics"));

XlfOper legCharacteristicsb(
	(legCharacteristicsa));
CellMatrix legCharacteristics(
	legCharacteristicsb.AsCellMatrix("legCharacteristics"));

XlfOper customAmortb(
	(customAmorta));
CellMatrix customAmort(
	customAmortb.AsCellMatrix("customAmort"));

XlfOper fixingsb(
	(fixingsa));
CellMatrix fixings(
	fixingsb.AsCellMatrix("fixings"));

XlfOper intRateIndexCharsb(
	(intRateIndexCharsa));
CellMatrix intRateIndexChars(
	intRateIndexCharsb.AsCellMatrix("intRateIndexChars"));

CellMatrix result(
	pvFloatingRateLegs2(
		valueDate,
		holidays,
		curveValues,
		curveCharacteristics,
		legCharacteristics,
		customAmort,
		fixings,
		intRateIndexChars)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

//////////////////////////
// Methods that will get registered to execute in AutoOpen
//////////////////////////

//////////////////////////
// Methods that will get registered to execute in AutoClose
//////////////////////////

