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
		public:
			QuantoLinearInterestRateCashflow(std::shared_ptr<LinearInterestRateCashflow> cashflow, FXRateIndex fxRateIndex,
				const TimeSeries& fxRateIndexValues, QCDate fxRateFixingDate)
				: _cashflow(cashflow), _fxRateIndex(fxRateIndex), _fxRateIndexValues(fxRateIndexValues), _fxRateFixingDate(fxRateFixingDate)
			{}
			// Otrs constructor que reciba un shared_pointer de TimeSeries
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

			virtual shared_ptr<QCCurrency> getInitialCcy() const
			{
				return _cashflow->getInitialCcy();
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

			const QCDate& getFxRateIndexFixingDate() const
			{
				return _fxRateFixingDate;
			}

			virtual double getNominal() const override
			{
				return _cashflow->getNominal();
			}

			virtual double nominal(const QCDate& fecha) const
			{
				QCCurrencyConverter converter;
				double valor = _fxRateIndexValues.at(fecha);
				return converter.convert(_cashflow->nominal(fecha), _cashflow->ccy(), valor, _fxRateIndex);
			}

			virtual double getAmortization() const override
			{
				return _cashflow->getAmortization();
			}

			virtual double amortization() const override
			{
				if (_cashflow->doesAmortize())
				{
					double monto = _cashflow->amortization();
					double valor = _fxRateIndexValues.at(_fxRateFixingDate);
					QCCurrencyConverter converter;
					return converter.convert(monto, _cashflow->ccy(), valor, _fxRateIndex);
				}
				else
				{
					return 0.0;
				}
				
			}

			virtual bool doesAmortize() const override
			{
				return _cashflow->doesAmortize();
			}

			virtual double interest() override
			{
				double interes = _cashflow->interest();
				auto fechaInteres = _cashflow->getEndDate();
				auto valor = _fxRateIndexValues.at(_fxRateFixingDate);
				QCCurrencyConverter converter;
				return converter.convert(interes, _cashflow->ccy(), valor, _fxRateIndex);

			}

			virtual double interest(const TimeSeries& fixings) override
			{
				double interes = _cashflow->interest(fixings);
				auto valor = _fxRateIndexValues.at(_fxRateFixingDate);
				QCCurrencyConverter converter;
				return converter.convert(interes, _cashflow->ccy(), valor, _fxRateIndex);
			}

			virtual double fixing() override
			{
				return _cashflow->fixing();
			}

			virtual double fixing(const TimeSeries& fixings) override
			{
				return _cashflow->fixing(fixings);
			}

			virtual double accruedInterest(const QCDate& fecha) override
			{
				double interes = _cashflow->accruedInterest(fecha);
				auto valor = _fxRateIndexValues.at(_fxRateFixingDate);
				QCCurrencyConverter converter;
				return converter.convert(interes, _cashflow->ccy(), valor, _fxRateIndex);
			}

			virtual double accruedInterest(const QCDate& fecha, const TimeSeries& fixings) override
			{
				double interes = _cashflow->accruedInterest(fecha, fixings);
				auto valor = _fxRateIndexValues.at(_fxRateFixingDate);
				QCCurrencyConverter converter;
				return converter.convert(interes, _cashflow->ccy(), valor, _fxRateIndex);
			}

			virtual double accruedFixing(const QCDate& fecha) override
			{
				return _cashflow->accruedFixing(fecha);
			}

			virtual double accruedFixing(const QCDate& fecha, const TimeSeries& fixings) override
			{
				return _cashflow->accruedFixing(fecha, fixings);
			}

			virtual std::shared_ptr<LinearInterestRateCashflow> originalCashflow()
			{
				return _cashflow;
			}

		private:
			std::shared_ptr<LinearInterestRateCashflow> _cashflow;
			FXRateIndex _fxRateIndex;
			const TimeSeries& _fxRateIndexValues;
			QCDate _fxRateFixingDate;

		};
	}
}
#endif //QUANTO_H