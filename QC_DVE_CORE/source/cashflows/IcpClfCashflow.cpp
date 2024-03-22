#include "cashflows/IcpClfCashflow.h"

namespace QCode
{
	namespace Financial
	{
		IcpClfCashflow::IcpClfCashflow(
                const QCDate &startDate,
                const QCDate &endDate,
                const QCDate &settlementDate,
                double nominal,
                double amortization,
                bool doesAmortize,
                double spread,
                double gearing,
                std::vector<double> icpAndUf) :
                IcpClpCashflow(
                        startDate,
                        endDate,
                        settlementDate,
                        nominal,
                        amortization,
                        doesAmortize,
                        spread,
                        gearing,
                        icpAndUf[0],
                        icpAndUf[1]),
                _startDateUF(icpAndUf[2]),
                _endDateUF(icpAndUf[3]) {
#ifndef NO_CONSTRUCTOR_VALIDATION
			if (!_validate())
			{
				throw std::invalid_argument(_validateMsg);
			}
#endif
			_currency = std::make_shared<QCCurrency>(QCCLF());
			_traDecimalPlaces = DEFAULT_TRA_DECIMAL_PLACES;
			_rate.setValue(getTra(_endDate, icpAndUf[1], icpAndUf[3]));
		}

        double IcpClfCashflow::amount()
        {
		    // Se calcula el interés sin considerar los redondeos de TRA y TNA
            auto interest = _nominal * (_endDateICP / _startDateICP * _startDateUF / _endDateUF - 1.0);
            interest += _nominal * _spread * _rate.yf(_startDate, _endDate);

            // Derivadas respecto a la curva de proyección de ICP
            size_t lengthICP = _startDateICPDerivatives.size();
            _amountICPDerivatives.resize(lengthICP);
            for (size_t i = 0; i < lengthICP; ++i)
            {
                _amountICPDerivatives.at(i) = _nominal * ((_endDateICPDerivatives.at(i) * _startDateICP -
                        _startDateICPDerivatives.at(i) * _endDateICP) * pow(_startDateICP, -2.0) *
                                _startDateUF / _endDateUF);
            }

            // Derivadas respecto a la curva CLP en la proyección de UF
            size_t lengthUFCLP = _startDateUFCLPDerivatives.size();
            _amountUFCLPDerivatives.resize(lengthUFCLP);
            auto icpForward = _endDateICP / _startDateICP;
            for (size_t i = 0; i < lengthUFCLP; ++i)
            {
                _amountUFCLPDerivatives.at(i) = _nominal * ((_startDateUFCLPDerivatives.at(i) * _endDateUF -
                        _endDateUFCLPDerivatives.at(i) * _startDateUF) *
                                pow(_endDateUF, -2.0) * icpForward);
            }

            // Derivadas respecto a la curva CLF en la proyección de UF
            size_t lengthUFCLF = _startDateUFCLFDerivatives.size();
            _amountUFCLFDerivatives.resize(lengthUFCLF);
            for (size_t i = 0; i < lengthUFCLF; ++i)
            {
                _amountUFCLFDerivatives.at(i) = _nominal * ((_startDateUFCLFDerivatives.at(i) * _endDateUF -
                                                  _endDateUFCLFDerivatives.at(i) * _startDateUF) *
                                                          pow(_endDateUF, -2.0) * icpForward);
            }

            // Se agrega la amortización si corresponde
            if (_doesAmortize)
            {
                return _amortization + interest;
            }
            else
            {
                return interest;
            }
        }

        void IcpClfCashflow::setTraDecimalPlaces(unsigned int decimalPlaces)
		{
			_traDecimalPlaces = decimalPlaces;
			_rate.setValue(getTra(_endDate, _endDateICP, _endDateUF));
		}

		double IcpClfCashflow::getTra(QCDate& accrualDate, double icpValue, double ufValue)
		{
			double tna = getTna(accrualDate, icpValue);
			IcpClpCashflow::_rate.setValue(tna);
			double wfTna = IcpClpCashflow::_rate.wf(_startDate, accrualDate);
			double tra = (wfTna * _startDateUF / ufValue - 1) * 360.0 / _startDate.dayDiff(accrualDate);
			double factor = pow(10, _traDecimalPlaces);
			return round(tra * factor) / factor;
		}

		double IcpClfCashflow::getRateValue()
		{
			return _rate.getValue();
		}

		double IcpClfCashflow::_calculateInterest(QCDate& date, double icpValue, double ufValue)
		{
			// Cálculo de TRA
			double tra = getTra(date, icpValue, ufValue);

			// Seteo del valor de la tasa interna
			_rate.setValue(tra * _gearing + _spread);

			// Cálculo de inter�s
			return _nominal * (_rate.wf(_startDate, date) - 1);
		}

		double IcpClfCashflow::accruedInterest(QCDate& accrualDate, double icpValue, double ufValue)
		{
			return _calculateInterest(accrualDate, icpValue, ufValue);
		}

		void IcpClfCashflow::setStartDateUf(double ufValue)
		{
			_startDateUF = ufValue;
			_rate.setValue(getTra(_endDate, _endDateICP, _endDateUF));
		}

