//
// Created by Alvaro Diaz on 15-07-16.
//

#ifndef QCBUSINESSCALENDAR_H
#define QCBUSINESSCALENDAR_H

#include <vector>
#include "QCDate.h"

using namespace std;

/*!
 * @brief La clase QCBusinessCalendar representa días feriados de una jurisdicción.
 */
class QCBusinessCalendar
{
    public:
        /*!
         * Constructor
         * @param startDate (QCDate) fecha inicial del calendario
         * @param length (int) largo en años del calendario
         * @return objeto construido
         */
        QCBusinessCalendar(QCDate& startDate, int length);

        /*!
         * Agrega una fecha (QCDate) al calendario (que representa un feriado)
         * @param holyday (QCDate) el feriado a agregar
         */
        void addHolyday(const QCDate& holyday);

        /*!
         * Calcula el día hábil siguiente a la fecha que ingresa.
         * Si la fecha que ingresa es hábil se mantiene igual.
         * @param fecha (QCDate) fecha que ingresa
         * @return (QCDate) el día hábil siguiente
         */
        QCDate nextBusinessDay(const QCDate& fecha);

        /*!
         * Calcula el día hábil siguiente a la fecha ingresada en comvención
         * modificada. Si el día hábil siguiente cambia de mes respecto a la
         * fecha ingresada, entonces retorna el día hábil anterior.
         * Si la fecha ingresada es hábil se retorna la misma fecha.
         * @param fecha (QCDate) fecha ingresada
         * @return el día hábil siguiente
         */
        QCDate modNextBusinessDay(const QCDate& fecha);

        /*!
         * Calcula el día hábil anterior a la fecha ingresada
         * @param fecha (QCDate)
         * @return
         */
        QCDate previousBusinessDay(const QCDate& fecha);

        /*!
         * Mueve una fecha un número nDays de días.
         * @param fecha (QCDate) fecha que se debe mover
         * @param nDays (int) número de días a mover (pos o neg)
         * @return (QCDate) fecha movida
         */
        QCDate shift(const QCDate& fecha, int nDays);

    private:
        QCDate _startDate;
        int _length;
        vector<QCDate> _holydays;
        QCDate::QCWeekDay _firstDayOfWeekend;
        QCDate::QCWeekDay _secondDayOfweekEnd;
        void insertNewYear();
        void sortHolydays();


};


#endif //QCBUSINESSCALENDAR_H
