#include "QCInterestRateBasisSwap.h"

QCInterestRateBasisSwap::QCInterestRateBasisSwap(vector<shared_ptr<QCFloatingRatePayoff>> basisSwaps) 
	: _basisSwaps(basisSwaps)
{

}

double QCInterestRateBasisSwap::marketValue()
{
	return _basisSwaps.at(0)->presentValue() + _basisSwaps.at(1)->presentValue();
}

QCDate QCInterestRateBasisSwap::getEndDate()
{
	QCDate endDate{ _basisSwaps.at(0)->getEndDate() };
	if (_basisSwaps.at(1)->getEndDate() > endDate)
	{
		endDate = _basisSwaps.at(1)->getEndDate();
	}
	return endDate;
}

bool QCInterestRateBasisSwap::operator<(const QCFloatingRatePayoff& other)
{
	return this->getEndDate() < other.getEndDate();
}

bool QCInterestRateBasisSwap::lessThan(shared_ptr<QCFloatingRatePayoff> lhs, shared_ptr<QCFloatingRatePayoff> rhs)
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
	return _basisSwaps.at(numPayoff)->getPvProjRateDerivativeAt(index);
}

QCInterestRateBasisSwap::~QCInterestRateBasisSwap()
{

}