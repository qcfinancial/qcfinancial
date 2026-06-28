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
            case BusinessCalendarId::CLBA:
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

    // Solstice-based Día Nacional de los Pueblos Indígenas (Ley 21.357, since 2021)
    // and ad-hoc one-off national holidays (Fiestas Patrias bridge days, plebiscites,
    // New-Year bridges, the 2017 census day, etc.). These are not expressible as
    // simple recurring rules: the indigenous holiday tracks the June solstice and the
    // others are declared year-by-year by special laws. The dates below are sourced
    // from the Python `holidays` library (v0.99) and cover 2010-2050. EXTEND THIS
    // TABLE for horizons beyond 2050.
    std::vector<HolidayRule> chileOneOffHolidays()
    {
        return {
            // Día Nacional de los Pueblos Indígenas (June solstice, Ley 21.357)
            HolidayRule::oneOff(QCDate(21, 6, 2021)),
            HolidayRule::oneOff(QCDate(21, 6, 2022)),
            HolidayRule::oneOff(QCDate(21, 6, 2023)),
            HolidayRule::oneOff(QCDate(20, 6, 2024)),
            HolidayRule::oneOff(QCDate(20, 6, 2025)),
            HolidayRule::oneOff(QCDate(21, 6, 2026)),
            HolidayRule::oneOff(QCDate(21, 6, 2027)),
            HolidayRule::oneOff(QCDate(20, 6, 2028)),
            HolidayRule::oneOff(QCDate(20, 6, 2029)),
            HolidayRule::oneOff(QCDate(21, 6, 2030)),
            HolidayRule::oneOff(QCDate(21, 6, 2031)),
            HolidayRule::oneOff(QCDate(20, 6, 2032)),
            HolidayRule::oneOff(QCDate(20, 6, 2033)),
            HolidayRule::oneOff(QCDate(21, 6, 2034)),
            HolidayRule::oneOff(QCDate(21, 6, 2035)),
            HolidayRule::oneOff(QCDate(20, 6, 2036)),
            HolidayRule::oneOff(QCDate(20, 6, 2037)),
            HolidayRule::oneOff(QCDate(21, 6, 2038)),
            HolidayRule::oneOff(QCDate(21, 6, 2039)),
            HolidayRule::oneOff(QCDate(20, 6, 2040)),
            HolidayRule::oneOff(QCDate(20, 6, 2041)),
            HolidayRule::oneOff(QCDate(21, 6, 2042)),
            HolidayRule::oneOff(QCDate(21, 6, 2043)),
            HolidayRule::oneOff(QCDate(20, 6, 2044)),
            HolidayRule::oneOff(QCDate(20, 6, 2045)),
            HolidayRule::oneOff(QCDate(21, 6, 2046)),
            HolidayRule::oneOff(QCDate(21, 6, 2047)),
            HolidayRule::oneOff(QCDate(20, 6, 2048)),
            HolidayRule::oneOff(QCDate(20, 6, 2049)),
            HolidayRule::oneOff(QCDate(20, 6, 2050)),

            // Ad-hoc one-off national holidays (special laws)
            HolidayRule::oneOff(QCDate(17, 9, 2010)),  // Feriado nacional (Bicentenario)
            HolidayRule::oneOff(QCDate(20, 9, 2010)),  // Feriado nacional (Bicentenario)
            HolidayRule::oneOff(QCDate(17, 9, 2012)),  // Fiestas Patrias bridge
            HolidayRule::oneOff(QCDate(20, 9, 2013)),  // Fiestas Patrias bridge
            HolidayRule::oneOff(QCDate(2, 1, 2017)),   // New-Year bridge
            HolidayRule::oneOff(QCDate(19, 4, 2017)),  // Censo Nacional
            HolidayRule::oneOff(QCDate(17, 9, 2018)),  // Fiestas Patrias bridge
            HolidayRule::oneOff(QCDate(20, 9, 2019)),  // Fiestas Patrias bridge
            HolidayRule::oneOff(QCDate(17, 9, 2021)),  // Fiestas Patrias bridge
            HolidayRule::oneOff(QCDate(16, 9, 2022)),  // Feriado nacional (plebiscito)
            HolidayRule::oneOff(QCDate(2, 1, 2023)),   // New-Year bridge
            HolidayRule::oneOff(QCDate(20, 9, 2024)),  // Fiestas Patrias bridge
            HolidayRule::oneOff(QCDate(17, 9, 2027)),  // Fiestas Patrias bridge
            HolidayRule::oneOff(QCDate(17, 9, 2029)),  // Fiestas Patrias bridge
            HolidayRule::oneOff(QCDate(20, 9, 2030)),  // Fiestas Patrias bridge
            HolidayRule::oneOff(QCDate(17, 9, 2032)),  // Fiestas Patrias bridge
            HolidayRule::oneOff(QCDate(2, 1, 2034)),   // New-Year bridge
            HolidayRule::oneOff(QCDate(17, 9, 2035)),  // Fiestas Patrias bridge
            HolidayRule::oneOff(QCDate(17, 9, 2038)),  // Fiestas Patrias bridge
            HolidayRule::oneOff(QCDate(2, 1, 2040)),   // New-Year bridge
            HolidayRule::oneOff(QCDate(17, 9, 2040)),  // Fiestas Patrias bridge
            HolidayRule::oneOff(QCDate(20, 9, 2041)),  // Fiestas Patrias bridge
            HolidayRule::oneOff(QCDate(2, 1, 2045)),   // New-Year bridge
            HolidayRule::oneOff(QCDate(17, 9, 2046)),  // Fiestas Patrias bridge
            HolidayRule::oneOff(QCDate(20, 9, 2047)),  // Fiestas Patrias bridge
            HolidayRule::oneOff(QCDate(17, 9, 2049)),  // Fiestas Patrias bridge
        };
    }

    // Santiago (Chile) bank holidays. The movable-holiday laws are modeled via
    // observance policies on the affected fixed dates:
    //   - Ley 20.215: San Pedro y San Pablo (Jun 29) and Encuentro de Dos Mundos
    //     (Oct 12) slide to Monday (chileMondayShift).
    //   - Ley 20.299: Día de las Iglesias Evangélicas (Oct 31) slides to a Friday
    //     (chileReformationShift).
    // These laws took effect in 2008; for years before that the dates were fixed.
    // The shift is applied for all years here (pre-2008 use is not expected for a
    // forward-looking valuation calendar).
    std::vector<HolidayRule> santiagoRules()
    {
        using O = Observance;
        std::vector<HolidayRule> rules = {
            HolidayRule::fixed(1, 1),    // Año Nuevo
            HolidayRule::easter(-2),     // Viernes Santo (Good Friday)
            HolidayRule::easter(-1),     // Sábado Santo (Holy Saturday)
            HolidayRule::fixed(5, 1),    // Día del Trabajo
            HolidayRule::fixed(5, 21),   // Día de las Glorias Navales
            HolidayRule::fixed(6, 29, std::nullopt, std::nullopt, O::chileMondayShift),     // San Pedro y San Pablo
            HolidayRule::fixed(7, 16),   // Virgen del Carmen
            HolidayRule::fixed(8, 15),   // Asunción de la Virgen
            HolidayRule::fixed(9, 18),   // Independencia Nacional
            HolidayRule::fixed(9, 19),   // Día de las Glorias del Ejército
            HolidayRule::fixed(10, 12, std::nullopt, std::nullopt, O::chileMondayShift),    // Encuentro de Dos Mundos
            HolidayRule::fixed(10, 31, std::nullopt, std::nullopt, O::chileReformationShift), // Iglesias Evangélicas
            HolidayRule::fixed(11, 1),   // Día de Todos los Santos
            HolidayRule::fixed(12, 8),   // Inmaculada Concepción
            HolidayRule::fixed(12, 25),  // Navidad
        };
        auto oneOffs = chileOneOffHolidays();
        rules.insert(rules.end(), oneOffs.begin(), oneOffs.end());
        return rules;
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

    // Chile banking calendar: the Santiago (public) holidays plus the bank
    // holiday on December 31 (Feriado bancario). Dec 31 is a fixed, non-movable
    // date.
    std::vector<HolidayRule> chileBankingRules()
    {
        auto rules = santiagoRules();
        rules.push_back(HolidayRule::fixed(12, 31)); // Feriado bancario
        return rules;
    }

    std::vector<HolidayRule> ruleSet(BusinessCalendarId id)
    {
        switch (id)
        {
            case BusinessCalendarId::CLSA: return santiagoRules();
            case BusinessCalendarId::CLBA: return chileBankingRules();
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
