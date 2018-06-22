#include "FixedRateBond.h"
#include "PresentValue.h"

namespace QCode
{
	namespace Financial
	{
		FixedRateBond::FixedRateBond(Leg& fixedRateLeg) :_fixedRateLeg(fixedRateLeg),
			_duration(0.0), _convexity(0.0)
		{
		}

		double FixedRateBond::presentValue(const QCDate& valueDate,
			const QCInterestRate& yieldToMaturity)
		{
			auto valDate = valueDate;
			auto ytm = yieldToMaturity;
			auto pv = PresentValue();
			auto result = pv.pv(valDate, _fixedRateLeg, ytm);
			// La duración de Macaulay es:
			// D = - dV / dTir * (1 + Tir) / V(Tir)
			_duration = 0.0;
			for (const auto& der : pv.getDerivatives())
			{
				_duration += der;
			}
			_duration *= -1;
			_duration *= 1 + ytm.getValue();
			_duration /= result;

			// La convexidad es:
			// C = (d2V/ dTir2) / V(Tir)
			_convexity = 0.0;
			for (const auto& der2 : pv.get2Derivatives())
			{
				_convexity += der2;
			}
			_convexity /= result;
			
			return result;
		}

		double FixedRateBond::price(const QCDate& valueDate, const QCInterestRate& yieldToMaturity)
		{
			auto valDate = valueDate;
			auto ytm = yieldToMaturity;
			return presentValue(valDate, ytm) - accruedInterest(valDate);
		}

		double FixedRateBond::accruedInterest(const QCDate& valueDate)
		{
			for (size_t i = 0; i < _fixedRateLeg.size(); ++i)
			{
				auto flow = std::dynamic_pointer_cast<FixedRateCashflow>(_fixedRateLeg.getCashflowAt(i));
				if (flow->getStartDate() <= valueDate && valueDate <= flow->getEndDate())
				{
					return flow->accruedInterest(valueDate);
				}
			}
			return 0.0;
		}

		double FixedRateBond::duration() const
		{
			return  _duration;
		}

		double FixedRateBond::convexity() const
		{
			return _convexity;
		}

		Leg FixedRateBond::getLeg() const
		{
			return _fixedRateLeg;
		}

	}
}



