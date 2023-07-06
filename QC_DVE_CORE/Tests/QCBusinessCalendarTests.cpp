//
// Created by ADiazV on 21-12-2021.
//

#include "catch/catch-2.hpp"
#include "time/QCBusinessCalendar.h"

TEST_CASE("QCBusinessCalendar: constructor") {
    auto numYears = 5;
    auto fechaInicio = QCDate(12, 6, 1969);
    auto cal = QCBusinessCalendar(fechaInicio, numYears);
    auto holidays = cal.getHolidays();
    REQUIRE(holidays.size() == 6);
    REQUIRE(holidays.at(0) == QCDate(1, 1, 1969));
    REQUIRE(holidays.at(1) == QCDate(1, 1, 1970));
    REQUIRE(holidays.at(2) == QCDate(1, 1, 1971));
    REQUIRE(holidays.at(3) == QCDate(1, 1, 1972));
    REQUIRE(holidays.at(4) == QCDate(1, 1, 1973));
    REQUIRE(holidays.at(5) == QCDate(1, 1, 1974));
}

TEST_CASE("QCBusinessCalendar: Add a date in the middle") {
    auto numYears = 5;
    auto fechaInicio = QCDate(12, 6, 1969);
    auto cal = QCBusinessCalendar(fechaInicio, numYears);
    auto newHoliday = QCDate(31, 12, 1969);
    cal.addHoliday(newHoliday);
    auto holidays = cal.getHolidays();
    REQUIRE(holidays.size() == 7);
    REQUIRE(holidays.at(0) == QCDate(1, 1, 1969));
    REQUIRE(holidays.at(1) == newHoliday);
    REQUIRE(holidays.at(2) == QCDate(1, 1, 1970));
}

TEST_CASE("QCBusinessCalendar: Add a date at the beginning") {
    auto numYears = 5;
    auto fechaInicio = QCDate(12, 6, 1969);
    auto cal = QCBusinessCalendar(fechaInicio, numYears);
    auto newHoliday = QCDate(31, 12, 1968);
    cal.addHoliday(newHoliday);
    auto holidays = cal.getHolidays();
    REQUIRE(holidays.size() == 7);
    REQUIRE(holidays.at(0) == newHoliday);
    REQUIRE(holidays.at(1) == QCDate(1, 1, 1969));
}

TEST_CASE("QCBusinessCalendar: Add a date at the end") {
    auto numYears = 5;
    auto fechaInicio = QCDate(12, 6, 1969);
    auto cal = QCBusinessCalendar(fechaInicio, numYears);
    auto newHoliday = QCDate(31, 12, 1980);
    cal.addHoliday(newHoliday);
    auto holidays = cal.getHolidays();
    REQUIRE(holidays.size() == 7);
    REQUIRE(holidays.at(6) == newHoliday);
}

TEST_CASE("QCBusinessCalendar: nextBusinessDay") {
    auto numYears = 5;
    auto fechaInicio = QCDate(12, 6, 1969);
    auto cal = QCBusinessCalendar(fechaInicio, numYears);
    auto holidays = cal.getHolidays();

    auto testDate = QCDate(12, 1, 1969); // This is a sunday
    auto nextDate = cal.nextBusinessDay(testDate);
    auto expectedDate = QCDate(13, 1, 1969);
    REQUIRE(nextDate == expectedDate);

    testDate = QCDate(13, 1, 1969);
    nextDate = cal.nextBusinessDay(testDate);
    REQUIRE(nextDate == expectedDate);

    testDate = QCDate(18, 9, 1969);
    expectedDate = testDate;
    nextDate = cal.nextBusinessDay(testDate);
    REQUIRE(nextDate == expectedDate);

    cal.addHoliday(testDate);
    expectedDate = QCDate(19, 9, 1969);
    nextDate = cal.nextBusinessDay(testDate);
    REQUIRE(nextDate == expectedDate);

    cal.addHoliday(expectedDate);
    expectedDate = QCDate(22, 9, 1969);
    nextDate = cal.nextBusinessDay(testDate);
    REQUIRE(nextDate == expectedDate);
}



TEST_CASE("QCBusinessCalendar: modNextBusinessDay") {
    auto numYears = 5;
    auto fechaInicio = QCDate(12, 6, 1969);
    auto cal = QCBusinessCalendar(fechaInicio, numYears);

    auto testDate = QCDate(30, 11, 1969);
    auto expectedDate = QCDate(28, 11, 1969);
    auto nextDate = cal.modNextBusinessDay(testDate);
    REQUIRE(nextDate == expectedDate);
}

TEST_CASE("QCBusinessCalendar: previousBusinessDay") {
    auto numYears = 5;
    auto fechaInicio = QCDate(12, 6, 1969);
    auto cal = QCBusinessCalendar(fechaInicio, numYears);

    auto testDate = QCDate(30, 11, 1969);
    auto expectedDate = QCDate(28, 11, 1969);
    auto nextDate = cal.previousBusinessDay(testDate);
    REQUIRE(nextDate == expectedDate);
}

TEST_CASE("QCBusinessCalendar: shift") {
    auto numYears = 5;
    auto fechaInicio = QCDate(12, 6, 1969);
    auto cal = QCBusinessCalendar(fechaInicio, numYears);

    auto testDate = QCDate(1, 2, 1969);
    auto result = cal.shift(testDate, 0);
    auto expectedDate = QCDate(1, 2, 1969);
    REQUIRE(result == expectedDate);

    result = cal.shift(testDate, 1);
    expectedDate = QCDate(3, 2, 1969);
    REQUIRE(result == expectedDate);

    result = cal.shift(testDate, -1);
    expectedDate = QCDate(31, 1, 1969);
    REQUIRE(result == expectedDate);

    result = cal.shift(testDate, 5);
    expectedDate = QCDate(7, 2, 1969);
    REQUIRE(result == expectedDate);

    result = cal.shift(testDate, 6);
    expectedDate = QCDate(10, 2, 1969);
    REQUIRE(result == expectedDate);
}