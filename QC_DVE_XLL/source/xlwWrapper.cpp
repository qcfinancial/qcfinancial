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
EchoShortArgs[]=
{
{ "x"," number to be echoed ","XLF_OPER"}
};
  XLRegistration::XLFunctionRegistrationHelper
registerEchoShort("xlEchoShort",
"EchoShort",
" echoes a short ",
LibraryName,
EchoShortArgs,
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
xlEchoShort(
LPXLFOPER xa)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

XlfOper xb(
	(xa));
short x(
	xb.AsShort("x"));

short result(
	EchoShort(
		x)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
suumaArgs[]=
{
{ "x","primer numero ","B"},
{ "y","segundo numero ","B"}
};
  XLRegistration::XLFunctionRegistrationHelper
registersuuma("xlsuuma",
"suuma",
"Devuelve la suma de 2 numeros ",
LibraryName,
suumaArgs,
2
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
xlsuuma(
double x,
double y)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);



double result(
	suuma(
		x,
		y)
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
pvArgs[]=
{
{ "tablaDesarrollo","Tabla de desarrollo de la pata ","XLF_OPER"},
{ "curva","Curva cero cupon (rango con tenors y tasas) ","XLF_OPER"},
{ "tasa","valor de la tasa fija de la pata ","B"},
{ "fecha","fecha de valorizacion ","B"},
{ "yf","nombre de la fraccion de agno de la tasa fija ","XLF_OPER"},
{ "wf","nombre del tipo de factor de capitalizacion ","XLF_OPER"}
};
  XLRegistration::XLFunctionRegistrationHelper
registerpv("xlpv",
"pv",
"too lazy to comment this function ",
LibraryName,
pvArgs,
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
xlpv(
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
	pv(
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

//////////////////////////
// Methods that will get registered to execute in AutoOpen
//////////////////////////

//////////////////////////
// Methods that will get registered to execute in AutoClose
//////////////////////////

