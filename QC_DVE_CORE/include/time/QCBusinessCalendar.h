//
// Created by Alvaro Diaz on 15-07-16.
//

#ifndef QCBUSINESSCALENDAR_H
#define QCBUSINESSCALENDAR_H

#include <vector>
#include <memory>
#include <set>

#include "time/QCDate.h"
#include "TypeAliases.h"

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
        QCBusinessCalendar(const QCDate& startDate, int length);

        /*!
         * Agrega una fecha (QCDate) al calendario (que representa un feriado)
         * @param holiday (QCDate) el feriado a agregar
         */
        void addHoliday(const QCDate& holiday);

        /*!
         * Calcula el día hábil siguiente a la fecha que ingresa.
         * Si la fecha que ingresa es hábil se mantiene igual.
         * @param fecha (QCDate) fecha que ingresa
         * @return (QCDate) el día hábil siguiente
         */
        QCDate nextBusinessDay(const QCDate& fecha);

        /*!
         * Calcula el día hábil siguiente a la fecha ingresada en convención
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

		/**
		 * @fn	DateList QCBusinessCalendar::getHolidays();
		 *
		 * @brief	Gets all the holidays in the calendar, not including weekends.
		 *
		 * @author	Alvaro Díaz V.
		 * @date	02/10/2017
		 *
		 * @return	The holidays.
		 */
		std::vector<QCDate> getHolidays();

        QCDate getStartDate() const;

        int getLength() const;

        void setHolidays(std::set<QCDate> holidays);

        std::set<QCDate> getHolidaysAsSet() const;

    private:
        QCDate _startDate;
        int _length;
		std::set<QCDate> _holidays;
        QCDate::QCWeekDay _firstDayOfWeekend;
        QCDate::QCWeekDay _secondDayOfWeekEnd;
        void insertNewYear();
        // void sortHolidays();


};


#endif //QCBUSINESSCALENDAR_H
