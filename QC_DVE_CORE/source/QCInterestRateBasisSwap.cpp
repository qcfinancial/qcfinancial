#include "QCInterestRateBasisSwap.h"

QCInterestRateBasisSwap::QCInterestRateBasisSwap(vector<shared_ptr<QCFloatingRatePayoff>> basisSwaps) 
	: _basisSwaps(basisSwaps)
{

}

double QCInterestRateBasisSwap::marketValue()
{
	bool verbose = false;
	if (verbose)
	{
		cout << "Basis Swap Market Value" << endl;
		cout << "Leg 0: " << _basisSwaps.at(0)->presentValue() << endl;
		cout << "Leg 1: " << _basisSwaps.at(1)->presentValue() << endl;
	}
	return _basisSwaps.at(0)->presentValue() + _basisSwaps.at(1)->presentValue();
}

QCDate QCInterestRateBasisSwap::getStartDate() const
{
	QCDate startDate{ _basisSwaps.at(0)->getStartDate() };
	if (_basisSwaps.at(1)->getStartDate() < startDate)
	{
		startDate = _basisSwaps.at(1)->getStartDate();
	}
	return startDate;
}

QCDate QCInterestRateBasisSwap::getEndDate() const
{
	QCDate endDate{ _basisSwaps.at(0)->getEndDate() };
	if (_basisSwaps.at(1)->getEndDate() > endDate)
	{
		endDate = _basisSwaps.at(1)->getEndDate();
	}
	return endDate;
}

bool QCInterestRateBasisSwap::operator<(const QCInterestRateBasisSwap& other)
{
	return this->getEndDate() < other.getEndDate();
}

bool QCInterestRateBasisSwap::lessThan(shared_ptr<QCInterestRateBasisSwap> lhs,
	shared_ptr<QCInterestRateBasisSwap> rhs)
{
	return lhs->getEndDate() < rhs->getEndDate();
}

shared_ptr<QCFloatingRatePayoff> QCInterestRateBasisSwap::getPayoff(unsigned int numPayoff)
{
	if (numPayoff > 1) numPayoff = 0;
	return _basisSwaps.at(numPayoff);
}

double QCInterestRateBasisSwap::getPvRateDerivativeAtFromLeg(unsigned int index, unsigned int numPayoff)
{
	return _basisSwaps.at(numPayoff)->getPvRateDerivativeAt(index);
}

double QCInterestRateBasisSwap::getProjRateDerivativeAtFromLeg(unsigned int index, unsigned int numPayoff)
{
	bool verbose = false;
	if (verbose)
	{
		cout << "derivative: " << _basisSwaps.at(numPayoff)->getPvProjRateDerivativeAt(index) << endl;
	}
	return _basisSwaps.at(numPayoff)->getPvProjRateDerivativeAt(index);
}

QCInterestRateBasisSwap::~QCInterestRateBasisSwap()
{

}