#ifndef INTERESTRATECURVE_H
#define INTERESTRATECURVE_H

#include "QCInterestRate.h"
#include "curves/QCInterpolator.h"

namespace QCode
{
	namespace Financial
	{
		/**
		 * @class	InterestRateCurve
		 *
		 * @brief	Abstract base class for all interest rate curves (discount, projection). 
		 *
		 * @author	Alvaro D�az V.
		 * @date	07/07/2018
		 */
		class InterestRateCurve
		{
		public:
			/**
			 * @fn	InterestRateCurve::InterestRateCurve(shared_ptr<QCInterpolator> curve, 
			 * 		QCInterestRate intRate)
			 *
			 * @brief	Constructor
			 *
			 * @author	Alvaro Díaz V.
			 * @date	07/07/2018
			 *
			 * @param	curve  	The values of the curve (x's and y's) with an associated
			 * 					method of interpolation.
			 * @param	intRate	A QCInterestRate object which establishes the convention
			 * 					of all rates in the curve.
			 */
			InterestRateCurve(shared_ptr<QCInterpolator> curve, QCInterestRate intRate) : _curve(curve), _intRate(intRate)
			{
				_dfDerivatives.resize(_curve->getLength());
				_fwdWfDerivatives.resize(_curve->getLength());
			}


			/**
			 * @fn	virtual double InterestRateCurve::getRateAt(long d) = 0;
			 *
			 * @brief	Gets rate at maturity d. This method uses the interpolation defined
			 * 			in the parameter curve used when constructing an instance of this class.
			 *
			 * @author	Alvaro Díaz V.
			 * @date	07/07/2018
			 *
			 * @param	d	A long representing the maturity of the rate.
			 *
			 * @return	The rate at maturity d.
			 */
			virtual double getRateAt(long d) = 0;

			/*!
			* Retorna la tasa interpolada al plazo d.
			* @param d plazo a interpolar
			* @return objeto de tipo QCInterestRate que representa
			* 		  la tasa interpolada con la convenci�n de las tasas de la curva.
			*/
			virtual QCInterestRate getQCInterestRateAt(long d) = 0;


			/*!
			* Retorna el factor de descuento interpolado al plazo d.
			* @param d plazo a interpolar
			* @return valor del factor de descuento interpolado.
			*/
			virtual double getDiscountFactorAt(long d) = 0;

			/*!
			* Retorna la tasa forward entre los plazos d1 y d2 en la convención de intRate.
			* @param intRate convención de la tasa forward que se debe calcular.
			* @param d1 plazo más corto de la tasa forward.
			* @param d2 plazo más largo de la tasa forward.
			* @return valor de la tasa forward calculada.
			* Probablemente este método puede mejorarse haciendo que retorne void y el valor de la
			* tasa forward calculada se almacene dentro de la variable intRate.
			*/
			virtual double getForwardRateWithRate(QCInterestRate& intRate, long d1, long d2) = 0;

			/*!
			* Retorna la tasa forward entre los plazos d1 y d2 en la convenci�n de las tasas de la curva.
			* @param d1 plazo m�s corto de la tasa forward.
			* @param d2 plazo m�s largo de la tasa forward.
			* @return valor de la tasa forward calculada.
			*/
			virtual double getForwardRate(long d1, long d2) = 0;

			/*!
			* Retorna la factor de capitalización forward entre los plazos d1 y d2.
			* @param d1 plazo más corto del factor forward.
			* @param d2 plazo más largo del factor forward.
			* @return valor del factor forward calculado.
			*/
			virtual double getForwardWf(long d1, long d2) = 0;

			/*!
			* Este método es un getter que retorna la derivada del último factor de descuento calculado.
			* @return valor de la derivada.
			*/
			virtual double dfDerivativeAt(unsigned int index) = 0;

            /*!
            * Este método es un getter que retorna la derivada del último factor de descuento calculado.
            * @return valor de la derivada.
            */
            virtual double wfDerivativeAt(unsigned int index) = 0;

            /*!
			* Este m�todo es un getter que retorna la derivada del �ltimo factor de capitalizaci�n
			* forward calculado.
			* @return valor de la derivada.
			*/
			virtual double fwdWfDerivativeAt(unsigned int index) = 0;

			/*!
			* Este método es un getter que retorna el largo de la curva
			* @return largo de la curva.
			*/
			size_t getLength() const
			{
				return _curve->getLength();
			}

			/*!
			* Retorna el valor de la tasa en la posición index
			* @param índice de la tasa buscada
			* @return valor de la tasa
			*/
			double getRateAtIndex(size_t index)
			{
				return _curve->getValuesAt(index).second;
			}

			/*!
			* Este método borra abscisas y ordenadas de la curva y vuelve a definir el tamaño de los
			* vectores de abscisa y ordenadas.
			* @param newSize nuevo tamaño de la curva
			*/
			void reset(unsigned long newSize)
			{
				_curve->reset(newSize);
				_dfDerivatives.resize(newSize);
				_fwdWfDerivatives.resize(newSize);
			}

			/*!
			* Este método actualiza el valor de un par ordenado de la curva. Si la abscisa corresponde
			* a un par ya existente, entonces sobre escribe los valores existentes. Si la abscisa no existe,
			* se inserta un nuevo par en la posición que corresponda (ordenado por abscisa ascendente).
			* @param x abscisa
			* @param y ordenada
			*/
			void setPair(long x, double y)
			{
				_curve->setPair(x, y);
			}

			/*!
			* Método que actualiza el valor de la ordenada en una posición dada.
			* @param pos
			* @param value
			*/
			void setOrdinateAtWithValue(unsigned long position, double value)
			{
				_curve->setOrdinateAtWithValue(position, value);
			}

			/*!
			* M�todo que actualiza el valor de la abscisa en una posici�n dada.
			* @param pos
			* @param value
			*/
			void setAbscissaAtWithValue(unsigned long position, long value)
			{
				_curve->setAbscissaAtWithValue(position, value);
			}

			/*!
			* Devuelve el par de la curva en una posici�n.
			* @param position posici�n buscada.
			* @return par en la posici�n.
			*/
			pair<long, double> getValuesAt(unsigned long position)
			{
				return _curve->getValuesAt(position);
			}

			/*!
			* Destructor de la clase.
			*/
			virtual ~InterestRateCurve(){}


		protected:
			shared_ptr<QCInterpolator> _curve;	/*!< Plazos y valores de la curva.*/

			QCInterestRate _intRate;			/*!> Tipo de tasa de inter�s de la curva*/

			vector<double> _dfDerivatives;		/*!< Derivadas del factor de descuento interpolado
												respecto a las tasas de la curva.*/

			vector<double> _fwdWfDerivatives;	/*!< Derivadas del factor de capitalizacion forward
												respecto a las tasas de la curva.*/
		};
	}
}

#endif //QCINTERESTRATECURVE_H

