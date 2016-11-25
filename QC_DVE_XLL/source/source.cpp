
#include<cppinterface.h>
#pragma warning (disable : 4996)

#include "QCTest.h"
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

