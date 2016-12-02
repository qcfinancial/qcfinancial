//
// Created by Alvaro Diaz on 14-07-16.
//

#ifndef QCDATE_H
#define QCDATE_H

#include <iostream>
#include <string>

using namespace std;

/*!
 * @brief QCDate es la implementación de QCode de una clase para manejar fechas
 * @author Alvaro Díaz (QCode)
 * @version 1.0
 * @date Julio, 2016
 */
class QCDate
{
    public:
        /*!
         * QCWeekDay es un enum que sirve para identificar los días de la semana.
         */
        enum QCWeekDay
        {
            qcMonday = 1,
            qcTuesday = 2,
            qcWednesday = 3,
            qcThursday = 4,
            qcFriday = 5,
            qcSaturday = 6,
			qcSunday = 7
        };
        /*!
         * Constructor por default
         * @return un objecto QCDate inicializado al 12 de enero de 1969
         */
        QCDate();

        /*!
         * Constructor
         * @param excelSerial (long) representación de Excel de una fecha
         * @return un objecto QCDate con la fecha indicada por excelSerial
         */
        QCDate(long excelSerial);

        /*!
         * Constructor
         * @param d día
         * @param m mes
         * @param y año
         * @return un objeto QCDate con la fecha d/m/y
         */
        QCDate(int d, int m, int y);

        /*!
         * Copy constructor
         * @param otherDate otro objeto de tipo QCDate
         * @return objeto QCDate receptor de la copia
         */
        QCDate(const QCDate& otherDate);

        /*!
         * Operator= sobrecarga
         * @param otherDate
         */
        void operator=(const QCDate& otherDate);

        /*!
         * Operator < sobrecarga
         * @param rhs
         * @return (bool)
         */
        bool operator<(const QCDate& rhs) const;

        /*!
         * Operator > sobrecarga
         * @param rhs
         * @return (bool)
         */
        bool operator>(const QCDate& rhs) const;

        /*!
         * Operator <= sobrecarga
         * @param rhs
         * @return (bool)
         */
        bool operator<=(const QCDate& rhs) const;

        /*!
         * Operator >= sobrecarga
         * @param rhs
         * @return (bool)
         */
        bool operator>=(const QCDate& rhs) const;

        /*!
         * Operator == sobrecarga
         * @param rhs
         * @return (bool)
         */
        bool operator==(const QCDate& rhs) const ;

        /*!
         * Operator != sobrecarga
         * @param rhs
         * @return (bool)
         */
        bool operator!=(const QCDate& rhs) const ;

        /*!
         * Setter: setea la fecha a partir de su representación Excel
         * @param excelSerial
         */
        void setDateFromExcelSerial(long excelSerial);

        /*!
         * Setter: setea el día de la fecha
         * @param d
         */
        void setDay(int d);

        /*!
         * Setter: setea el mes de la fecha
         * @param m
         */
        void setMonth(int m);

        /*!
         * Setter: setea el año de la fecha
         * @param y
         */
        void setYear(int y);

        /*!
         * Setter: setea día, mes y año
         * @param d día
         * @param m mes
         * @param y año
         */
        void setDayMonthYear(int d, int m, int y);

        /*!
         * Getter
         * @return (int) el día de la fecha
         */
        int day() const;

        /*!
         * Getter
         * @return (int) el mes de la fecha
         */
        int month() const;

        /*!
         * Getter
         * @return (int) el año de la fecha
         */
        int year() const;

        /*!
         * @return (int) Retorna la fecha como su representación en Excel
         */
        long excelSerial() const;

        /*!
         * Calcula el número de días reales entre otherDate y sí misma
         * @param otherDate
         * @return (long) número de días calculados
         */
        long dayDiff(const QCDate& otherDate) const;

        /*!
         * Calcula la fecha que resulta de sumar un número de días a si misma
         * @param nDays número de días a sumar
         * @return (QCDate) fecha resultante
         */
        QCDate addDays(long nDays) const;

        /*!
         * Calcula la fecha que resulta de sumar un número de meses a si misma
         * @param nMonths número de meses a sumar
         * @return (QCDate) fecha resultante
         */
        QCDate addMonths(int nMonths) const;

        /*!
         * Retorna el día de la semana
         * @return (QCWeekDay) día de la semana resultante
         */
        QCWeekDay weekDay() const;

        /*!
         * Retorna a si misma como string legible y printer friendly
         * @return (std::string)
         */
        string description();

    private:
        int _day;
        int _month;
        int _year;
        bool _validate(int d, int m, int y);
};

#endif //QCDATE_H
