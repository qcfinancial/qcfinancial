//
// Tests for the QCDate calendar primitives, HolidayRule, BusinessCalendarId and
// CalendarFactory.
//
// NOTE: the C++ test executable target is currently commented out in
// Tests/CMakeLists.txt (see CLAUDE.md). These cases document intent and become
// live once the target is re-enabled. Primary verification is the Python smoke
// test driven from setup.py builds.
//

#include "catch/catch-2.hpp"

#include "time/QCDate.h"
#include "time/BusinessCalendarId.h"
#include "time/HolidayRule.h"
#include "time/CalendarFactory.h"

#include <algorithm>
#include <set>
#include <vector>

namespace {
    bool contains(const std::vector<QCDate>& holidays, const QCDate& d) {
        return std::find(holidays.begin(), holidays.end(), d) != holidays.end();
    }
}

// ---------------------------------------------------------------------------
// QCDate primitives
// ---------------------------------------------------------------------------

TEST_CASE("QCDate::easterSunday computes Gregorian computus") {
    REQUIRE(QCDate::easterSunday(2025) == QCDate(20, 4, 2025));
    REQUIRE(QCDate::easterSunday(2024) == QCDate(31, 3, 2024));
    REQUIRE(QCDate::easterSunday(2000) == QCDate(23, 4, 2000)); // century boundary
    REQUIRE(QCDate::easterSunday(2021) == QCDate(4, 4, 2021));
}

TEST_CASE("QCDate::nthWeekdayOfMonth from start of month") {
    // 3rd Monday of January 2025 = MLK Day
    REQUIRE(QCDate::nthWeekdayOfMonth(3, QCDate::qcMonday, 1, 2025) == QCDate(20, 1, 2025));
    // 4th Thursday of November 2025 = Thanksgiving
    REQUIRE(QCDate::nthWeekdayOfMonth(4, QCDate::qcThursday, 11, 2025) == QCDate(27, 11, 2025));
}

TEST_CASE("QCDate::nthWeekdayOfMonth from end of month") {
    // last Monday of May 2025 = Memorial Day
    REQUIRE(QCDate::nthWeekdayOfMonth(-1, QCDate::qcMonday, 5, 2025) == QCDate(26, 5, 2025));
}

TEST_CASE("QCDate::lastWeekdayOfMonth delegates to nth(-1)") {
    REQUIRE(QCDate::lastWeekdayOfMonth(QCDate::qcMonday, 5, 2025) == QCDate(26, 5, 2025));
}

// ---------------------------------------------------------------------------
// HolidayRule + observance
// ---------------------------------------------------------------------------

TEST_CASE("HolidayRule validity window excludes years before fromYear") {
    auto juneteenth = HolidayRule::fixed(6, 19, 2021);
    REQUIRE_FALSE(juneteenth.resolve(2020, Observance::none).has_value());
    REQUIRE(juneteenth.resolve(2021, Observance::none).value() == QCDate(19, 6, 2021));
}

TEST_CASE("Easter-relative rule resolves Good Friday") {
    auto goodFriday = HolidayRule::easter(-2);
    REQUIRE(goodFriday.resolve(2025, Observance::none).value() == QCDate(18, 4, 2025));
}

TEST_CASE("Observance satToFriSunToMon shifts weekend holidays") {
    // 2021-12-25 (Christmas) was a Saturday -> observed Friday 2021-12-24
    auto xmas = HolidayRule::fixed(12, 25);
    REQUIRE(xmas.resolve(2021, Observance::satToFriSunToMon).value() == QCDate(24, 12, 2021));
    // 2022-12-25 was a Sunday -> observed Monday 2022-12-26
    REQUIRE(xmas.resolve(2022, Observance::satToFriSunToMon).value() == QCDate(26, 12, 2022));
}

TEST_CASE("Observance none leaves weekend holidays in place") {
    auto xmas = HolidayRule::fixed(12, 25);
    REQUIRE(xmas.resolve(2021, Observance::none).value() == QCDate(25, 12, 2021));
}

// ---------------------------------------------------------------------------
// BusinessCalendarId round-trip
// ---------------------------------------------------------------------------

TEST_CASE("FpML code round-trips for all calendars") {
    for (auto id : {BusinessCalendarId::CLSA, BusinessCalendarId::USNY,
                    BusinessCalendarId::USGS, BusinessCalendarId::EUTA}) {
        REQUIRE(fromFpmlCode(fpmlCode(id)) == id);
        REQUIRE_FALSE(description(id).empty());
    }
}

TEST_CASE("fromFpmlCode throws on unknown code") {
    REQUIRE_THROWS(fromFpmlCode("ZZZZ"));
}

// ---------------------------------------------------------------------------
// CalendarFactory
// ---------------------------------------------------------------------------

