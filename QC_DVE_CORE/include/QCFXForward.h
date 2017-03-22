#ifndef QCFXFORWARD_H
#define QCFXFORWARD_H

#include "QCOperation.h"
#include "QCDiscountBondPayoff.h"
#include "QCCurrencyConverter.h"-

class QCFXForward : public QCOperation
{
public:
	QCFXForward(vector<shared_ptr<QCDiscountBondPayoff>> legs,
		QCCurrencyConverter::QCCurrency marketValueCurrency,
		shared_ptr<map<QCCurrencyConverter::QCFxRate, double>> fxRates);

	virtual double marketValue() override;

	virtual ~QCFXForward();

protected:
	vector<shared_ptr<QCDiscountBondPayoff>> _legs;
	QCCurrencyConverter::QCCurrency _marketValueCurrency;
	shared_ptr<map<QCCurrencyConverter::QCFxRate, double>> _fxRates;

};

#endif //QCFXFORWARD_H

