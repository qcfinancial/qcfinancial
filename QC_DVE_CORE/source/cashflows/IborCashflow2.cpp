//
// Created by Alvaro Diaz on 2019-07-28.
//

#include "cashflows/IborCashflow2.h"

QCode::Financial::IborCashflow2::IborCashflow2(
        shared_ptr<QCode::Financial::InterestRateIndex> index,
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
    _fixingDates.resize(1);
    _fixingDates.at(0) = _fixingDate;
}

std::shared_ptr<QCode::Financial::InterestRateIndex> QCode::Financial::IborCashflow2::getInterestRateIndex()
{
    return _index;
}


double QCode::Financial::IborCashflow2::amount()
{
    double amort{ 0.0 };
    if (_doesAmortize)
    {
        amort = _amortization;
    }
    return  amort + _interest;
}


shared_ptr<QCCurrency> QCode::Financial::IborCashflow2::ccy()
{
    return _currency;
}


QCDate QCode::Financial::IborCashflow2::date()
{
    return _settlementDate;
}


string QCode::Financial::IborCashflow2::getType() const
{
    return "IBOR";
}


shared_ptr<QCCurrency> QCode::Financial::IborCashflow2::getInitialCcy() const
{
    return _currency;
}


const QCDate& QCode::Financial::IborCashflow2::getStartDate() const
{
    return _startDate;
}


const QCDate& QCode::Financial::IborCashflow2::getEndDate() const
{
    return _endDate;
}


const QCDate& QCode::Financial::IborCashflow2::getSettlementDate() const
{
    return _settlementDate;
}


const QCode::Financial::DateList& QCode::Financial::IborCashflow2::getFixingDates() const
{
    return _fixingDates;
}


double QCode::Financial::IborCashflow2::getNominal() const
{
    return _nominal;
}


double QCode::Financial::IborCashflow2::nominal(const QCDate& fecha) const
{
    return _nominal;
}


double QCode::Financial::IborCashflow2::getAmortization() const
{
    return _amortization;
}


double QCode::Financial::IborCashflow2::amortization() const
{
    return _amortization;
}


double QCode::Financial::IborCashflow2::interest()
{
    _calculateInterest();
    return _interest;
}


double QCode::Financial::IborCashflow2::interest(const QCode::Financial::TimeSeries& fixings)
{
    try
    {
        _rateValue = fixings.at(_fixingDate);
    }
    catch (const exception& e)
    {
        throw invalid_argument("Interest rate index value not found for fixing date " +
        _fixingDate.description(false) + ".");
    }
    _calculateInterest();
    return _interest;
}


double QCode::Financial::IborCashflow2::fixing()
{
    return _rateValue;
}


double QCode::Financial::IborCashflow2::fixing(const QCode::Financial::TimeSeries& fixings)
{
    try
    {
        _rateValue = fixings.at(_fixingDate);
    }
    catch (const exception& e)
    {
        throw invalid_argument("Interest rate index value not found for fixing date " +
                               _fixingDate.description(false) + ".");
    }
    return _rateValue;
}


double QCode::Financial::IborCashflow2::accruedInterest(const QCDate& fecha)
{
    if ((fecha < _startDate) || (_endDate <= fecha))
    {
        return 0.0;
    }
    _index->setRateValue(_rateValue * _gearing + _spread);
    // Con este procedimiento vamos a tener un problema al momento de calcular
    // derivadas. Se dará de alta una sobrecarga de QCInterestRate.wf que considere
    // también gearing y spread (o se agregan como parámetros opcionales en el
    // método ya existente).
    QCDate f = fecha;
    double interest = _nominal * (_index->getRate().wf(_startDate, f) - 1.0);
    _index->setRateValue(_rateValue);
    return interest;
}


double QCode::Financial::IborCashflow2::accruedInterest(const QCDate& fecha,
        const QCode::Financial::TimeSeries& fixings)
{
    if (fecha < _startDate || _endDate >= fecha)
    {
        return 0.0;
    }
    try
    {
        _rateValue = fixings.at(_fixingDate);
    }
    catch (const exception& e)
    {
        throw invalid_argument("Interest rate index value not found for fixing date " +
                               _fixingDate.description(false) + ".");
    }
    return accruedInterest(fecha);
}


double QCode::Financial::IborCashflow2::accruedFixing(const QCDate& fecha)
{
    return _rateValue;
}


double QCode::Financial::IborCashflow2::accruedFixing(const QCDate& fecha, const QCode::Financial::TimeSeries& fixings)
{
    if (fecha < _startDate || _endDate >= fecha)
    {
        return 0.0;
    }
    try
    {
        _rateValue = fixings.at(_fixingDate);
    }
    catch (const exception& e)
    {
        throw invalid_argument("Interest rate index value not found for fixing date " +
                               _fixingDate.description(false) + ".");
    }
    return _rateValue;
}


bool QCode::Financial::IborCashflow2::doesAmortize() const
{
    return _doesAmortize;
}


double QCode::Financial::IborCashflow2::getSpread() const
{
    return _spread;
}


double QCode::Financial::IborCashflow2::getGearing() const
{
    return _gearing;
}


std::shared_ptr<QCode::Financial::IborCashflow2::IborCashflow2Wrapper> QCode::Financial::IborCashflow2::wrap()
{
    QCode::Financial::IborCashflow2::IborCashflow2Wrapper tup = std::make_tuple(_startDate,
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

    return std::make_shared<QCode::Financial::IborCashflow2::IborCashflow2Wrapper>(tup);

}


void QCode::Financial::IborCashflow2::_calculateInterest()
{
    _index->setRateValue(_rateValue * _gearing + _spread);
    // Con este procedimiento vamos a tener un problema al momento de calcular
    // derivadas. Se dará de alta una sobrecarga de QCInterestRate.wf que considere
    // también gearing y spread (o se agregan como parámetros opcionales en el
    // método ya existente).
    _interest = _nominal * (_index->getRate().wf(_startDate, _endDate) - 1.0);
    _index->setRateValue(_rateValue);
}


bool QCode::Financial::IborCashflow2::_validate()
{
    bool result = true;
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


QCode::Financial::IborCashflow2::~IborCashflow2()
{
}