TEST_CASE("CalendarFactory builds US base holidays with observance") {
    auto cal = CalendarFactory::build(QCDate(1, 1, 2025), 1, {BusinessCalendarId::USNY});
    auto h = cal.getHolidays();
    REQUIRE(contains(h, QCDate(1, 1, 2025)));   // New Year
    REQUIRE(contains(h, QCDate(20, 1, 2025)));  // MLK
    REQUIRE(contains(h, QCDate(26, 5, 2025)));  // Memorial
    REQUIRE(contains(h, QCDate(19, 6, 2025)));  // Juneteenth
    REQUIRE(contains(h, QCDate(4, 7, 2025)));   // Independence
    REQUIRE(contains(h, QCDate(27, 11, 2025))); // Thanksgiving
    REQUIRE(contains(h, QCDate(25, 12, 2025))); // Christmas
}

TEST_CASE("USGS adds Columbus and Veterans Day; USNY does not") {
    auto usgs = CalendarFactory::build(QCDate(1, 1, 2025), 1, {BusinessCalendarId::USGS}).getHolidays();
    auto usny = CalendarFactory::build(QCDate(1, 1, 2025), 1, {BusinessCalendarId::USNY}).getHolidays();
    REQUIRE(contains(usgs, QCDate(13, 10, 2025))); // Columbus Day (2nd Mon Oct)
    REQUIRE(contains(usgs, QCDate(11, 11, 2025))); // Veterans Day
    REQUIRE_FALSE(contains(usny, QCDate(13, 10, 2025)));
    REQUIRE_FALSE(contains(usny, QCDate(11, 11, 2025)));
}

TEST_CASE("EUTA holidays") {
    auto h = CalendarFactory::build(QCDate(1, 1, 2025), 1, {BusinessCalendarId::EUTA}).getHolidays();
    REQUIRE(contains(h, QCDate(1, 1, 2025)));   // New Year
    REQUIRE(contains(h, QCDate(18, 4, 2025)));  // Good Friday
    REQUIRE(contains(h, QCDate(21, 4, 2025)));  // Easter Monday
    REQUIRE(contains(h, QCDate(1, 5, 2025)));   // Labour Day
    REQUIRE(contains(h, QCDate(25, 12, 2025))); // Christmas
    REQUIRE(contains(h, QCDate(26, 12, 2025))); // Boxing Day
}

TEST_CASE("CalendarFactory merges calendars via union") {
    auto merged = CalendarFactory::build(QCDate(1, 1, 2025), 1,
                                         {BusinessCalendarId::CLSA, BusinessCalendarId::USNY}).getHolidays();
    REQUIRE(contains(merged, QCDate(18, 9, 2025))); // CLSA Independencia
    REQUIRE(contains(merged, QCDate(4, 7, 2025)));  // USNY Independence Day
}

TEST_CASE("CLSA models Ley 20.215 Monday shift (San Pedro / Dos Mundos)") {
    auto h = CalendarFactory::build(QCDate(1, 1, 2023), 1, {BusinessCalendarId::CLSA}).getHolidays();
    // 2023: Jun 29 = Thu -> Mon Jun 26; Oct 12 = Thu -> Mon Oct 9
    REQUIRE(contains(h, QCDate(26, 6, 2023)));
    REQUIRE_FALSE(contains(h, QCDate(29, 6, 2023)));
    REQUIRE(contains(h, QCDate(9, 10, 2023)));
    REQUIRE_FALSE(contains(h, QCDate(12, 10, 2023)));
}

TEST_CASE("CLSA models Ley 20.299 Reformation-day Friday shift") {
    // 2023: Oct 31 = Tue -> Friday of previous week (Oct 27)
    auto h2023 = CalendarFactory::build(QCDate(1, 1, 2023), 1, {BusinessCalendarId::CLSA}).getHolidays();
    REQUIRE(contains(h2023, QCDate(27, 10, 2023)));
    REQUIRE_FALSE(contains(h2023, QCDate(31, 10, 2023)));
    // 2022: Oct 31 = Mon -> stays
    auto h2022 = CalendarFactory::build(QCDate(1, 1, 2022), 1, {BusinessCalendarId::CLSA}).getHolidays();
    REQUIRE(contains(h2022, QCDate(31, 10, 2022)));
}

TEST_CASE("CLSA includes solstice-based and one-off national holidays") {
    auto h = CalendarFactory::build(QCDate(1, 1, 2021), 4, {BusinessCalendarId::CLSA}).getHolidays();
    REQUIRE(contains(h, QCDate(21, 6, 2021)));  // Día de los Pueblos Indígenas (solstice)
    REQUIRE(contains(h, QCDate(20, 6, 2024)));  // solstice shifts to Jun 20
    REQUIRE(contains(h, QCDate(17, 9, 2021)));  // Fiestas Patrias bridge
    REQUIRE(contains(h, QCDate(16, 9, 2022)));  // plebiscito one-off
}

TEST_CASE("HolidayRule::fixedOnWeekday fires only on the target weekday") {
    auto sep17 = HolidayRule::fixedOnWeekday(9, 17, QCDate::qcFriday, 2017);
    // 2021: Sep 17 is a Friday -> fires
    REQUIRE(sep17.resolve(2021, Observance::none).value() == QCDate(17, 9, 2021));
    // 2025: Sep 17 is a Wednesday -> no holiday
    REQUIRE_FALSE(sep17.resolve(2025, Observance::none).has_value());
    // fromYear gate: not active before 2017 even when Sep 17 is a Friday (e.g. 2010)
    REQUIRE_FALSE(sep17.resolve(2010, Observance::none).has_value());
}

