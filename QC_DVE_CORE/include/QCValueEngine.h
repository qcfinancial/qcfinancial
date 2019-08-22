#ifndef QCVALUEENGINE_H
#define QCVALUEENGINE_H

#include <string>
#include <vector>
#include <map>

//ESTA CLASE ESTA EN DESARROLLO

#include "time/QCDate.h"

class QCValueEngine
{
	typedef tuple<string, string, string, string> string4;

public:
	QCValueEngine(map<string, vector<QCDate>>& mapHolidays,
		map<string, pair<vector<long>, vector<double>>>& crvValues,
		map<string, string4>& crvChars);
private:
	const map<string, vector<QCDate>>& _mapHolidays;
	const map<string, pair<vector<long>, vector<double>>>& _crvValues;
	const map<string, string4>& _crvChars;


};
#endif //QCVALUEENGINE_H