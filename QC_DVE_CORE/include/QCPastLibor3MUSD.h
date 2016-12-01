#pragma once

#include "QCPastFixing.h"

class QCPastLibor3MUSD :
	public QCPastFixing
{
public:
	QCPastLibor3MUSD();
	virtual void pastFixing(QCDate& startDate, QCDate& endDate,
		vector<double>& rates) override;
	~QCPastLibor3MUSD();
};

