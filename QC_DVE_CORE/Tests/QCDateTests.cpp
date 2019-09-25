//
// Created by Alvaro Diaz Valenzuela on 2019-01-14.
//

#include "catch/catch-2.hpp"
#include "time/QCDate.h"

#include <array>
#include <algorithm> // for std::find
#include <iterator> // for std::begin, std::end
#include <string>


TEST_CASE("Default constructor") {
    QCDate fecha;
    REQUIRE(fecha.day() == 12);
    REQUIRE(fecha.month() == 1);
    REQUIRE(fecha.year() == 1969);
}

TEST_CASE("Constructor QCDate::QCDate(int d, int m, int y)") {
    array<int, 7> meses31Dias {1, 3, 5, 7, 8, 10, 12};
    array<int, 4> meses30Dias {4, 6, 9, 11};
    array<int, 33> leapYears {1968, 1972, 1976, 1980, 1984, 1988, 1992, 1996,
                              2000, 2004, 2008, 2012, 2016, 2020, 2024, 2028,
                              2032, 2036, 2040, 2044, 2048, 2052, 2056, 2060,
                              2064, 2068, 2072, 2076, 2080, 2084, 2088, 2092,
                              2096};

    SECTION("Valid dates") {
        for (const auto& mes : meses31Dias) {
            for (int dia = 1; dia < 32; ++dia) {
                for (int agno = 1969; agno < 2051; ++agno) {
                    INFO("Day:" << dia << " Month:" << mes << " Year:" << agno);
                    REQUIRE_NOTHROW(QCDate(dia, mes, agno));
                    QCDate fecha {dia, mes, agno};
                    REQUIRE(fecha.day() == dia);
                    REQUIRE(fecha.month() == mes);
                    REQUIRE(fecha.year() == agno);
                }
            }
        }

        for (const auto& mes : meses30Dias) {
            for (int dia = 1; dia < 31; ++dia) {
                for (int agno = 1969; agno < 2051; ++agno) {
                    INFO("Day:" << dia << " Month:" << mes << " Year:" << agno)
                    REQUIRE_NOTHROW(QCDate(dia, mes, agno));
                    QCDate fecha {dia, mes, agno};
                    REQUIRE(fecha.day() == dia);
                    REQUIRE(fecha.month() == mes);
                    REQUIRE(fecha.year() == agno);
                }
            }
        }

        int mes = 2;
        for (int dia = 1; dia < 29; ++dia) {
            for (int agno = 1969; agno < 2051; ++agno) {
                INFO("Day:" << dia << " Month:" << mes << " Year:" << agno)
                REQUIRE_NOTHROW(QCDate(dia, mes, agno));
                QCDate fecha {dia, mes, agno};
                REQUIRE(fecha.day() == dia);
                REQUIRE(fecha.month() == mes);
                REQUIRE(fecha.year() == agno);
            }
        }

        for (const auto& agno: leapYears) {
            int dia = 29;
            int mes = 2;
            INFO("Day:" << dia << " Month:" << mes << " Year:" << agno)
            REQUIRE_NOTHROW(QCDate(dia, mes, agno));
            QCDate fecha {dia, mes, agno};
            REQUIRE(fecha.day() == dia);
            REQUIRE(fecha.month() == mes);
            REQUIRE(fecha.year() == agno);
        }
    }

    SECTION("Invalid Dates") {
        for (const auto& mes : meses30Dias) {
            int dia = 31;
            for (int agno = 1969; agno < 2051; ++agno) {
                INFO("Day:" << dia << " Month:" << mes << " Year:" << agno)
                REQUIRE_THROWS(QCDate(dia, mes, agno));
            }
        }

        for (int agno = 1969; agno < 2051; ++agno) {
            if (std::find(std::begin(leapYears), std::end(leapYears), agno) == std::end(leapYears))
            {
                int dia = 29;
                int mes = 2;
                INFO("Day:" << dia << " Month:" << mes << " Year:" << agno)
                REQUIRE_THROWS(QCDate(dia, mes, agno));
            }
        }
    }
}

TEST_CASE("Constructor QCDate::QCDate(string& stringDate)") {
    SECTION("Well padded valid dates") {
        array<std::string, 12> fechasString {"1969/01/01", "1969/02/01", "1969/03/01", "1969/04/01",
                                "1969/05/01", "1969/06/01", "1969/07/01", "1969/08/01",
                                "1969/09/01", "1969/10/01", "1969/11/01", "1969/12/01"};
        int i = 0;
        for (auto& fechaString : fechasString) {
            INFO("Fecha:" << fechaString);
            REQUIRE_NOTHROW(QCDate(fechaString));
            QCDate fecha {fechaString};
            REQUIRE(fecha.day() == 1);
            REQUIRE(fecha.month() == i + 1);
            REQUIRE(fecha.year() == 1969);
            ++i;
        }
    }

    SECTION("Well padded valid dates different separators") {
        array<std::string, 12> fechasString {"1969%01$01", "1969-02_01", "1969a03n01", "1969?04?01",
                                             "1969z05@01", "1969i06001", "1969)07?01", "1969<08>01",
                                             "1969#09#01", "1969)10(01", "1969;11,01", "1969}12{01"};
        int i = 0;
        for (auto& fechaString : fechasString) {
            INFO("Fecha:" << fechaString);
            REQUIRE_NOTHROW(QCDate(fechaString));
            QCDate fecha {fechaString};
            REQUIRE(fecha.day() == 1);
            REQUIRE(fecha.month() == i + 1);
            REQUIRE(fecha.year() == 1969);
            ++i;
        }
    }

}

TEST_CASE( "Day, month and year setters") {
    SECTION("Day setter") {
        QCDate fecha{12, 1, 1969};
        REQUIRE(fecha.day() == 12);
        for (int i = 1; i < 32; ++i) {
            fecha.setDay(i);
            REQUIRE(fecha.day() == i);
        }
    }

    SECTION("Month setter") {
        QCDate fecha {12, 1, 1969};
        REQUIRE(fecha.month() == 1);
        for (int i = 1; i < 13; ++i) {
            fecha.setMonth(i);
            REQUIRE( fecha.month() == i );
        }
    }

    SECTION("Year setter") {
        QCDate fecha {12, 1, 1969};
        REQUIRE(fecha.year() == 1969);
        for (int i = 1; i < 50; ++i) {
            fecha.setYear(i);
            REQUIRE( fecha.year() == i );
        }
    }

}


