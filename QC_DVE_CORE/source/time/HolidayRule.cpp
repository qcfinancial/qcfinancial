//
// HolidayRule.cpp
//

#include "time/HolidayRule.h"

HolidayRule HolidayRule::fixed(int month, int day,
                               std::optional<int> fromYear,
                               std::optional<int> toYear,
                               std::optional<Observance> observance)
{
    HolidayRule r;
    r.kind = RuleKind::fixedDate;
    r.month = month;
    r.day = day;
    r.fromYear = fromYear;
    r.toYear = toYear;
    r.observance = observance;
    return r;
}

HolidayRule HolidayRule::fixedOnWeekday(int month, int day, QCDate::QCWeekDay weekday,
                                        std::optional<int> fromYear,
                                        std::optional<int> toYear)
{
    HolidayRule r;
    r.kind = RuleKind::fixedOnWeekday;
    r.month = month;
    r.day = day;
    r.weekday = weekday;
    r.fromYear = fromYear;
    r.toYear = toYear;
    return r;
}

HolidayRule HolidayRule::nthWeekday(int n, QCDate::QCWeekDay weekday, int month,
                                    std::optional<int> fromYear,
                                    std::optional<int> toYear,
                                    std::optional<Observance> observance)
{
    HolidayRule r;
    r.kind = RuleKind::nthWeekdayOfMonth;
    r.n = n;
    r.weekday = weekday;
    r.month = month;
    r.fromYear = fromYear;
    r.toYear = toYear;
    r.observance = observance;
    return r;
}

HolidayRule HolidayRule::easter(int offsetDays, std::optional<Observance> observance)
{
    HolidayRule r;
    r.kind = RuleKind::easterRelative;
    r.offsetDays = offsetDays;
    r.observance = observance;
    return r;
}

HolidayRule HolidayRule::oneOff(const QCDate& date)
{
    HolidayRule r;
    r.kind = RuleKind::specialOneOff;
    r.special = date;
    r.fromYear = date.year();
    r.toYear = date.year();
    return r;
}

QCDate applyObservance(const QCDate& date, Observance observance)
{
    QCDate::QCWeekDay wd = date.weekDay();
    switch (observance)
    {
        case Observance::none:
            return date;

        case Observance::satToFriSunToMon:
        case Observance::nearest:
            if (wd == QCDate::qcSaturday) return date.addDays(-1);
            if (wd == QCDate::qcSunday) return date.addDays(1);
            return date;

        case Observance::sunToMon:
            if (wd == QCDate::qcSunday) return date.addDays(1);
            return date;

        case Observance::chileMondayShift:
            // Ley 20.215: martes/miércoles/jueves -> lunes de la misma semana;
            // viernes -> lunes de la semana siguiente. Lunes/sábado/domingo sin cambio.
            if (wd == QCDate::qcTuesday)   return date.addDays(-1);
            if (wd == QCDate::qcWednesday) return date.addDays(-2);
            if (wd == QCDate::qcThursday)  return date.addDays(-3);
            if (wd == QCDate::qcFriday)    return date.addDays(3);
            return date;

        case Observance::chileReformationShift:
            // Ley 20.299: martes -> viernes de la semana anterior (-4);
            // miércoles -> viernes inmediatamente siguiente (+2). Resto sin cambio.
            if (wd == QCDate::qcTuesday)   return date.addDays(-4);
            if (wd == QCDate::qcWednesday) return date.addDays(2);
            return date;
    }
    return date;
}

std::optional<QCDate> HolidayRule::resolve(int year, Observance calendarDefault) const
{
    if (fromYear.has_value() && year < fromYear.value()) return std::nullopt;
    if (toYear.has_value() && year > toYear.value()) return std::nullopt;

    QCDate nominal;
    switch (kind)
    {
        case RuleKind::fixedDate:
            nominal = QCDate{day, month, year};
            break;
        case RuleKind::fixedOnWeekday:
        {
            // Emit (month, day) only if it falls on the required weekday; never
            // shifted. Used for Ley 20.983: Sep 17 is a holiday when Sep 18/19 are
            // Sat/Sun (i.e. when Sep 17 is a Friday), and Jan 2 is a holiday when
            // Jan 1 is a Sunday (i.e. when Jan 2 is a Monday).
            QCDate candidate{day, month, year};
            if (candidate.weekDay() != weekday) return std::nullopt;
            return candidate;
        }
        case RuleKind::nthWeekdayOfMonth:
            nominal = QCDate::nthWeekdayOfMonth(n, weekday, month, year);
            break;
        case RuleKind::easterRelative:
            nominal = QCDate::easterSunday(year).addDays(offsetDays);
            break;
        case RuleKind::specialOneOff:
            // One-off dates are emitted only in their own year and never shifted.
            if (year != special.year()) return std::nullopt;
            return special;
    }

    Observance obs = observance.value_or(calendarDefault);
    return applyObservance(nominal, obs);
}
