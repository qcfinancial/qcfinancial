#include "cashflows/IborCashflow.h"

#include<iostream>

namespace QCode
{
    namespace Financial
    {
        IborCashflow::IborCashflow(std::shared_ptr<InterestRateIndex> index,
                                   const QCDate& startDate,
                                   const QCDate& endDate,
                                   const QCDate& fixingDate,
                                   const QCDate& settlementDate,
                                   double nominal,
                                   double amortization,
                                   bool doesAmortize,
                                   shared_ptr<QCCurrency> currency,
                                   double spread,
                                   double gearing) :
                _index(index),
                _startDate(startDate),
                _endDate(endDate),
                _fixingDate(fixingDate),
                _settlementDate(settlementDate),
                _nominal(nominal),
                _amortization(amortization),
                _doesAmortize(doesAmortize),
                _currency(currency),
                _spread(spread),
                _gearing(gearing)
        {
#ifndef NO_CONSTRUCTOR_VALIDATION
            if (!_validate())
			{
				throw std::invalid_argument(_validateMsg);
			}
#endif
			_rateValue = index->getRate().getValue();
            _calculateInterest();
        }


        double IborCashflow::amount()
        {
            double amort{ 0.0 };
            if (_doesAmortize)
            {
                amort = _amortization;
            }
            return  amort + _interest;
        }


        shared_ptr<QCCurrency> IborCashflow::ccy()
        {
            return _currency;
        }


        QCDate IborCashflow::date()
        {
            return _settlementDate;
        }


        QCDate IborCashflow::getStartDate() const
        {
            return _startDate;
        }


        QCDate IborCashflow::getEndDate() const
        {
            return _endDate;
        }


        QCDate IborCashflow::getFixingDate() const
        {
            return _fixingDate;
        }

        QCDate  IborCashflow::getSettlementDate() const {
            return _settlementDate;
        }


        QCDate IborCashflow::getIndexStartDate()
        {
            return _index->getStartDate(_fixingDate);
        }


        QCDate IborCashflow::getIndexEndDate()
        {
            return _index->getEndDate(_fixingDate);
        }


        void IborCashflow::setNominal(double nominal)
        {
            _nominal = nominal;
        }


        double IborCashflow::getNominal() const
        {
            return _nominal;
        }


        void IborCashflow::setAmortization(double amortization)
        {
            _amortization = amortization;
        }


        double IborCashflow::getAmortization() const
        {
            return _amortization;
        }


        void IborCashflow::setInterestRateValue(double value)
        {
            _rateValue = value;
            _index->setRateValue(_rateValue);
            _calculateInterest();
        }

        void QCode::Financial::IborCashflow::setForwardRateWfDerivatives(const std::vector<double> &der) {
            _forwardRateWfDerivatives.resize(der.size());
            for (size_t i = 0; i < der.size(); ++i) {
                _forwardRateWfDerivatives.at(i) = der.at(i);
            }
        }

        std::vector<double> QCode::Financial::IborCashflow::getAmountDerivatives() const {
            return _amountDerivatives;
        }


        std::shared_ptr<InterestRateIndex> QCode::Financial::IborCashflow::getInterestRateIndex() const {
            return _index;
        }


        double IborCashflow::getInterestRateValue() const
        {
            return _rateValue;
        }


        double IborCashflow::accruedInterest(const QCDate& valueDate)
        {
            if (Cashflow::isExpired(valueDate) || valueDate < _startDate)
            {
                return 0.0;
            }
            QCDate temp = valueDate;
            double indexValue = _index->getRate().getValue();
            _index->setRateValue(indexValue * _gearing + _spread);
            double result = _nominal * (_index->getRate().wf(_startDate, temp) - 1.0);
            _index->setRateValue(indexValue);
            return result;
        }


        shared_ptr<IborCashflowWrapper> IborCashflow::wrap()
        {
            IborCashflowWrapper tup = std::make_tuple(
                    _startDate,
                    _endDate,
                    _fixingDate,
                    _settlementDate,
                    _nominal,
                    _amortization,
                    _interest,
                    _doesAmortize,
                    amount(),
                    _currency,
                    _index->getCode(),
                    _index->getRate(),
                    _spread,
                    _gearing,
                    _rateValue);

            return std::make_shared<IborCashflowWrapper>(tup);
        }


        void IborCashflow::_calculateInterest()
        {
            _index->setRateValue(_rateValue * _gearing + _spread);
            // Con este procedimiento vamos a tener un problema al momento de calcular
            // derivadas. Se dará de alta una sobrecarga de QCInterestRate.wf que considere
            // también gearing y spread (o se agregan como parámetros opcionales en el
            // método ya existente).
            _interest = _nominal * (_index->getRate().wf(_startDate, _endDate) - 1.0);
            _index->setRateValue(_rateValue);
        }


        bool IborCashflow::_validate()
        {
            bool result;
            _validateMsg = "";
            if (_startDate >= _endDate)
            {
                result = false;
                _validateMsg += "Start date (" + _startDate.description();
                _validateMsg += ") is gt or eq to end date (" + _endDate.description() + ").";
            }
            if (_fixingDate > _startDate)
            {
                result = false;
                _validateMsg += "Fixing date (" + _fixingDate.description() + ") ";
                _validateMsg +=	"is gt start date (" + _startDate.description() + ").";
            }
            if (_settlementDate < _endDate)
            {
                result = false;
                _validateMsg += "Settlement date (" + _settlementDate.description() + ") ";
                _validateMsg +=	"is lt end date (" + _endDate.description() + ").";
            }
            return result;
        }


        IborCashflow::~IborCashflow() = default;

        std::string IborCashflow::getType() const {
            return "IborCashflow";
        }

        double IborCashflow::settlementAmount() {
            return ccy()->amount(amount());
        }

        shared_ptr<QCCurrency> IborCashflow::settlementCurrency() {
            return ccy();
        }

        std::string IborCashflow::getInterestRateIndexCode() const {
            return _index->getCode();
        }
    }
}
