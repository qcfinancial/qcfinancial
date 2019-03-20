#ifndef QUANTO_H
#define QUANTO_H

#include <memory>

#include "Cashflow.h"
#include "LinearInterestRateCashflow.h"
#include "FXRateIndex.h"
#include "TypeAliases.h"
#include "QCCurrencyConverter.h"

namespace QCode
{
	namespace Financial
	{
		class QuantoCashflow : public Cashflow
		{
		public:
			QuantoCashflow(std::shared_ptr<Cashflow> cashflow, FXRateIndex fxRateIndex, const TimeSeries& fxRateIndexValues, QCDate fxRateFixingDate)
				: _cashflow(cashflow), _fxRateIndex(fxRateIndex), _fxRateIndexValues(fxRateIndexValues), _fxRateFixingDate(fxRateFixingDate)
			{}

			double amount() override
			{
				QCCurrencyConverter converter;
				double amount = _cashflow->amount();
				auto fecha = _cashflow->date();
				auto valor = _fxRateIndexValues.at(fecha);
				return converter.convert(amount, _cashflow->ccy(), valor, _fxRateIndex);
			}

			virtual shared_ptr<QCCurrency> ccy() override
			{
				auto originalCurrency = _cashflow->ccy()->getIsoCode();
				if (originalCurrency == _fxRateIndex.strongCcyCode())
				{
					return _fxRateIndex.getFxRate()->getWeakCcy();
				}
				else
				{
					return _fxRateIndex.getFxRate()->getStrongCcy();
				}
			}

			virtual QCDate date() override
			{
				return _cashflow->date();
			}

		private:
			std::shared_ptr<Cashflow> _cashflow;
			FXRateIndex _fxRateIndex;
			TimeSeries _fxRateIndexValues;
			QCDate _fxRateFixingDate;
		};

		class QuantoLinearInterestRateCashflow : public LinearInterestRateCashflow
		{
			QuantoLinearInterestRateCashflow(std::shared_ptr<LinearInterestRateCashflow> cashflow, FXRateIndex fxRateIndex,
				const TimeSeries& fxRateIndexValues, QCDate fxRateFixingDate)
				: _cashflow(cashflow), _fxRateIndex(fxRateIndex), _fxRateIndexValues(fxRateIndexValues), _fxRateFixingDate(fxRateFixingDate)
			{}

			double amount() override
			{
				QCCurrencyConverter converter;
				double amount = _cashflow->amount();
				auto fecha = _cashflow->date();
				auto valor = _fxRateIndexValues.at(fecha);
				return converter.convert(amount, _cashflow->ccy(), valor, _fxRateIndex);
			}

			virtual shared_ptr<QCCurrency> ccy() override
			{
				auto originalCurrency = _cashflow->ccy()->getIsoCode();
				if (originalCurrency == _fxRateIndex.strongCcyCode())
				{
					return _fxRateIndex.getFxRate()->getWeakCcy();
				}
				else
				{
					return _fxRateIndex.getFxRate()->getStrongCcy();
				}
			}

			virtual QCDate date() override
			{
				return _cashflow->date();
			}

			virtual const QCDate& getStartDate() const override
			{
				return _cashflow->getStartDate();
			}

			virtual const QCDate& getEndDate() const override
			{
				return _cashflow->getEndDate();
			}

			virtual const QCDate& getSettlementDate() const override
			{
				return _cashflow->getSettlementDate();
			}

			virtual const DateList& getFixingDates() const override
			{
				return _cashflow->getFixingDates();
			}

			virtual double getNominal() const override
			{
				return _cashflow->getNominal();
			}

			virtual double getAmortization() const
			{
				if (_cashflow->doesAmortize())
				{
					double amortization = _cashflow->getAmortization();
					auto fecha = _cashflow->getSettlementDate();
					auto valor = _fxRateIndexValues.at(fecha);
					QCCurrencyConverter converter;
					return converter.convert(amortization, _cashflow->ccy(), valor, _fxRateIndex);
				}
				else
				{
					return 0.0;
				}
			}

		private:
			std::shared_ptr<LinearInterestRateCashflow> _cashflow;
			FXRateIndex _fxRateIndex;
			TimeSeries _fxRateIndexValues;
			QCDate _fxRateFixingDate;

		};
	}
}
#endif //QUANTO_H