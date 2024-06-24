#ifndef ADSCATTER_H
#define ADSCATTER_H

#include <vector>
#include <utility>
#include <exception>
#include <algorithm>

#include <../autodiff/autodiff/forward/real/eigen.hpp>

#include "time/QCDate.h"

using namespace std;
namespace ad = autodiff;

namespace QCode::ADFinancial {
    class ADScatter
    {
    public:
        ADScatter() = default;


        ADScatter(std::vector<double>& abscissa, std::vector<ad::real>& ordinate) {
            // Se verifica si el numero de abscissa y ordinate coinciden
            if (abscissa.size() != ordinate.size())
            {
                string msg = "Error constructing curve: ";
                msg += "number of abscissa is different from number of ordinate.";
                throw invalid_argument(msg);
            }

            // Se verifica que no se repitan valores de abscisa
            for (unsigned long i = 0; i < abscissa.size(); ++i)
            {
                for (unsigned long j = i + 1; j < abscissa.size(); ++j)
                {
                    if (abscissa[i] == abscissa[j])
                    {
                        string msg = "Error constructing curve: ";
                        msg += "Some values of abscissa coincide. ";
                        msg += "Values " + to_string(abscissa[i]) + " and " + to_string(abscissa[j]);
                        msg += " at positions " + to_string(i) + " and " + to_string(j);
                        throw invalid_argument(msg);
                    }
                }
            }

            //Se inicializa el vector _values
            if (!abscissa.empty())
            {
                _values.resize(abscissa.size());
                for (size_t i = 0; i < abscissa.size(); ++i)
                {
                    _values.at(i) = make_pair(abscissa[i], ordinate[i]);
                }
            }
            else
            {
                throw runtime_error("El vector de abscissa no contiene datos.");
            }

            //Ordenar las abscissa respetando el mapeo con la ordenada
            sort(_values.begin(), _values.end());

        };


        void reset(size_t newSize) {
            _values.clear();
            _values.resize(newSize);
        };


        void setPair(double x, ad::real y) {
            for (auto& elmnt : _values)
            {
                if (elmnt.first == x)
                {
                    elmnt.second = y;
                    return;
                }
            }
            _values.push_back(make_pair(x, y));
            sort(_values.begin(), _values.end());

        };


        void setOrdinateAtWithValue(size_t position, ad::real value) {
            _values.at(position).second = value;
        };


        void setAbscissaAtWithValue(size_t position, double value) {
            _values.at(position).first = value;
        };


        pair<double, ad::real> getValuesAt(size_t position) {
            return _values[position];
        };


        size_t getLength() {
            return _values.size();
        };


        ~ADScatter() = default;


    protected:
        vector<pair<double, ad::real>> _values;	//En esta variable se guardan los datos de la curva
    };
}

#endif //ADSCATTER_H

