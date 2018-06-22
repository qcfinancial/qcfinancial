#include "ChileanFixedRateBond.h"
#include "PresentValue.h"

namespace QCode
{
	namespace Financial
	{
		ChileanFixedRateBond::ChileanFixedRateBond(Leg& fixedRateLeg,
			const QCInterestRate& tera) : FixedRateBond(fixedRateLeg), _tera(tera)
		{
		}
		
		double ChileanFixedRateBond::valorPar(const QCDate& valueDate)
		{
			for (size_t i = 0; i < _fixedRateLeg.size(); ++i)
			{
				auto flow = std::dynamic_pointer_cast<FixedRateCashflow>(_fixedRateLeg.getCashflowAt(i));
				if (flow->getStartDate() <= valueDate && valueDate <= flow->getEndDate())
				{
					auto valDate = valueDate;
					auto startDate = flow->getStartDate();
					return flow->getNominal() * _tera.wf(startDate, valDate);
				}
			}
			return 0.0;
		}

		double ChileanFixedRateBond::price(const QCDate& valueDate, const QCInterestRate& yieldToMaturity)
		{
			auto vPar = valorPar(valueDate);
			auto valDate = valueDate;
			auto ytm = yieldToMaturity;
			auto presentValue = PresentValue();
			auto pv = presentValue.pv(valDate, _fixedRateLeg, ytm);
			// La duración de Macaulay es:
			// D = - dV / dTir * (1 + Tir) / V(Tir)
			_duration = 0.0;
			for (const auto& der: presentValue.getDerivatives())
			{
				_duration += der;
			}
			_duration *= -1;
			_duration *= 1 + ytm.getValue();
			_duration /= pv;

			// La convexidad es:
			// C = (d2V/ dTir2) / V(Tir)
			_convexity = 0.0;
			for (const auto& der2 : presentValue.get2Derivatives())
			{
				_convexity += der2;
			}
			_convexity /= pv;

			return round((pv / vPar) * 10000) / 10000;
		}
		
		double ChileanFixedRateBond::settlementValue(double notional, std::shared_ptr<QCCurrency> currency,
			const QCDate& valueDate, const QCInterestRate& yieldToMaturity)
		{
			auto valDate = valueDate;
			auto ytm = yieldToMaturity;
			return currency->amount(price(valDate, ytm) * valorPar(valDate) * notional / 100);
		}
	}
}
