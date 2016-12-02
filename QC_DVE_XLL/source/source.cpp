
#include<cppinterface.h>
#include <algorithm>
#include <string>

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
#include "QCInterestRateLeg.h"
#include "QCFixedRatePayoff.h"

using namespace std;

short // echoes a short
EchoShort(short x // number to be echoed
           )
{
    return x;
}

double suuma(double x, double y)
{
	QCTest qct;
	return qct.suma(x, y);
	//return x + y;
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

CellMatrix cashFlow(CellMatrix tablaDesarrollo, double tasa, string yf, string wf)
{
	QCInterestRatePeriods periods;
	int filas = tablaDesarrollo.RowsInStructure();

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
			QCDate{ (long)tablaDesarrollo(i, 7).NumericValue() }
		));
	}

	QCInterestRateLeg irLeg{ make_shared<QCInterestRatePeriods>(periods), 9 };
	QCIntrstRtLgShrdPtr irLegPtr = make_shared<QCInterestRateLeg>(irLeg);
	QCLinearWf wf;
	QCAct360 act360;
	QCWlthFctrShrdPtr wfPtr = make_shared<QCWealthFactor>(wf);
	QCYrFrctnShrdPtr act360Ptr = make_shared<QCYearFraction>(act360);
	QCInterestRate intRt{ .01, act360Ptr, wfPtr };
	QCIntrstRtShrdPtr intRtPtr = make_shared<QCInterestRate>(intRt);
	QCDate valueDate{ 1, 12, 2016 };
	QCFixedRatePayoff fxdRtPoff{ intRtPtr, irLegPtr, 0, valueDate };

	fxdRtPoff.
}