TEST_CASE("CLSA models Ley 20.983 bridges (Sep 17 / Jan 2)") {
    auto h2021 = CalendarFactory::build(QCDate(1, 1, 2021), 0, {BusinessCalendarId::CLSA}).getHolidays();
    REQUIRE(contains(h2021, QCDate(17, 9, 2021)));   // Sep 18/19 = Sat/Sun -> Sep 17 Fri
    auto h2023 = CalendarFactory::build(QCDate(1, 1, 2023), 0, {BusinessCalendarId::CLSA}).getHolidays();
    REQUIRE(contains(h2023, QCDate(2, 1, 2023)));    // Jan 1 = Sun -> Jan 2 Mon
    // Conditions not met: 2025 Sep 17 is not a Friday; 2024 Jan 1 is not a Sunday
    auto h2025 = CalendarFactory::build(QCDate(1, 1, 2025), 0, {BusinessCalendarId::CLSA}).getHolidays();
    REQUIRE_FALSE(contains(h2025, QCDate(17, 9, 2025)));
    auto h2024 = CalendarFactory::build(QCDate(1, 1, 2024), 0, {BusinessCalendarId::CLSA}).getHolidays();
    REQUIRE_FALSE(contains(h2024, QCDate(2, 1, 2024)));
}

TEST_CASE("CLBA inherits the Ley 20.983 bridges") {
    auto h2021 = CalendarFactory::build(QCDate(1, 1, 2021), 0, {BusinessCalendarId::CLBA}).getHolidays();
    REQUIRE(contains(h2021, QCDate(17, 9, 2021)));
    REQUIRE(contains(h2021, QCDate(31, 12, 2021)));  // plus the banking Dec 31
    auto h2023 = CalendarFactory::build(QCDate(1, 1, 2023), 0, {BusinessCalendarId::CLBA}).getHolidays();
    REQUIRE(contains(h2023, QCDate(2, 1, 2023)));
}

TEST_CASE("CLSA models Ley 20.215 Art 35 ter sandwich bridges") {
    // 2029: Sep 18 = Tue, Sep 19 = Wed -> Sep 17 (Mon) is a holiday
    auto h2029 = CalendarFactory::build(QCDate(1, 1, 2029), 0, {BusinessCalendarId::CLSA}).getHolidays();
    REQUIRE(contains(h2029, QCDate(17, 9, 2029)));
    // 2030: Sep 18 = Wed, Sep 19 = Thu -> Sep 20 (Fri) is a holiday
    auto h2030 = CalendarFactory::build(QCDate(1, 1, 2030), 0, {BusinessCalendarId::CLSA}).getHolidays();
    REQUIRE(contains(h2030, QCDate(20, 9, 2030)));
    // 2025: neither condition holds
    auto h2025 = CalendarFactory::build(QCDate(1, 1, 2025), 0, {BusinessCalendarId::CLSA}).getHolidays();
    REQUIRE_FALSE(contains(h2025, QCDate(17, 9, 2025)));
    REQUIRE_FALSE(contains(h2025, QCDate(20, 9, 2025)));
    // 2001: Sep 17 is a Monday but the law was not yet in force (fromYear 2007)
    auto h2001 = CalendarFactory::build(QCDate(1, 1, 2001), 0, {BusinessCalendarId::CLSA}).getHolidays();
    REQUIRE_FALSE(contains(h2001, QCDate(17, 9, 2001)));
}

TEST_CASE("CLBA inherits the Art 35 ter bridges") {
    auto h2029 = CalendarFactory::build(QCDate(1, 1, 2029), 0, {BusinessCalendarId::CLBA}).getHolidays();
    REQUIRE(contains(h2029, QCDate(17, 9, 2029)));
    REQUIRE(contains(h2029, QCDate(31, 12, 2029)));
}

TEST_CASE("CLBA is CLSA plus December 31") {
    auto clsa = CalendarFactory::build(QCDate(1, 1, 2025), 1, {BusinessCalendarId::CLSA}).getHolidays();
    auto clba = CalendarFactory::build(QCDate(1, 1, 2025), 1, {BusinessCalendarId::CLBA}).getHolidays();
    // CLBA has every CLSA holiday...
    for (const auto& d : clsa) {
        REQUIRE(contains(clba, d));
    }
    // ...plus a non-movable Dec 31, which CLSA does not have.
    REQUIRE(contains(clba, QCDate(31, 12, 2025)));
    REQUIRE_FALSE(contains(clsa, QCDate(31, 12, 2025)));
    REQUIRE(fromFpmlCode("CLBA") == BusinessCalendarId::CLBA);
}

TEST_CASE("Empty calendar list yields no holidays") {
    auto cal = CalendarFactory::build(QCDate(1, 1, 2025), 5, {});
    REQUIRE(cal.getHolidays().empty());
    REQUIRE(cal.getStartDate() == QCDate(1, 1, 2025));
    REQUIRE(cal.getLength() == 5);
}
