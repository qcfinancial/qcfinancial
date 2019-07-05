//
// Created by Alvaro Diaz on 14-07-16.
//

#ifndef QCDATE_H
#define QCDATE_H

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <memory>

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
			qcSunday = 0
        };

		/*!
		* QCBusDayAdjRules es un enum que sirve para identificar las formas de realizar un ajuste
		* de fechas.
		*/
		enum QCBusDayAdjRules
		{
			qcNo = 0,
			qcFollow = 1,
			qcModFollow = 2,
			qcPrev = 3,
			qcModPrev = 4
		};

		/*!
		* QCDirection es un enum que sirve para identificar si un desplazamiento de fechas es
		* hacia adelante o hacia atrás.
		*/
		enum QCDirection
		{
			qcBackward = 0,
			qcForward = 1
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
		* Constructor
		* @param stringDate yyyy/mm/dd o yyyy-mm-dd
		* @return un objeto QCDate con la fecha d/m/y
		*/
		QCDate(string& stringDate);

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
		 * Sera > 0 si otherDate es mayor que sí misma.
         * @param otherDate
         * @return (long) número de días calculados
         */
        long dayDiff(const QCDate& otherDate) const;

		/*!
		* Calcula el número de meses enteros entre otherDate y sí misma.
		* Sera > 0 si otherDate es mayor que sí misma. Entrega el resto en dias.
		* @param otherDate
		* @return (tuple<unsigned long, unsigned int>) número de meses y resto en días.
		*/
		tuple<unsigned long, int> monthDiffDayRemainder(const QCDate& otherDate, vector<QCDate>& calendar, QCBusDayAdjRules rule) const;

		/*!
		* Calcula el número de meses enteros entre otherDate y sí misma.
		* Sera > 0 si otherDate es mayor que sí misma. Entrega el resto en dias.
		* @param otherDate
		* @return (tuple<unsigned long, unsigned int>) número de meses y resto en días.
		*/
		tuple<unsigned long, int> monthDiffDayRemainder(const QCDate& otherDate, shared_ptr<vector<QCDate>> calendar, QCDate::QCBusDayAdjRules rule) const;
        /*!
         * Calcula la fecha que resulta de sumar un número de días a si misma
         * @param nDays número de días a sumar
         * @return (QCDate) fecha resultante
         */
        QCDate addDays(long nDays) const;

		/*!
		* Desplaza la fecha lo que sea necesario en la direccion direction para que el día de la nueva
		* fecha coincida con el parámetro dayOfMonth. Si es necesario, se cambia de mes y de año.
		* @param dayOfMonth día del mes que se quiere alcanzar
		* @param direction dirección del movimiento
		* @return (QCDate) fecha resultante
		*/
		QCDate moveToDayOfMonth(unsigned int dayOfMonth, QCDirection direction,
			bool stopAtEndOfMonth = false) const;

		/*!
		* Indica si la fecha corresponde al último día del mes.
		* @return (bool) true si es el último día del mes, false en los otros casos.
		*/
		bool isEndOfMonth() const;

		/*!
		* Calcula la siguiente fecha que es dia habil considerando el vector de
		* fechas que son feriado y la regla (FOLLOW, MODFOLLOW). Si la fecha es habil
		* entonces se retorna a si misma.
		* @param calendar (vector<QCDate>&) vector con los feriados
		* @param rule (string) regla para el calculo (FOLLOW, MOD_FOLLOW, PREV, MOD_PREV)
		* @return (QCDate) fecha resultante
		*/
		QCDate businessDay(vector<QCDate>& calendar, QCBusDayAdjRules rule) const;

		/*!
		* Calcula la siguiente fecha que es dia habil considerando el vector de
		* fechas que son feriado y la regla (FOLLOW, MODFOLLOW). Si la fecha es habil
		* entonces se retorna a si misma.
		* @param calendar (shared_ptr<vector<QCDate>>) vector con los feriados
		* @param rule (string) regla para el calculo (FOLLOW, MOD_FOLLOW, PREV, MOD_PREV)
		* @return (QCDate) fecha resultante
		*/
		QCDate businessDay(shared_ptr<vector<QCDate>> calendar, QCDate::QCBusDayAdjRules rule) const;

		/*!
		* Suma nDays dias habiles a la fecha considerando el calendario entregado.
		* @param calendar (vector<QCDate>&) vector con los feriados
		* @param nDays (unsigned int) numero de dias habiles
		* @param direction (QCDate::QCBusDayAdjRules) indica si hay que avanzar o retroceder
		* @return (QCDate) fecha resultante
		*/
		QCDate shift(vector<QCDate>& calendar, unsigned int nDays,
			QCDate::QCBusDayAdjRules direction) const;

		/*!
		* Suma nDays dias habiles a la fecha considerando el calendario entregado.
		* @param calendar (shared_ptr<vector<QCDate>>) vector con los feriados
		* @param nDays (unsigned int) numero de dias habiles
		* @param direction (QCDate::QCBusDayAdjRules) indica si hay que avanzar o retroceder
		* @return (QCDate) fecha resultante
		*/
		QCDate shift(shared_ptr<vector<QCDate>> calendar, unsigned int nDays,
			QCDate::QCBusDayAdjRules direction) const;

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
		* Calcula la fecha que resulta de sumar un número de semanas a si misma
		* @param calendar (shared_ptr<vector<QCDate>>) vector con los feriados
		* @param nWeeks número de semanas a sumar. Una semana son 7 días más
		* 				el ajuste que se produzca por el ajuste por día hábil.
		* @param direction (QCDate::QCBusDayAdjRules) indica si hay que avanzar o retroceder
		* @return (QCDate) fecha resultante
		*/
		QCDate addWeeks(vector<QCDate>& calendar, unsigned int nWeeks,
			QCDate::QCBusDayAdjRules direction) const;

        /*!
         * Retorna a si misma como string legible y printer friendly
		 * @param dmy si es true retorna formato 'dd-mm-yyyy' si es false el formato
		 * es 'yyyy-mm-dd'.
		 * @return (std::string)
         */
        string description(bool dmy = true) const;

    private:
        int _day;
        int _month;
        int _year;
        bool _validate(int d, int m, int y);
};

std::ostream &operator<<(std::ostream &ostr, const QCDate& date);

#endif //QCDATE_H
