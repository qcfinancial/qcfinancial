#ifndef QCINTERESTRATEPERIODSFACTORY_H
#define QCINTERESTRATEPERIODSFACTORY_H

#include <string>
#include <tuple>

#include "QCInterestRateLeg.h"
#include "QCDate.h"

using namespace std;

class QCInterestRatePeriodsFactory
{
public:
	QCInterestRatePeriodsFactory(QCDate startDate, QCDate endDate,
		QCDate::QCBusDayAdjRules endDateAdjustment,
		string settlementPeriodicity,
		QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
		shared_ptr<vector<QCDate>> settlementCalendar,
		unsigned int settlementLag,
		string fixingPeriodicity,
		QCInterestRateLeg::QCStubPeriod fixingStubPeriod,
		shared_ptr<vector<QCDate>> fixingCalendar,
		unsigned int fixingLag,
		unsigned int indexStartDateLag,
		string indexTenor
		);

	QCInterestRateLeg::QCInterestRatePeriods getPeriods();

	~QCInterestRatePeriodsFactory();

private:
	QCDate _startDate;
	QCDate _endDate;
	QCDate::QCBusDayAdjRules _endDateAdjustment;

	string _settlementPeriodicity;
	QCInterestRateLeg::QCStubPeriod _settlementStubPeriod;
	shared_ptr<vector<QCDate>> _settlementCalendar;
	unsigned int _settlementLag;

	string _fixingPeriodicity;
	QCInterestRateLeg::QCStubPeriod _fixingStubPeriod;
	shared_ptr<vector<QCDate>> _fixingCalendar;
	unsigned int _fixingLag;
	unsigned int _indexStartDateLag;
	string _indexTenor;
	
	vector<tuple<QCDate, QCDate>> _settlementBasicDates;
	vector<tuple<QCDate, QCDate>> _fixingBasicDates;

	vector<tuple<QCDate, QCDate>> _buildBasicDates(string periodicity,
		QCInterestRateLeg::QCStubPeriod stubPeriod, shared_ptr<vector<QCDate>> calendar);

};

#endif //QCINTERESTRATEPERIODSFACTORY_H