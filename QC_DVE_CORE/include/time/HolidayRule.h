//
// HolidayRule.h
//
// A small declarative holiday-rule taxonomy. Rules are resolved eagerly, one
// concrete date per year (python-holidays style), with optional validity windows
// and observance policies. Used internally by CalendarFactory.
//

#ifndef QC_HOLIDAYRULE_H
#define QC_HOLIDAYRULE_H

#include <optional>

#include "time/QCDate.h"

/*!
 * @brief Política de observancia: cómo se traslada un feriado que cae en fin de semana.
 */
enum class Observance
{
    none,             //!< no se traslada
    satToFriSunToMon, //!< sábado -> viernes, domingo -> lunes (US)
    sunToMon,         //!< solo domingo -> lunes
    nearest,          //!< día hábil más cercano (sábado -> viernes, domingo -> lunes)

    // Traslados de feriados chilenos (leyes 20.215 y 20.299).
    chileMondayShift,     //!< Ley 20.215 (San Pedro y San Pablo, Encuentro de Dos Mundos):
                          //!< martes/miércoles/jueves -> lunes de la misma semana;
                          //!< viernes -> lunes de la semana siguiente.
    chileReformationShift //!< Ley 20.299 (Día de las Iglesias Evangélicas, 31-oct):
                          //!< martes -> viernes de la semana anterior; miércoles -> viernes siguiente.
};

/*!
 * @brief Tipo de regla de feriado.
 */
enum class RuleKind
{
    fixedDate,         //!< fecha fija (mes, día)
    fixedOnWeekday,    //!< fecha fija (mes, día) que solo aplica si cae en un día de la semana dado
    nthWeekdayOfMonth, //!< n-ésimo día de la semana del mes (n < 0 cuenta desde el final)
    easterRelative,    //!< desplazamiento en días respecto al domingo de Pascua
    specialOneOff      //!< fecha única (no se repite cada año)
};

/*!
 * @brief Una regla declarativa de feriado.
 *
 * Solo los campos correspondientes a `kind` son significativos. Cada regla
 * acepta una ventana de validez opcional (fromYear/toYear) y una política de
 * observancia opcional que sobreescribe la política por defecto del calendario.
 */
struct HolidayRule
{
    RuleKind kind;

    // fixedDate / nthWeekdayOfMonth
    int month = 0;
    int day = 0;

    // nthWeekdayOfMonth
    int n = 0;
    QCDate::QCWeekDay weekday = QCDate::qcMonday;

    // easterRelative
    int offsetDays = 0;

    // specialOneOff
    QCDate special{};

    // cross-cutting
    std::optional<int> fromYear;
    std::optional<int> toYear;
    std::optional<Observance> observance; //!< sobreescribe el default del calendario

    // --- factory helpers ---

    static HolidayRule fixed(int month, int day,
                             std::optional<int> fromYear = std::nullopt,
                             std::optional<int> toYear = std::nullopt,
                             std::optional<Observance> observance = std::nullopt);

    //! Emite (month, day) solo si esa fecha cae en `weekday`; no aplica observancia.
    static HolidayRule fixedOnWeekday(int month, int day, QCDate::QCWeekDay weekday,
                                      std::optional<int> fromYear = std::nullopt,
                                      std::optional<int> toYear = std::nullopt);

    static HolidayRule nthWeekday(int n, QCDate::QCWeekDay weekday, int month,
                                  std::optional<int> fromYear = std::nullopt,
                                  std::optional<int> toYear = std::nullopt,
                                  std::optional<Observance> observance = std::nullopt);

    static HolidayRule easter(int offsetDays,
                              std::optional<Observance> observance = std::nullopt);

    static HolidayRule oneOff(const QCDate& date);

    /*!
     * Resuelve la regla a una fecha (ya ajustada por observancia) para el año dado.
     * Retorna std::nullopt si el año está fuera de la ventana de validez.
     * @param year (int) año a resolver
     * @param calendarDefault (Observance) política por defecto del calendario
     */
    [[nodiscard]] std::optional<QCDate> resolve(int year, Observance calendarDefault) const;
};

/*!
 * Aplica una política de observancia a una fecha.
 */
QCDate applyObservance(const QCDate& date, Observance observance);

#endif //QC_HOLIDAYRULE_H
