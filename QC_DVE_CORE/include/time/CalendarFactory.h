//
// CalendarFactory.h
//
// Rule-driven construction of a single merged QCBusinessCalendar from a start
// date, a number of years, and a set of BusinessCalendarId values.
//

#ifndef QC_CALENDARFACTORY_H
#define QC_CALENDARFACTORY_H

#include <vector>

#include "time/QCDate.h"
#include "time/QCBusinessCalendar.h"
#include "time/BusinessCalendarId.h"

/*!
 * @brief Construye calendarios de feriados a partir de reglas declarativas.
 */
class CalendarFactory
{
    public:
        /*!
         * Construye un único QCBusinessCalendar cuya lista de feriados es la unión
         * de los feriados de todos los calendarios solicitados, sobre el horizonte
         * [startDate, startDate + nYears años]. La observancia (traslado de feriados
         * que caen en fin de semana) se aplica por calendario ANTES de la unión.
         *
         * @param startDate (QCDate) fecha inicial del calendario
         * @param nYears (int) largo en años del calendario
         * @param ids (vector<BusinessCalendarId>) calendarios a construir y unir
         * @return (QCBusinessCalendar) calendario combinado
         */
        static QCBusinessCalendar build(
                const QCDate& startDate,
                int nYears,
                const std::vector<BusinessCalendarId>& ids);
};

#endif //QC_CALENDARFACTORY_H
