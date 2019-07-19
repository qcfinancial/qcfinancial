#ifndef FIXEDRATECASHFLOWWRAPPER_H
#define FIXEDRATECASHFLOWWRAPPER_H

#include<tuple>
#include<memory>

#include "FixedRateCashflow.h"

typedef std::tuple<QCDate, QCDate, QCDate, double, double, double, bool, shared_ptr<QCCurrency>> elmnt;

class FixedRateCashflowWrapper
{
public:
	FixedRateCashflowWrapper(const QCDate& startDate,
							 const QCDate& endDate,
							 const QCDate& settlementDate,
							 double nominal,
							 double amortization,
							 bool doesAmortize,
							 const QCInterestRate& rate,
							 shared_ptr<QCCurrency> currency)
	{
		_wrapper = std::make_tuple(startDate,
							  endDate,
							  settlementDate,
							  nominal,
							  amortization,
							  doesAmortize,
							  rate,
							  currency);
	}

private:
	elmnt _wrapper;
};
#endif //FIXEDRATECASHFLOWWRAPPER_H