#include "QCPastLibor3MUSD.h"


QCPastLibor3MUSD::QCPastLibor3MUSD()
{
}

void QCPastLibor3MUSD::pastFixing(QCDate& startDate, QCDate& endDate,
	vector<double>& rates)
{
	rates.resize(0);
	rates.at(0) = .01;

	//Aqui en la practica se dara de alta un objeto de la 
	//clase DataAccess

	//O simplemente 
}

QCPastLibor3MUSD::~QCPastLibor3MUSD()
{
}