		double IcpClfCashflow::getStartDateUf() const
		{
			return _startDateUF;
		}

		void IcpClfCashflow::setEndDateUf(double ufValue)
		{
			_endDateUF = ufValue;
			_rate.setValue(getTra(_endDate, _endDateICP, _endDateUF));
		}

		double IcpClfCashflow::getEndDateUf() const
		{
			return _endDateUF;
		}

		shared_ptr<QCCurrency> IcpClfCashflow::ccy()
		{
			return _currency;
		}

		shared_ptr<QCCurrency> IcpClfCashflow::settlementCurrency()
		{
			auto clp = QCCLP();
			return std::make_shared<QCCLP>(clp);
		}

		shared_ptr<IcpClfCashflowWrapper> IcpClfCashflow::wrap()
		{
			    //QCDate,                 /* Start Date */
				//QCDate,                 /* End Date */
				//QCDate,                 /* Settlement Date */
				//double,                 /* Nominal */
				//double,                 /* Amortization */
				//bool,                   /* Amortization is cashflow */
				//shared_ptr<QCCurrency>, /* Nominal Currency (always CLP) */
				//double,                 /* Start date ICP value */
				//double,                 /* End date ICP value */
				//double,                 /* Start date UF value */
				//double,                 /* End date UF value */
				//double,                 /* Rate */
				//double,                 /* Interest */
				//double,                 /* Spread */
				//double                  /* Gearing */

			//Se precalcula el interés porque eso permite asegurar que el valor
			//de la tasa es consistente con los valores de ICP y UF ingresados.
			double interest = accruedInterest(_endDate, _endDateICP, _endDateUF);
			IcpClfCashflowWrapper tup = std::make_tuple(
                    _startDate,
                    _endDate,
                    _settlementDate,
                    _nominal,
                    _amortization,
                    _doesAmortize,
                    _currency,
                    _startDateICP,
                    _endDateICP,
                    _startDateUF,
                    _endDateUF,
                    _rate.getValue(),
                    interest,
                    _spread,
                    _gearing,
                    settlementAmount());

			return std::make_shared<IcpClfCashflowWrapper>(tup);

		}

        void IcpClfCashflow::setStartDateICPDerivatives(std::vector<double> der)
        {
		    size_t size = der.size();
		    _startDateICPDerivatives.resize(size);
		    for (size_t i = 0; i < size; ++i)
            {
                _startDateICPDerivatives.at(i) = der.at(i);
            }
        }


        void IcpClfCashflow::setEndDateICPDerivatives(std::vector<double> der)
        {
            size_t size = der.size();
            _endDateICPDerivatives.resize(size);
            for (size_t i = 0; i < size; ++i)
            {
                _endDateICPDerivatives.at(i) = der.at(i);
            }
        }


        void IcpClfCashflow::setStartDateUFCLPDerivatives(std::vector<double> der)
        {
            size_t size = der.size();
            _startDateUFCLPDerivatives.resize(size);
            for (size_t i = 0; i < size; ++i)
            {
                _startDateUFCLPDerivatives.at(i) = der.at(i);
            }
        }


        void IcpClfCashflow::setEndDateUFCLPDerivatives(std::vector<double> der)
        {
            size_t size = der.size();
            _endDateUFCLPDerivatives.resize(size);
            for (size_t i = 0; i < size; ++i)
            {
                _endDateUFCLPDerivatives.at(i) = der.at(i);
            }
        }


        void IcpClfCashflow::setStartDateUFCLFDerivatives(std::vector<double> der)
        {
            size_t size = der.size();
            _startDateUFCLFDerivatives.resize(size);
            for (size_t i = 0; i < size; ++i)
            {
                _startDateUFCLFDerivatives.at(i) = der.at(i);
            }
        }


        void IcpClfCashflow::setEndDateUFCLFDerivatives(std::vector<double> der)
        {
            size_t size = der.size();
            _endDateUFCLFDerivatives.resize(size);
            for (size_t i = 0; i < size; ++i)
            {
                _endDateUFCLFDerivatives.at(i) = der.at(i);
            }
        }


        IcpClfCashflow::~IcpClfCashflow()
        {
        }


        std::vector<double> IcpClfCashflow::getAmountICPDerivatives() const
        {
            return _amountICPDerivatives;
        }


        std::vector<double> IcpClfCashflow::getAmountUFCLPDerivatives() const
        {
            return _amountUFCLPDerivatives;
        }


        std::vector<double> IcpClfCashflow::getAmountUFCLFDerivatives() const
        {
            return _amountUFCLFDerivatives;
        }

        std::string IcpClfCashflow::getType() const {
            return "IcpClfCashflow";
        }

        double IcpClfCashflow::settlementAmount() {
            auto ufAmount = _calculateInterest(_endDate, _endDateICP, _endDateUF);
            if (_doesAmortize) {
                ufAmount += _amortization;
            }
            auto clp = settlementCurrency();
            return clp->amount(ufAmount * _endDateUF);
        }

    }
}
