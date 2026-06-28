//
// CalendarFactory.cpp
//

#include "time/CalendarFactory.h"
#include "time/HolidayRule.h"

namespace {

    // Default observance per calendar (how a holiday on a weekend is shifted).
    Observance defaultObservance(BusinessCalendarId id)
    {
        switch (id)
        {
            case BusinessCalendarId::USNY:
            case BusinessCalendarId::USGS:
                return Observance::satToFriSunToMon;
            case BusinessCalendarId::CLSA:
            case BusinessCalendarId::EUTA:
            default:
                return Observance::none;
        }
    }

    // Shared US-base federal holidays. USNY uses these as-is; USGS extends them.
    std::vector<HolidayRule> usBaseRules()
    {
        return {
            HolidayRule::fixed(1, 1),                                    // New Year's Day
            HolidayRule::nthWeekday(3, QCDate::qcMonday, 1),             // MLK Jr. Day
            HolidayRule::nthWeekday(3, QCDate::qcMonday, 2),             // Washington's Birthday
            HolidayRule::nthWeekday(-1, QCDate::qcMonday, 5),            // Memorial Day
            HolidayRule::fixed(6, 19, 2021),                            // Juneteenth (from 2021)
            HolidayRule::fixed(7, 4),                                    // Independence Day
            HolidayRule::nthWeekday(1, QCDate::qcMonday, 9),             // Labor Day
            HolidayRule::nthWeekday(4, QCDate::qcThursday, 11),          // Thanksgiving
            HolidayRule::fixed(12, 25),                                  // Christmas Day
        };
    }

    std::vector<HolidayRule> usGovSecuritiesRules()
    {
        auto rules = usBaseRules();
        rules.push_back(HolidayRule::nthWeekday(2, QCDate::qcMonday, 10)); // Columbus Day
        rules.push_back(HolidayRule::fixed(11, 11));                       // Veterans Day
        return rules;
    }

    // Santiago (Chile) bank holidays. NOTE: Chile's movable-holiday law
    // (Ley 19.973 / 20.215, which slides some holidays to Monday) is NOT modeled
    // in v1; these are the nominal fixed and Easter-relative dates with no
    // weekend observance shift.
    std::vector<HolidayRule> santiagoRules()
    {
        return {
            HolidayRule::fixed(1, 1),    // Año Nuevo
            HolidayRule::easter(-2),     // Viernes Santo (Good Friday)
            HolidayRule::easter(-1),     // Sábado Santo (Holy Saturday)
            HolidayRule::fixed(5, 1),    // Día del Trabajo
            HolidayRule::fixed(5, 21),   // Día de las Glorias Navales
            HolidayRule::fixed(6, 29),   // San Pedro y San Pablo
            HolidayRule::fixed(7, 16),   // Virgen del Carmen
            HolidayRule::fixed(8, 15),   // Asunción de la Virgen
            HolidayRule::fixed(9, 18),   // Independencia Nacional
            HolidayRule::fixed(9, 19),   // Día de las Glorias del Ejército
            HolidayRule::fixed(10, 12),  // Encuentro de Dos Mundos
            HolidayRule::fixed(10, 31),  // Día de las Iglesias Evangélicas
            HolidayRule::fixed(11, 1),   // Día de Todos los Santos
            HolidayRule::fixed(12, 8),   // Inmaculada Concepción
            HolidayRule::fixed(12, 25),  // Navidad
        };
    }

    // Eurozone TARGET calendar. TARGET does not apply weekend observance shifts.
    std::vector<HolidayRule> targetRules()
    {
        return {
            HolidayRule::fixed(1, 1),    // New Year's Day
            HolidayRule::easter(-2),     // Good Friday
            HolidayRule::easter(1),      // Easter Monday
            HolidayRule::fixed(5, 1),    // Labour Day
            HolidayRule::fixed(12, 25),  // Christmas Day
            HolidayRule::fixed(12, 26),  // Christmas Holiday (Boxing Day)
        };
    }

    std::vector<HolidayRule> ruleSet(BusinessCalendarId id)
    {
        switch (id)
        {
            case BusinessCalendarId::CLSA: return santiagoRules();
            case BusinessCalendarId::USNY: return usBaseRules();
            case BusinessCalendarId::USGS: return usGovSecuritiesRules();
            case BusinessCalendarId::EUTA: return targetRules();
            default: return {};
        }
    }

} // anonymous namespace

QCBusinessCalendar CalendarFactory::build(
        const QCDate& startDate,
        int nYears,
        const std::vector<BusinessCalendarId>& ids)
{
    QCBusinessCalendar result{startDate, nYears};

    int startYear = startDate.year();
    int endYear = startYear + nYears;

    for (auto id : ids)
    {
        Observance calDefault = defaultObservance(id);
        std::vector<HolidayRule> rules = ruleSet(id);
        for (int year = startYear; year <= endYear; ++year)
        {
            for (const auto& rule : rules)
            {
                // Observance is resolved per-calendar here, BEFORE the union: the
                // holiday set is a std::set, so duplicates across calendars collapse.
                auto holiday = rule.resolve(year, calDefault);
                if (holiday.has_value())
                {
                    result.addHoliday(holiday.value());
                }
            }
        }
    }

    return result;
}
