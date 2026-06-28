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
