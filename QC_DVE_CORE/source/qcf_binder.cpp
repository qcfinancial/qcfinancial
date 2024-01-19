//
// Created by Alvaro Diaz on 18-06-23.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;
using namespace pybind11::literals;

#include <asset_classes/QCCurrency.h>
#include <asset_classes/QCYearFraction.h>
#include <asset_classes/QCAct360.h>
#include <asset_classes/QCAct365.h>
#include <asset_classes/QCAct30.h>
#include <asset_classes/QCActAct.h>
#include <asset_classes/QC30360.h>
#include <asset_classes/QC3030.h>
#include <asset_classes/QCWealthFactor.h>
#include <asset_classes/QCLinearWf.h>
#include <asset_classes/QCCompoundWf.h>
#include <asset_classes/QCContinousWf.h>
#include <asset_classes/QCInterestRate.h>
#include <asset_classes/Tenor.h>
#include <asset_classes/IndexFactory.h>
#include <asset_classes/InterestRateIndex.h>
#include <asset_classes/FXRate.h>
#include <asset_classes/FXRateIndex.h>
#include <asset_classes/QCCurrencyConverter.h>
#include <asset_classes/InterestRateCurve.h>
#include <asset_classes/ZeroCouponCurve.h>

#include <cashflows/Cashflow.h>
#include <cashflows/SimpleCashflow.h>
#include <cashflows/SimpleMultiCurrencyCashflow.h>
#include <cashflows/FixedRateCashflow.h>
#include <cashflows/FixedRateCashflow2.h>
#include <cashflows/FixedRateMultiCurrencyCashflow.h>
#include <cashflows/LinearInterestRateCashflow.h>
#include <cashflows/IborCashflow.h>
#include <cashflows/IborCashflow2.h>
#include <cashflows/IborMultiCurrencyCashflow.h>
#include <cashflows/IcpClpCashflow.h>
#include <cashflows/IcpClpCashflow2.h>
#include <cashflows/IcpClfCashflow.h>
#include <cashflows/CompoundedOvernightRateCashflow.h>
#include <cashflows/CompoundedOvernightRateCashflow2.h>
#include <cashflows/CompoundedOvernightRateMultiCurrencyCashflow2.h>
#include <cashflows/OvernightIndexCashflow.h>
#include <cashflows/OvernightIndexMultiCurrencyCashflow.h>

#include "FixedRateBond.h"
#include "ChileanFixedRateBond.h"

#include <present_value/PresentValue.h>
#include <present_value/ForwardRates.h>

#include <Leg.h>
#include <LegFactory.h>

#include <curves/QCCurve.h>
#include <curves/QCLinearInterpolator.h>

#include <time/QCDate.h>
#include <time/QCBusinessCalendar.h>

#include <QcfinancialPybind11Helpers.h>

// PYBIND11_MAKE_OPAQUE(std::vector<QCDate>);
PYBIND11_MAKE_OPAQUE(std::vector<double>);

PYBIND11_MAKE_OPAQUE(std::vector<long>);

PYBIND11_MAKE_OPAQUE(std::map<QCDate, double>);

// PYBIND11_MAKE_OPAQUE(std::pair<long, double>)

namespace qf = QCode::Financial;

PYBIND11_MODULE(qcfinancial, m) {
        m.doc() = R"pbdoc(
        qcfinancial
        -----------------------

        .. currentmodule:: qcf_binder

        .. autosummary::
           :toctree: _generate

           QCDate
           QCCurrency (and subclasses)
           QCYearFraction (and subclasses)

    )pbdoc";


        m.def(
                "id",
                []() { return "48"; });

        // QCDate
        py::class_<QCDate>(m, "QCDate", R"pbdoc(
        Permite representar una fecha en calendario gregoriano.
    )pbdoc")
                        .def(py::init<int, int, int>(), R"pbdoc(
- d: int, es el día de la fecha.
- m: int, es el mes de la fecha.
- y: int, es el año de la fecha.)pbdoc")
                        .def(py::init<>())
                        .def(py::init<long>())
                        .def(py::init<std::string &>())
                        .def("set_day", &QCDate::setDay, R"pbdoc(
            Establece el día de la fecha.

            - d: int
            )pbdoc")
                        .def("set_month", &QCDate::setMonth, "Establece el mes")
                        .def("set_year", &QCDate::setYear, "Establece el año")
                        .def("day", &QCDate::day, "Obtiene el día")
                        .def("month", &QCDate::month, "Obtiene el mes")
                        .def("year", &QCDate::year, "Obtiene el año")
                        .def("week_day", &QCDate::weekDay, "Obtiene el día de la semana")
                        .def("description", &QCDate::description, "Muestra la fecha como texto", py::arg("es_iso"))
                        .def("iso_code",
                             &QCDate::description,
                             "Muestra la fecha en formato ISO. Mantener el parámetro `es_iso` en su valor por default.",
                             py::arg("es_iso") = false)
                        .def("add_months", &QCDate::addMonths)
                        .def("add_days", &QCDate::addDays)
                        .def("day_diff", &QCDate::dayDiff)
                        // .def("month_diff_day_remainder", mddiff_1)
                        // .def("month_diff_day_remainder", mddiff_2)
                        .def("__lt__", &QCDate::operator<)
                        .def("__le__", &QCDate::operator<=)
                        .def("__eq__", &QCDate::operator==)
                        .def("__ne__", &QCDate::operator!=)
                        .def("__ge__", &QCDate::operator>=)
                        .def("__gt__", &QCDate::operator>)
                        .def("__hash__", &QCDate::excelSerial)
                        .def("__str__", &QCDate::description, py::arg("es_iso") = false)
                        .def("__repr__", &QCDate::description, py::arg("es_iso") = false)
                        .def<QCDate(QCDate::*)(std::vector<QCDate>&, QCDate::QCBusDayAdjRules) const>(
                                "business_day", &QCDate::businessDay);


        m.def("build_qcdate_from_string", [](std::string& strDate) {
                return QCDate{strDate};
        });

        py::enum_<QCDate::QCWeekDay>(m, "WeekDay")
                        .value("MON", QCDate::qcMonday)
                        .value("TUE", QCDate::qcTuesday)
                        .value("WED", QCDate::qcWednesday)
                        .value("THU", QCDate::qcThursday)
                        .value("FRI", QCDate::qcFriday)
                        .value("SAT", QCDate::qcSaturday)
                        .value("SUN", QCDate::qcSunday);

        // QCBusinessCalendar
        py::class_<QCBusinessCalendar>(m, "BusinessCalendar")
                        .def(py::init<const QCDate &, int>())
                        .def("add_holiday", &QCBusinessCalendar::addHoliday)
                        .def("next_busy_day", &QCBusinessCalendar::nextBusinessDay)
                        .def("mod_next_busy_day", &QCBusinessCalendar::modNextBusinessDay)
                        .def("prev_busy_day", &QCBusinessCalendar::previousBusinessDay)
                        .def("shift", &QCBusinessCalendar::shift)
                        .def("get_holidays", &QCBusinessCalendar::getHolidays);

        // QCCurrency
        py::class_<QCCurrency, std::shared_ptr<QCCurrency>>(m, "QCCurrency")
                        .def(py::init<>())
                        .def(py::init<std::string, std::string, unsigned int, unsigned int>())
                        .def("get_name", &QCCurrency::getName)
                        .def("get_iso_code", &QCCurrency::getIsoCode)
                        .def("get_iso_number", &QCCurrency::getIsoNumber)
                        .def("get_decimal_places", &QCCurrency::getDecimalPlaces)
                        .def("amount", &QCCurrency::amount)
                        .def("__str__", &QCCurrency::getIsoCode);

        py::class_<QCSEK, std::shared_ptr<QCSEK>, QCCurrency>(m, "QCSEK")
                        .def(py::init<>());

        py::class_<QCCLF, std::shared_ptr<QCCLF>, QCCurrency>(m, "QCCLF")
                        .def(py::init<>());

        py::class_<QCCLF2, std::shared_ptr<QCCLF2>, QCCurrency>(m, "QCCLF2")
                        .def(py::init<>());

        py::class_<QCBRL, std::shared_ptr<QCBRL>, QCCurrency>(m, "QCBRL")
                        .def(py::init<>());

        py::class_<QCCLP, std::shared_ptr<QCCLP>, QCCurrency>(m, "QCCLP")
                        .def(py::init<>());

        py::class_<QCMXN, std::shared_ptr<QCMXN>, QCCurrency>(m, "QCMXN")
                        .def(py::init<>());

        py::class_<QCCAD, std::shared_ptr<QCCAD>, QCCurrency>(m, "QCCAD")
                        .def(py::init<>());

        py::class_<QCUSD, std::shared_ptr<QCUSD>, QCCurrency>(m, "QCUSD")
                        .def(py::init<>());

        py::class_<QCEUR, std::shared_ptr<QCEUR>, QCCurrency>(m, "QCEUR")
                        .def(py::init<>());

        py::class_<QCGBP, std::shared_ptr<QCGBP>, QCCurrency>(m, "QCGBP")
                        .def(py::init<>());

        py::class_<QCJPY, std::shared_ptr<QCJPY>, QCCurrency>(m, "QCJPY")
                        .def(py::init<>());

        py::class_<QCCHF, std::shared_ptr<QCCHF>, QCCurrency>(m, "QCCHF")
                        .def(py::init<>());

        py::class_<QCCOP, std::shared_ptr<QCCOP>, QCCurrency>(m, "QCCOP")
                        .def(py::init<>());

        py::class_<QCCNY, std::shared_ptr<QCCNY>, QCCurrency>(m, "QCCNY")
                        .def(py::init<>());

        py::class_<QCPEN, std::shared_ptr<QCPEN>, QCCurrency>(m, "QCPEN")
                        .def(py::init<>());

        py::class_<QCNOK, std::shared_ptr<QCNOK>, QCCurrency>(m, "QCNOK")
                        .def(py::init<>());

        py::class_<QCAUD, std::shared_ptr<QCAUD>, QCCurrency>(m, "QCAUD")
                        .def(py::init<>());

        py::class_<QCDKK, std::shared_ptr<QCDKK>, QCCurrency>(m, "QCDKK")
                        .def(py::init<>());

        py::class_<QCHKD, std::shared_ptr<QCHKD>, QCCurrency>(m, "QCHKD")
                        .def(py::init<>());

        // QCYearFraction
        py::class_<QCYearFraction, shared_ptr<QCYearFraction>>(m, "QCYearFraction",
                                                               "Clase madre de todas las fracciones de año.");

        py::class_<QCAct360, shared_ptr<QCAct360>, QCYearFraction>(m, "QCAct360",
                                                                   "Representa la fracción de año Act360.")
                        .def(py::init<>())
                        .def("yf", py::overload_cast<const QCDate &, const QCDate &>(&QCAct360::yf),
                             R"pbdoc(Calcula la fracción de año entre dos fechas:)pbdoc")
                        .def("yf", py::overload_cast<long>(&QCAct360::yf))
                        .def("count_days", &QCAct360::countDays)
                        .def("__str__", &QCAct360::description);

        py::class_<QCAct365, shared_ptr<QCAct365>, QCYearFraction>(m, "QCAct365")
                        .def(py::init<>())
                        .def("yf", py::overload_cast<const QCDate &, const QCDate &>(&QCAct365::yf))
                        .def("yf", py::overload_cast<long>(&QCAct365::yf))
                        .def("count_days", &QCAct365::countDays)
                        .def("__str__", &QCAct365::description);

        py::class_<QCAct30, shared_ptr<QCAct30>, QCYearFraction>(m, "QCAct30")
                        .def(py::init<>())
                        .def("yf", py::overload_cast<const QCDate &, const QCDate &>(&QCAct30::yf))
                        .def("yf", py::overload_cast<long>(&QCAct30::yf))
                        .def("count_days", &QCAct30::countDays)
                        .def("__str__", &QCAct30::description);

        py::class_<QCActAct, shared_ptr<QCActAct>, QCYearFraction>(m, "QCActAct")
                        .def(py::init<>())
                        .def("yf", py::overload_cast<const QCDate &, const QCDate &>(&QCActAct::yf))
                        .def("yf", py::overload_cast<long>(&QCActAct::yf))
                        .def("count_days", &QCActAct::countDays)
                        .def("__str__", &QCActAct::description);

        py::class_<QC30360, shared_ptr<QC30360>, QCYearFraction>(m, "QC30360")
                        .def(py::init<>())
                        .def("yf", py::overload_cast<const QCDate &, const QCDate &>(&QC30360::yf))
                        .def("yf", py::overload_cast<long>(&QC30360::yf))
                        .def("count_days", &QC30360::countDays)
                        .def("__str__", &QC30360::description);

        py::class_<QC3030, shared_ptr<QC3030>, QCYearFraction>(m, "QC3030")
                        .def(py::init<>())
                        .def("yf", py::overload_cast<const QCDate &, const QCDate &>(&QC3030::yf))
                        .def("yf", py::overload_cast<long>(&QC3030::yf))
                        .def("count_days", &QC3030::countDays)
                        .def("__str__", &QC3030::description);

        // QCWealthFactor
        py::class_<QCWealthFactor, shared_ptr<QCWealthFactor>>(m, "QCWealthFactor");

        py::class_<QCLinearWf, shared_ptr<QCLinearWf>, QCWealthFactor>(m, "QCLinearWf")
                        .def(py::init<>())
                        .def("wf", &QCLinearWf::wf)
                        .def("rate", &QCLinearWf::rate)
                        .def("__str__", &QCLinearWf::description);

        py::class_<QCCompoundWf, shared_ptr<QCCompoundWf>, QCWealthFactor>(m, "QCCompoundWf")
                        .def(py::init<>())
                        .def("wf", &QCCompoundWf::wf)
                        .def("rate", &QCCompoundWf::rate)
                        .def("__str__", &QCCompoundWf::description);

        py::class_<QCContinousWf, shared_ptr<QCContinousWf>, QCWealthFactor>(m, "QCContinousWf")
                        .def(py::init<>())
                        .def("wf", &QCContinousWf::wf)
                        .def("rate", &QCContinousWf::rate)
                        .def("__str__", &QCContinousWf::description);

        // QCInterestRate
        py::class_<QCInterestRate>(m, "QCInterestRate")
                        .def(py::init<double, shared_ptr<QCYearFraction>, shared_ptr<QCWealthFactor>>())
                        .def("get_value", &QCInterestRate::getValue)
                        .def("set_value", &QCInterestRate::setValue)
                        .def("wf", py::overload_cast<QCDate &, QCDate &>(&QCInterestRate::wf))
                        .def("wf", py::overload_cast<long>(&QCInterestRate::wf))
                        .def("dwf", py::overload_cast<QCDate &, QCDate &>(&QCInterestRate::dwf))
                        .def("dwf", py::overload_cast<long>(&QCInterestRate::dwf))
                        .def("d2wf", py::overload_cast<QCDate &, QCDate &>(&QCInterestRate::d2wf))
                        .def("d2wf", py::overload_cast<long>(&QCInterestRate::d2wf))
                        .def("get_rate_from_wf",
                             py::overload_cast<double, QCDate &, QCDate &>(&QCInterestRate::getRateFromWf))
                        .def("get_rate_from_wf", py::overload_cast<double, long>(&QCInterestRate::getRateFromWf))
                        .def("yf", &QCInterestRate::yf)
                        .def("get_year_fraction", &QCInterestRate::getYearFraction)
                        .def("get_wealth_factor", &QCInterestRate::getWealthFactor)
                        .def("__str__", &QCInterestRate::description);

        // Tenor
        py::class_<qf::Tenor>(m, "Tenor")
                        .def(py::init<std::string>())
                        .def("set_tenor", &qf::Tenor::setTenor)
                        .def("get_years", &qf::Tenor::getYears)
                        .def("get_months", &qf::Tenor::getMonths)
                        .def("get_days", &qf::Tenor::getDays)
                        .def("get_string", &qf::Tenor::getString)
                        .def("__str__", &qf::Tenor::getString);

        // qf::FinancialIndex
        py::class_<qf::FinancialIndex, std::shared_ptr<qf::FinancialIndex>>(m, "FinancialIndex")
                        .def(py::init<qf::FinancialIndex::AssetClass, std::string>())
                        .def("get_asset_class", &qf::FinancialIndex::getAssetClass)
                        .def("get_code", &qf::FinancialIndex::getCode);

        // qf::FinancialIndex::AssetClass
        py::enum_<qf::FinancialIndex::AssetClass>(m, "AssetClass")
                        .value("InterestRate", qf::FinancialIndex::InterestRate)
                        .value("InterestRateVol", qf::FinancialIndex::InterestRateVolatility)
                        .value("Fx", qf::FinancialIndex::Fx)
                        .value("FxVol", qf::FinancialIndex::FxVolatility)
                        .value("Equity", qf::FinancialIndex::Equity)
                        .value("EquityVol", qf::FinancialIndex::EquityVolatility)
                        .value("Commodity", qf::FinancialIndex::Commodity)
                        .value("CommodityVol", qf::FinancialIndex::CommodityVolatility)
                        .value("Credit", qf::FinancialIndex::Credit)
                        .value("CreditVol", qf::FinancialIndex::CreditVolatility);

        // InterestRateIndex
        py::class_<qf::InterestRateIndex, qf::FinancialIndex, shared_ptr<qf::InterestRateIndex>>(m, "InterestRateIndex")
                        .def(py::init<std::string,
                                QCInterestRate,
                                qf::Tenor,
                                qf::Tenor,
                                QCBusinessCalendar,
                                QCBusinessCalendar,
                                std::shared_ptr<QCCurrency>>())
                        .def("set_rate_value", &qf::InterestRateIndex::setRateValue)
                        .def("get_start_date", &qf::InterestRateIndex::getStartDate)
                        .def("get_end_date", &qf::InterestRateIndex::getEndDate)
                        .def("get_tenor", &qf::InterestRateIndex::getTenor)
                        .def("get_rate", &qf::InterestRateIndex::getRate);

        // FXRate
        py::class_<qf::FXRate, std::shared_ptr<qf::FXRate>>(m, "FXRate")
                        .def(py::init<std::shared_ptr<QCCurrency>, std::shared_ptr<QCCurrency>>())
                        .def("get_code", &qf::FXRate::getCode);

        // FXRateIndex
        m.def("get_fx_rate_index_from_fx_rate_code", &qf::getFXRateIndexFromFXRateCode);
        py::class_<qf::FXRateIndex, std::shared_ptr<qf::FXRateIndex>, qf::FinancialIndex>(m, "FXRateIndex")
                        .def(py::init<std::shared_ptr<qf::FXRate>, std::string, qf::Tenor, qf::Tenor,
                                QCBusinessCalendar>())
                        .def("fixing_date", &qf::FXRateIndex::fixingDate)
                        .def("value_date", &qf::FXRateIndex::valueDate)
                        .def("convert", &qf::FXRateIndex::convert)
                        .def("get_calendar", &qf::FXRateIndex::getCalendar);

        // QCCurrencyConverter
        py::class_<QCCurrencyConverter>(m, "QCCurrencyConverter")
                        .def(py::init<>())
                        .def("convert", py::overload_cast<
                                     double,
                                     QCCurrencyConverter::QCCurrencyEnum,
                                     double,
                                     QCCurrencyConverter::QCFxRateEnum>(&QCCurrencyConverter::convert))
                        .def("convert", py::overload_cast<
                                     double,
                                     std::shared_ptr<QCCurrency>,
                                     double,
                                     qf::FXRateIndex>(&QCCurrencyConverter::convert));

        py::enum_<QCCurrencyConverter::QCCurrencyEnum>(m, "QCCurrencyEnum")
                        .value("AUD", QCCurrencyConverter::QCCurrencyEnum::qcAUD)
                        .value("BRL", QCCurrencyConverter::QCCurrencyEnum::qcBRL)
                        .value("CAD", QCCurrencyConverter::QCCurrencyEnum::qcCAD)
                        .value("CHF", QCCurrencyConverter::QCCurrencyEnum::qcCHF)
                        .value("CLF", QCCurrencyConverter::QCCurrencyEnum::qcCLF)
                        .value("CLP", QCCurrencyConverter::QCCurrencyEnum::qcCLP)
                        .value("CNY", QCCurrencyConverter::QCCurrencyEnum::qcCNY)
                        .value("COP", QCCurrencyConverter::QCCurrencyEnum::qcCOP)
                        .value("DKK", QCCurrencyConverter::QCCurrencyEnum::qcDKK)
                        .value("EUR", QCCurrencyConverter::QCCurrencyEnum::qcEUR)
                        .value("GBP", QCCurrencyConverter::QCCurrencyEnum::qcGBP)
                        .value("HKD", QCCurrencyConverter::QCCurrencyEnum::qcHKD)
                        .value("JPY", QCCurrencyConverter::QCCurrencyEnum::qcJPY)
                        .value("MXN", QCCurrencyConverter::QCCurrencyEnum::qcMXN)
                        .value("NOK", QCCurrencyConverter::QCCurrencyEnum::qcNOK)
                        .value("PEN", QCCurrencyConverter::QCCurrencyEnum::qcPEN)
                        .value("SEK", QCCurrencyConverter::QCCurrencyEnum::qcSEK)
                        .value("USD", QCCurrencyConverter::QCCurrencyEnum::qcUSD);

        py::enum_<QCCurrencyConverter::QCFxRateEnum>(m, "QCFxRateEnum")
                        .value("AUDAUD", QCCurrencyConverter::QCFxRateEnum::qcAUDAUD)
                        .value("AUDUSD", QCCurrencyConverter::QCFxRateEnum::qcAUDUSD)
                        .value("AUDCLP", QCCurrencyConverter::QCFxRateEnum::qcAUDCLP)
                        .value("BRLBRL", QCCurrencyConverter::QCFxRateEnum::qcBRLBRL)
                        .value("USDBRL", QCCurrencyConverter::QCFxRateEnum::qcUSDBRL)
                        .value("BRLCLP", QCCurrencyConverter::QCFxRateEnum::qcBRLCLP)
                        .value("CADCAD", QCCurrencyConverter::QCFxRateEnum::qcCADCAD)
                        .value("USDCAD", QCCurrencyConverter::QCFxRateEnum::qcUSDCAD)
                        .value("CADCLP", QCCurrencyConverter::QCFxRateEnum::qcCADCLP)
                        .value("CHFCHF", QCCurrencyConverter::QCFxRateEnum::qcCHFCHF)
                        .value("USDCHF", QCCurrencyConverter::QCFxRateEnum::qcUSDCHF)
                        .value("CHFCLP", QCCurrencyConverter::QCFxRateEnum::qcCHFCLP)
                        .value("CLFCLF", QCCurrencyConverter::QCFxRateEnum::qcCLFCLF)
                        .value("USDCLF", QCCurrencyConverter::QCFxRateEnum::qcUSDCLF)
                        .value("CLFCLP", QCCurrencyConverter::QCFxRateEnum::qcCLFCLP)
                        .value("CLPCLP", QCCurrencyConverter::QCFxRateEnum::qcCLPCLP)
                        .value("USDCLP", QCCurrencyConverter::QCFxRateEnum::qcUSDCLP)
                        .value("CNYCNY", QCCurrencyConverter::QCFxRateEnum::qcCNYCNY)
                        .value("USDCNY", QCCurrencyConverter::QCFxRateEnum::qcUSDCNY)
                        .value("CNYCLP", QCCurrencyConverter::QCFxRateEnum::qcCNYCLP)
                        .value("COPCOP", QCCurrencyConverter::QCFxRateEnum::qcCOPCOP)
                        .value("USDCOP", QCCurrencyConverter::QCFxRateEnum::qcUSDCOP)
                        .value("COPCLP", QCCurrencyConverter::QCFxRateEnum::qcCOPCLP)
                        .value("DKKDKK", QCCurrencyConverter::QCFxRateEnum::qcDKKDKK)
                        .value("USDDKK", QCCurrencyConverter::QCFxRateEnum::qcUSDDKK)
                        .value("DKKCLP", QCCurrencyConverter::QCFxRateEnum::qcDKKCLP)
                        .value("EUREUR", QCCurrencyConverter::QCFxRateEnum::qcEUREUR)
                        .value("EURUSD", QCCurrencyConverter::QCFxRateEnum::qcEURUSD)
                        .value("EURCLP", QCCurrencyConverter::QCFxRateEnum::qcEURCLP)
                        .value("GBPGBP", QCCurrencyConverter::QCFxRateEnum::qcGBPGBP)
                        .value("GBPUSD", QCCurrencyConverter::QCFxRateEnum::qcGBPUSD)
                        .value("GBPCLP", QCCurrencyConverter::QCFxRateEnum::qcGBPCLP)
                        .value("HKDHKD", QCCurrencyConverter::QCFxRateEnum::qcHKDHKD)
                        .value("USDHKD", QCCurrencyConverter::QCFxRateEnum::qcUSDHKD)
                        .value("HKDCLP", QCCurrencyConverter::QCFxRateEnum::qcHKDCLP)
                        .value("JPYJPY", QCCurrencyConverter::QCFxRateEnum::qcJPYJPY)
                        .value("USDJPY", QCCurrencyConverter::QCFxRateEnum::qcUSDJPY)
                        .value("JPYCLP", QCCurrencyConverter::QCFxRateEnum::qcJPYCLP)
                        .value("MXNMXN", QCCurrencyConverter::QCFxRateEnum::qcMXNMXN)
                        .value("USDMXN", QCCurrencyConverter::QCFxRateEnum::qcUSDMXN)
                        .value("MXNCLP", QCCurrencyConverter::QCFxRateEnum::qcMXNCLP)
                        .value("NOKNOK", QCCurrencyConverter::QCFxRateEnum::qcNOKNOK)
                        .value("USDNOK", QCCurrencyConverter::QCFxRateEnum::qcUSDNOK)
                        .value("NOKCLP", QCCurrencyConverter::QCFxRateEnum::qcNOKCLP)
                        .value("PENPEN", QCCurrencyConverter::QCFxRateEnum::qcPENPEN)
                        .value("USDPEN", QCCurrencyConverter::QCFxRateEnum::qcUSDPEN)
                        .value("PENCLP", QCCurrencyConverter::QCFxRateEnum::qcPENCLP)
                        .value("SEKSEK", QCCurrencyConverter::QCFxRateEnum::qcSEKSEK)
                        .value("USDSEK", QCCurrencyConverter::QCFxRateEnum::qcUSDSEK)
                        .value("SEKCLP", QCCurrencyConverter::QCFxRateEnum::qcSEKCLP)
                        .value("USDUSD", QCCurrencyConverter::QCFxRateEnum::qcUSDUSD);

        // RecPay
        py::enum_<qf::RecPay>(m, "RecPay")
                        .value("RECEIVE", qf::Receive)
                        .value("PAY", qf::Pay);

        // Cashflow
        py::class_<qf::Cashflow, PyCashflow, std::shared_ptr<qf::Cashflow>>(m, "Cashflow")
                        .def(py::init<>())
                        .def("amount", &qf::Cashflow::amount)
                        .def("ccy", &qf::Cashflow::ccy)
                        .def("date", &qf::Cashflow::date);

        // SimpleCashflow
        py::class_<qf::SimpleCashflow, qf::Cashflow, std::shared_ptr<qf::SimpleCashflow>>(m, "SimpleCashflow")
                        .def(py::init<const QCDate &, double, std::shared_ptr<QCCurrency>>())
                        .def("get_type", &qf::SimpleCashflow::getType);

        m.def("show", py::overload_cast<const std::shared_ptr<qf::SimpleCashflow> &>(&show));

        // TimeSeries
        py::bind_map<std::map<QCDate, double>>(m, "time_series");

        // DateList
        py::bind_vector<std::vector<QCDate>>(m, "DateList");

        // CashflowVec
        py::bind_vector<std::vector<std::shared_ptr<qf::Cashflow>>>(m, "CashflowVec");

        // SimpleMultiCurrencyCashflow
        py::class_<qf::SimpleMultiCurrencyCashflow, qf::SimpleCashflow, std::shared_ptr<
                                qf::SimpleMultiCurrencyCashflow>>(
                                m, "SimpleMultiCurrencyCashflow")
                        .def(py::init<QCDate &, double, std::shared_ptr<QCCurrency>, QCDate &, std::shared_ptr<
                                        QCCurrency>,
                                std::shared_ptr<qf::FXRateIndex>, double>())
                        .def("amount", &qf::SimpleMultiCurrencyCashflow::amount)
                        .def("nominal", &qf::SimpleMultiCurrencyCashflow::nominal)
                        .def("ccy", &qf::SimpleMultiCurrencyCashflow::ccy)
                        .def("settlement_ccy", &qf::SimpleMultiCurrencyCashflow::settlementCcy)
                        .def("set_fx_rate_index_value", &qf::SimpleMultiCurrencyCashflow::setFxRateIndexValue)
                        .def("date", &qf::SimpleMultiCurrencyCashflow::date)
                        .def("get_type", &qf::SimpleMultiCurrencyCashflow::getType);

        // FixedRateCashflow
        py::class_<qf::FixedRateCashflow, qf::Cashflow, std::shared_ptr<qf::FixedRateCashflow>>(
                                m, "FixedRateCashflow")
                        .def(py::init<QCDate &, QCDate &, QCDate &, double, double, bool, const QCInterestRate &,
                                shared_ptr<QCCurrency>>())
                        .def("amount", &qf::FixedRateCashflow::amount)
                        .def("accrued_interest", &qf::FixedRateCashflow::accruedInterest)
                        .def("ccy", &qf::FixedRateCashflow::ccy)
                        .def("get_settlement_date", &qf::FixedRateCashflow::date)
                        .def("get_start_date", &qf::FixedRateCashflow::getStartDate)
                        .def("get_end_date", &qf::FixedRateCashflow::getEndDate)
                        .def("get_rate", &qf::FixedRateCashflow::getRate)
                        .def("set_nominal", &qf::FixedRateCashflow::setNominal)
                        .def("get_nominal", &qf::FixedRateCashflow::getNominal)
                        .def("set_amortization", &qf::FixedRateCashflow::setAmortization)
                        .def("get_amortization", &qf::FixedRateCashflow::getAmortization)
                        .def("get_type", &qf::FixedRateCashflow::getType)
                        .def("settlement_amount", &qf::FixedRateCashflow::settlementAmount)
                        .def("settlement_currency", &qf::FixedRateCashflow::settlementCurrency);

        m.def("show", py::overload_cast<const std::shared_ptr<qf::FixedRateCashflow> &>(&show));

        // FixedRateMulticurrencyCashflow
        py::class_<qf::FixedRateMultiCurrencyCashflow, std::shared_ptr<qf::FixedRateMultiCurrencyCashflow>,
                                qf::FixedRateCashflow>
                        (m, "FixedRateMultiCurrencyCashflow")
                        .def(py::init<
                                QCDate &, QCDate &, QCDate &,
                                double, double, bool,
                                const QCInterestRate &, shared_ptr<QCCurrency>, QCDate &,
                                shared_ptr<QCCurrency>, shared_ptr<qf::FXRateIndex>, double>())
                        // .def("amount", &qf::FixedRateMultiCurrencyCashflow::amount)
                        .def("get_fx_fixing_date", &qf::FixedRateMultiCurrencyCashflow::getFXPublishDate)
                        .def("settlement_currency", &qf::FixedRateMultiCurrencyCashflow::settlementCcy)
                        .def("set_fx_rate_index_value", &qf::FixedRateMultiCurrencyCashflow::setFxRateIndexValue)
                        .def("accrued_interest", &qf::FixedRateCashflow::accruedInterest)
                        .def("accrued_interest", &qf::FixedRateMultiCurrencyCashflow::accruedInterest)
                        .def("get_amortization", &qf::FixedRateCashflow::getAmortization)
                        .def("get_amortization", &qf::FixedRateMultiCurrencyCashflow::getAmortization)
                        .def("get_fx_rate_index", &qf::FixedRateMultiCurrencyCashflow::getFXRateIndex)
                        .def("get_fx_rate_index_code", &qf::FixedRateMultiCurrencyCashflow::getFXRateIndexCode)
                        .def("get_type", &qf::FixedRateMultiCurrencyCashflow::getType)
                        .def("settlement_amount", &qf::FixedRateMultiCurrencyCashflow::settlementAmount)
                        .def("settlement_currency", &qf::FixedRateMultiCurrencyCashflow::settlementCurrency);

        // LinearInterestRateCashflow
        py::class_<qf::LinearInterestRateCashflow, std::shared_ptr<qf::LinearInterestRateCashflow>,
                                PyLinearInterestRateCashflow>(
                                m, "LinearInterestRateCashflow")
                        .def("get_type", &qf::LinearInterestRateCashflow::getType)
                        .def("get_initial_currency", &qf::LinearInterestRateCashflow::getInitialCcy)
                        .def("does_amortize", &qf::LinearInterestRateCashflow::doesAmortize)
                        .def("get_start_date", &qf::LinearInterestRateCashflow::getStartDate)
                        .def("get_end_date", &qf::LinearInterestRateCashflow::getEndDate)
                        .def("get_settlement_date", &qf::LinearInterestRateCashflow::getSettlementDate)
                        .def("get_fixing_dates", &qf::LinearInterestRateCashflow::getFixingDates)
                        .def("get_nominal", &qf::LinearInterestRateCashflow::getNominal)
                        .def("nominal", &qf::LinearInterestRateCashflow::nominal)
                        .def("get_amortization", &qf::LinearInterestRateCashflow::getAmortization)
                        .def("interest", py::overload_cast<>(&qf::LinearInterestRateCashflow::interest))
                        .def("interest",
                             py::overload_cast<const qf::TimeSeries &>(&qf::LinearInterestRateCashflow::interest))
                        .def("fixing", py::overload_cast<>(&qf::LinearInterestRateCashflow::fixing))
                        .def("fixing", py::overload_cast<const qf::TimeSeries &>(
                                     &qf::LinearInterestRateCashflow::fixing))
                        .def("accrued_interest",
                             py::overload_cast<const QCDate &>(&qf::LinearInterestRateCashflow::accruedInterest))
                        .def("accrued_interest",
                             py::overload_cast<const QCDate &, const qf::TimeSeries &>(
                                     &qf::LinearInterestRateCashflow::accruedInterest))
                        .def("accrued_fixing",
                             py::overload_cast<const QCDate &>(&qf::LinearInterestRateCashflow::accruedFixing))
                        .def("accrued_fixing",
                             py::overload_cast<const QCDate &, const qf::TimeSeries &>(
                                     &qf::LinearInterestRateCashflow::accruedFixing));

        // FixedRateCashflow2
        py::class_<qf::FixedRateCashflow2, qf::LinearInterestRateCashflow, std::shared_ptr<qf::FixedRateCashflow2>>(
                                m, "FixedRateCashflow2")
                        .def(py::init<QCDate &, QCDate &, QCDate &, double, double, bool, const QCInterestRate &,
                                shared_ptr<QCCurrency>>())
                        .def("amount", &qf::FixedRateCashflow2::amount)
                        .def("ccy", &qf::FixedRateCashflow2::ccy)
                        .def("date", &qf::FixedRateCashflow2::date)
                        .def("get_type", &qf::FixedRateCashflow2::getType)
                        .def("get_start_date", &qf::FixedRateCashflow2::getStartDate)
                        .def("get_end_date", &qf::FixedRateCashflow2::getEndDate)
                        .def("get_settlement_date", &qf::FixedRateCashflow2::getSettlementDate)
                        .def("get_fixing_dates", &qf::FixedRateCashflow2::getFixingDates)
                        .def("get_nominal", &qf::FixedRateCashflow2::getNominal)
                        .def("nominal", &qf::FixedRateCashflow2::nominal)
                        .def("get_amortization", &qf::FixedRateCashflow2::getAmortization)
                        .def("amortization", &qf::FixedRateCashflow2::amortization)
                        .def("get_interest_rate_value", &qf::FixedRateCashflow2::getInterestRateValue)
                        .def("get_interest_rate_type", &qf::FixedRateCashflow2::getInterestRateType)
                        .def<double(qf::FixedRateCashflow2::*)()>("interest", &qf::FixedRateCashflow2::interest)
                        .def<double(qf::FixedRateCashflow2::*)(const qf::TimeSeries&)>(
                                "interest", &qf::FixedRateCashflow2::interest)
                        .def<double(qf::FixedRateCashflow2::*)()>("fixing", &qf::FixedRateCashflow2::fixing)
                        .def<double(qf::FixedRateCashflow2::*)(const qf::TimeSeries&)>(
                                "fixing", &qf::FixedRateCashflow2::fixing)
                        .def<double(qf::FixedRateCashflow2::*)(const QCDate&)>(
                                "accrued_interest", &qf::FixedRateCashflow2::accruedInterest)
                        .def<double(qf::FixedRateCashflow2::*)(const QCDate&, const qf::TimeSeries&)>(
                                "accrued_interest", &qf::FixedRateCashflow2::accruedInterest)
                        .def<double(qf::FixedRateCashflow2::*)(const QCDate&)>(
                                "accrued_fixing", &qf::FixedRateCashflow2::accruedFixing)
                        .def<double(qf::FixedRateCashflow2::*)(const QCDate&, const qf::TimeSeries&)>(
                                "accrued_fixing", &qf::FixedRateCashflow2::accruedFixing)
                        .def("does_amortize", &qf::FixedRateCashflow2::doesAmortize);

        m.def("show", py::overload_cast<const std::shared_ptr<qf::FixedRateCashflow2> &>(&show));

        // IborCashflow
        py::class_<qf::IborCashflow, qf::Cashflow, std::shared_ptr<qf::IborCashflow>>(
                                m, "IborCashflow")
                        .def(py::init<std::shared_ptr<qf::InterestRateIndex>,
                                QCDate &, QCDate &, QCDate &, QCDate &,
                                double, double, bool,
                                shared_ptr<QCCurrency>, double, double>())
                        .def("amount", &qf::IborCashflow::amount)
                        .def("accrued_interest", &qf::IborCashflow::accruedInterest)
                        .def("ccy", &qf::IborCashflow::ccy)
                        .def("get_fixing_date", &qf::IborCashflow::getFixingDate)
                        .def("get_start_date", &qf::IborCashflow::getStartDate)
                        .def("get_end_date", &qf::IborCashflow::getEndDate)
                        .def("get_settlement_date", &qf::IborCashflow::date)
                        .def("get_interest_rate_index_code", &qf::IborCashflow::getInterestRateIndexCode)
                        .def("set_interest_rate_value", &qf::IborCashflow::setInterestRateValue)
                        .def("get_interest_rate_value", &qf::IborCashflow::getInterestRateValue)
                        .def("set_nominal", &qf::IborCashflow::setNominal)
                        .def("get_nominal", &qf::IborCashflow::getNominal)
                        .def("set_amortization", &qf::IborCashflow::setAmortization)
                        .def("get_amortization", &qf::IborCashflow::getAmortization)
                        .def("get_type", &qf::IborCashflow::getType)
                        .def("settlement_amount", &qf::IborCashflow::settlementAmount)
                        .def("settlement_currency", &qf::IborCashflow::settlementCurrency);

        m.def("show", py::overload_cast<const std::shared_ptr<qf::IborCashflow> &>(&show));

        // IborCashflow2
        py::class_<qf::IborCashflow2, std::shared_ptr<qf::IborCashflow2>, qf::LinearInterestRateCashflow>(
                                m, "IborCashflow2")
                        .def(py::init<std::shared_ptr<qf::InterestRateIndex>,
                                QCDate &, QCDate &, QCDate &, QCDate &,
                                double, double, bool,
                                shared_ptr<QCCurrency>,
                                double, double>())
                        .def("amount", &qf::IborCashflow2::amount)
                        .def("ccy", &qf::IborCashflow2::ccy)
                        .def("date", &qf::IborCashflow2::date)
                        .def("get_type", &qf::IborCashflow2::getType)
                        .def("get_interest_rate_index", &qf::IborCashflow2::getInterestRateIndex)
                        .def("get_start_date", &qf::IborCashflow2::getStartDate)
                        .def("get_end_date", &qf::IborCashflow2::getEndDate)
                        .def("get_settlement_date", &qf::IborCashflow2::getSettlementDate)
                        .def("get_fixing_dates", &qf::IborCashflow2::getFixingDates)
                        .def("get_nominal", &qf::IborCashflow2::getNominal)
                        .def("nominal", &qf::IborCashflow2::nominal)
                        .def("get_amortization", &qf::IborCashflow2::getAmortization)
                        .def("amortization", &qf::IborCashflow2::amortization)
                        .def("get_spread", &qf::IborCashflow2::getSpread)
                        .def("get_gearing", &qf::IborCashflow2::getGearing)
                        .def("set_nominal", &qf::IborCashflow2::setNominal)
                        .def("set_amortization", &qf::IborCashflow2::setAmortization)
                        .def("set_rate_value", &qf::IborCashflow2::setRateValue)
                        .def("get_amount_derivatives", &qf::IborCashflow2::getAmountDerivatives)
                        .def<double(qf::IborCashflow2::*)()>("interest", &qf::IborCashflow2::interest)
                        .def<double(qf::IborCashflow2::*)(const qf::TimeSeries&)>(
                                "interest", &qf::IborCashflow2::interest)
                        .def<double(qf::IborCashflow2::*)()>("fixing", &qf::IborCashflow2::fixing)
                        .def<double(qf::IborCashflow2::*)(const qf::TimeSeries&)>("fixing", &qf::IborCashflow2::fixing)
                        .def<double(qf::IborCashflow2::*)(const QCDate&)>(
                                "accrued_interest", &qf::IborCashflow2::accruedInterest)
                        .def<double(qf::IborCashflow2::*)(const QCDate&, const qf::TimeSeries&)>(
                                "accrued_interest", &qf::IborCashflow2::accruedInterest)
                        .def<double(qf::IborCashflow2::*)(const QCDate&)>(
                                "accrued_fixing", &qf::IborCashflow2::accruedFixing)
                        .def<double(qf::IborCashflow2::*)(const QCDate&, const qf::TimeSeries&)>(
                                "accrued_fixing", &qf::IborCashflow2::accruedFixing)
                        .def("does_amortize", &qf::IborCashflow2::doesAmortize);

        m.def("show", py::overload_cast<const std::shared_ptr<qf::IborCashflow2> &>(&show));

        // IbrMultiCurrencyCashflow
        py::class_<qf::IborMultiCurrencyCashflow, std::shared_ptr<qf::IborMultiCurrencyCashflow>, qf::IborCashflow>(
                                m, "IborMultiCurrencyCashflow")
                        .def(py::init<std::shared_ptr<qf::InterestRateIndex>,
                                const QCDate &, const QCDate &, const QCDate &, const QCDate &,
                                double, double, bool,
                                std::shared_ptr<QCCurrency>,
                                double, double,
                                const QCDate &,
                                std::shared_ptr<QCCurrency>,
                                std::shared_ptr<qf::FXRateIndex>,
                                double>())
                        .def("amount", &qf::IborMultiCurrencyCashflow::amount)
                        .def("settlement_currency", &qf::IborMultiCurrencyCashflow::settlementCurrency)
                        .def("settlement_amount", &qf::IborMultiCurrencyCashflow::settlementAmount)
                        .def("get_fx_fixing_date", &qf::IborMultiCurrencyCashflow::getFXFixingDate)
                        .def("set_fx_rate_index_value", &qf::IborMultiCurrencyCashflow::setFxRateIndexValue)
                        .def("get_fx_rate_index_code", &qf::IborMultiCurrencyCashflow::getFXRateIndexCode)
                        .def("accrued_interest", &qf::IborCashflow::accruedInterest)
                        .def("accrued_interest", &qf::IborMultiCurrencyCashflow::accruedInterest);

        // IcpClpCashflow
        py::class_<qf::IcpClpCashflow, std::shared_ptr<qf::IcpClpCashflow>, qf::Cashflow>(
                                m, "IcpClpCashflow")
                        .def(py::init<QCDate &, QCDate &, QCDate &,
                                double, double, bool, double, double, double, double>())
                        .def("amount", &qf::IcpClpCashflow::amount)
                        .def("ccy", &qf::IcpClpCashflow::ccy)
                        .def("date", &qf::IcpClpCashflow::date)
                        .def("get_start_date", &qf::IcpClpCashflow::getStartDate)
                        .def("get_end_date", &qf::IcpClpCashflow::getEndDate)
                        .def("get_settlement_date", &qf::IcpClpCashflow::getSettlementDate)
                        .def("set_start_date_icp", &qf::IcpClpCashflow::setStartDateICP)
                        .def("get_start_date_icp", &qf::IcpClpCashflow::getStartDateICP)
                        .def("set_end_date_icp", &qf::IcpClpCashflow::setEndDateICP)
                        .def("get_end_date_icp", &qf::IcpClpCashflow::getEndDateICP)
                        .def("set_nominal", &qf::IcpClpCashflow::setNominal)
                        .def("get_nominal", &qf::IcpClpCashflow::getNominal)
                        .def("set_amortization", &qf::IcpClpCashflow::setAmortization)
                        .def("get_amortization", &qf::IcpClpCashflow::getAmortization)
                        .def("get_rate_value", &qf::IcpClpCashflow::getRateValue)
                        .def("get_type_of_rate", &qf::IcpClpCashflow::getTypeOfRate)
                        .def("get_tna", &qf::IcpClpCashflow::getTna)
                        .def("accrued_interest", &qf::IcpClpCashflow::accruedInterest)
                        .def("set_tna_decimal_places", &qf::IcpClpCashflow::setTnaDecimalPlaces)
                        .def("get_type", &qf::IcpClpCashflow::getType);

        m.def("show", py::overload_cast<const std::shared_ptr<qf::IcpClpCashflow> &>(&show));

        // OvernightIndexCashflow
        py::class_<qf::OvernightIndexCashflow, std::shared_ptr<qf::OvernightIndexCashflow>, qf::Cashflow>(
                                m, "OvernightIndexCashflow")
                        .def(py::init<const QCDate &, const QCDate &, const QCDate &, const QCDate &, const QCDate &,
                                std::shared_ptr<QCCurrency>,
                                double, double, bool, double, double,
                                const QCInterestRate &,
                                std::string,
                                unsigned int>())
                        .def("amount", &qf::OvernightIndexCashflow::amount)
                        .def("ccy", &qf::OvernightIndexCashflow::ccy)
                        .def("date", &qf::OvernightIndexCashflow::date)
                        .def("get_start_date", &qf::OvernightIndexCashflow::getStartDate)
                        .def("get_end_date", &qf::OvernightIndexCashflow::getEndDate)
                        .def("get_index_start_date", &qf::OvernightIndexCashflow::getIndexStartDate)
                        .def("get_index_end_date", &qf::OvernightIndexCashflow::getIndexEndDate)
                        .def("get_settlement_date", &qf::OvernightIndexCashflow::getSettlementDate)
                        .def("set_start_date_index", &qf::OvernightIndexCashflow::setStartDateIndex)
                        .def("set_end_date_index", &qf::OvernightIndexCashflow::setEndDateIndex)
                        .def("get_start_date_index", &qf::OvernightIndexCashflow::getStartDateIndex)
                        .def("get_end_date_index", &qf::OvernightIndexCashflow::getEndDateIndex)
                        .def("set_nominal", &qf::OvernightIndexCashflow::setNotional)
                        .def("get_nominal", &qf::OvernightIndexCashflow::getNotional)
                        .def("set_amortization", &qf::OvernightIndexCashflow::setAmortization)
                        .def("get_amortization", &qf::OvernightIndexCashflow::getAmortization)
                        .def("get_rate_value", &qf::OvernightIndexCashflow::getRateValue)
                        .def("get_type_of_rate", &qf::OvernightIndexCashflow::getTypeOfRate)
                        .def("get_eq_rate", &qf::OvernightIndexCashflow::getEqRate)
                        .def<double(qf::OvernightIndexCashflow::*)(QCDate&, double)>(
                                "accrued_interest", &qf::OvernightIndexCashflow::accruedInterest)
                        .def<double(qf::OvernightIndexCashflow::*)(const QCDate&, const qf::TimeSeries&)>(
                                "accrued_interest", &qf::OvernightIndexCashflow::accruedInterest)
                        .def("set_eq_rate_decimal_places", &qf::OvernightIndexCashflow::setEqRateDecimalPlaces)
                        .def("get_type", &qf::OvernightIndexCashflow::getType)
                        .def("get_index_code", &qf::OvernightIndexCashflow::getIndexCode)
                        .def("settlement_currency", &qf::OvernightIndexCashflow::settlementCurrency)
                        .def("get_amount_derivatives", &qf::OvernightIndexCashflow::getAmountDerivatives)
                        .def("get_start_date_index_derivatives",
                             &qf::OvernightIndexCashflow::getStartDateIndexDerivatives)
                        .def("get_end_date_index_derivatives", &qf::OvernightIndexCashflow::getEndDateIndexDerivatives)
                        .def("settlement_amount", &qf::OvernightIndexCashflow::settlementAmount);

        m.def("show", py::overload_cast<const std::shared_ptr<qf::OvernightIndexCashflow> &>(&show));

        // OvernightIndexMultiCurrencyCashflow
        py::class_<qf::OvernightIndexMultiCurrencyCashflow, std::shared_ptr<qf::OvernightIndexMultiCurrencyCashflow>,
                                qf::OvernightIndexCashflow>(
                                m, "OvernightIndexMultiCurrencyCashflow")
                        .def(py::init<const QCDate &, const QCDate &, const QCDate &, const QCDate &, const QCDate &,
                                std::shared_ptr<QCCurrency>,
                                double, double, bool, double, double,
                                const QCInterestRate &,
                                std::string,
                                unsigned int,
                                const QCDate &,
                                std::shared_ptr<QCCurrency>,
                                std::shared_ptr<qf::FXRateIndex>>())
                        .def("settlement_currency", &qf::OvernightIndexMultiCurrencyCashflow::settlementCurrency)
                        .def("settlement_amount", &qf::OvernightIndexMultiCurrencyCashflow::settlementAmount)
                        .def("set_fx_rate_index_value", &qf::OvernightIndexMultiCurrencyCashflow::setFxRateIndexValue)
                        .def("get_fx_rate_index_value", &qf::OvernightIndexMultiCurrencyCashflow::getFXRateIndexValue)
                        .def("get_fx_rate_index", &qf::OvernightIndexMultiCurrencyCashflow::getFXRateIndex)
                        .def("get_fx_rate_index_code", &qf::OvernightIndexMultiCurrencyCashflow::getFXRateIndexCode)
                        .def("get_fx_rate_index_fixing_date",
                             &qf::OvernightIndexMultiCurrencyCashflow::getFXRateIndexFixingDate)
                        .def("get_type", &qf::OvernightIndexMultiCurrencyCashflow::getType)
                        .def<double(qf::OvernightIndexMultiCurrencyCashflow::*)(
                                const qf::TimeSeries&, const qf::TimeSeries&)>(
                                "settlement_ccy_interest",
                                &qf::OvernightIndexMultiCurrencyCashflow::settlementCurrencyInterest)
                        .def<double(qf::OvernightIndexMultiCurrencyCashflow::*)()>(
                                "settlement_ccy_interest",
                                &qf::OvernightIndexMultiCurrencyCashflow::settlementCurrencyInterest)
                        .def<double(qf::OvernightIndexMultiCurrencyCashflow::*)(
                                const qf::TimeSeries&, const qf::TimeSeries&)>(
                                "settlement_ccy_amortization",
                                &qf::OvernightIndexMultiCurrencyCashflow::settlementCurrencyAmortization)
                        .def<double(qf::OvernightIndexMultiCurrencyCashflow::*)()>(
                                "settlement_ccy_amortization",
                                &qf::OvernightIndexMultiCurrencyCashflow::settlementCurrencyAmortization)
                        .def<double(qf::OvernightIndexMultiCurrencyCashflow::*)(
                                const qf::TimeSeries&, const qf::TimeSeries&)>(
                                "settlement_ccy_amount",
                                &qf::OvernightIndexMultiCurrencyCashflow::settlementCurrencyAmount)
                        .def<double(qf::OvernightIndexMultiCurrencyCashflow::*)()>(
                                "settlement_ccy_amount",
                                &qf::OvernightIndexMultiCurrencyCashflow::settlementCurrencyAmount);


        // IcpClpCashflow2
        py::class_<qf::IcpClpCashflow2, std::shared_ptr<qf::IcpClpCashflow2>, qf::LinearInterestRateCashflow>(
                                m, "IcpClpCashflow2")
                        .def(py::init<QCDate &, QCDate &, QCDate &, double, double, bool, double, double, bool, double,
                                double>())
                        .def("amount", &qf::IcpClpCashflow2::amount)
                        .def("ccy", &qf::IcpClpCashflow2::ccy)
                        .def("date", &qf::IcpClpCashflow2::date)
                        .def("get_type", &qf::IcpClpCashflow2::getType)
                        .def("get_start_date", &qf::IcpClpCashflow2::getStartDate)
                        .def("get_end_date", &qf::IcpClpCashflow2::getEndDate)
                        .def("get_settlement_date", &qf::IcpClpCashflow2::getSettlementDate)
                        .def("get_fixing_dates", &qf::IcpClpCashflow2::getFixingDates)
                        .def("get_nominal", &qf::IcpClpCashflow2::getNominal)
                        .def("nominal", &qf::IcpClpCashflow2::nominal)
                        .def("get_amortization", &qf::IcpClpCashflow2::getAmortization)
                        .def("amortization", &qf::IcpClpCashflow2::amortization)
                        .def<double(qf::IcpClpCashflow2::*)()>("interest", &qf::IcpClpCashflow2::interest)
                        .def<double(qf::IcpClpCashflow2::*)(const qf::TimeSeries&)>(
                                "interest", &qf::IcpClpCashflow2::interest)
                        .def<double(qf::IcpClpCashflow2::*)()>("fixing", &qf::IcpClpCashflow2::fixing)
                        .def<double(qf::IcpClpCashflow2::*)(const qf::TimeSeries&)>(
                                "fixing", &qf::IcpClpCashflow2::fixing)
                        .def<double(qf::IcpClpCashflow2::*)(const QCDate&)>(
                                "accrued_interest", &qf::IcpClpCashflow2::accruedInterest)
                        .def<double(qf::IcpClpCashflow2::*)(const QCDate&, const qf::TimeSeries&)>(
                                "accrued_interest", &qf::IcpClpCashflow2::accruedInterest)
                        .def<double(qf::IcpClpCashflow2::*)(const QCDate&)>(
                                "accrued_fixing", &qf::IcpClpCashflow2::accruedFixing)
                        .def<double(qf::IcpClpCashflow2::*)(const QCDate&, const qf::TimeSeries&)>(
                                "accrued_fixing", &qf::IcpClpCashflow2::accruedFixing)
                        .def("does_amortize", &qf::IcpClpCashflow2::doesAmortize)
                        .def("get_start_date_icp", &qf::IcpClpCashflow2::getStartDateICP)
                        .def("get_end_date_icp", &qf::IcpClpCashflow2::getEndDateICP)
                        .def("get_rate_value", &qf::IcpClpCashflow2::getRateValue)
                        .def("get_type_of_rate", &qf::IcpClpCashflow2::getTypeOfRate)
                        .def("set_tna_decimal_places", &qf::IcpClpCashflow2::setTnaDecimalPlaces)
                        .def("set_nominal", &qf::IcpClpCashflow2::setNominal)
                        .def("set_amortization", &qf::IcpClpCashflow2::setAmortization)
                        .def("set_start_date_icp", &qf::IcpClpCashflow2::setStartDateICP)
                        .def("set_end_date_icp", &qf::IcpClpCashflow2::setEndDateICP)
                        .def("get_tna", &qf::IcpClpCashflow2::getTna)
                        .def("get_spread", &qf::IcpClpCashflow2::getSpread)
                        .def("get_gearing", &qf::IcpClpCashflow2::getGearing)
                        .def("get_start_date_icp_derivatives", &qf::IcpClpCashflow2::getStartDateICPDerivatives)
                        .def("get_amount_derivatives", &qf::IcpClpCashflow2::getAmountDerivatives)
                        .def("get_end_date_icp_derivatives", &qf::IcpClpCashflow2::getEndDateICPDerivatives);

        m.def("show", py::overload_cast<const std::shared_ptr<qf::IcpClpCashflow2> &>(&show));

        // IcpClfCashflow
        py::class_<qf::IcpClfCashflow, std::shared_ptr<qf::IcpClfCashflow>, qf::IcpClpCashflow>(
                                m, "IcpClfCashflow")
                        .def(py::init<QCDate &, QCDate &, QCDate &, double, double, bool, double, double, vector<
                                double>>())
                        .def("amount", &qf::IcpClfCashflow::amount)
                        .def("settlement_currency", &qf::IcpClfCashflow::settlementCurrency)
                        .def("settlement_amount", &qf::IcpClfCashflow::settlementAmount)
                        .def("ccy", &qf::IcpClfCashflow::ccy)
                        .def("accrued_interest", &qf::IcpClfCashflow::accruedInterest)
                        .def("get_type", &qf::IcpClfCashflow::getType)
                        .def("get_tra", &qf::IcpClfCashflow::getTra)
                        .def("get_rate_value", &qf::IcpClfCashflow::getRateValue)
                        .def("set_start_date_uf", &qf::IcpClfCashflow::setStartDateUf)
                        .def("get_start_date_uf", &qf::IcpClfCashflow::getStartDateUf)
                        .def("set_end_date_uf", &qf::IcpClfCashflow::setEndDateUf)
                        .def("get_end_date_uf", &qf::IcpClfCashflow::getEndDateUf)
                        .def("set_tra_decimal_places", &qf::IcpClfCashflow::setTraDecimalPlaces)
                        .def("get_amount_icp_derivatives", &qf::IcpClfCashflow::getAmountICPDerivatives)
                        .def("get_amount_ufclp_derivatives", &qf::IcpClfCashflow::getAmountUFCLPDerivatives)
                        .def("get_amount_ufclf_derivatives", &qf::IcpClfCashflow::getAmountUFCLFDerivatives);

        // CompoundedOvernightRateCashflow
        py::class_<qf::CompoundedOvernightRateCashflow, std::shared_ptr<qf::CompoundedOvernightRateCashflow>,
                                qf::LinearInterestRateCashflow>(
                                m, "CompoundedOvernightRateCashflow")
                        .def(py::init<std::shared_ptr<qf::InterestRateIndex>,
                                const QCDate &,
                                const QCDate &,
                                const QCDate &,
                                const std::vector<QCDate> &,
                                double,
                                double,
                                bool,
                                shared_ptr<QCCurrency>,
                                double,
                                double,
                                bool,
                                unsigned int,
                                unsigned int,
                                unsigned int>())
                        .def("amount", &qf::CompoundedOvernightRateCashflow::amount)
                        .def("ccy", &qf::CompoundedOvernightRateCashflow::ccy)
                        .def("date", &qf::CompoundedOvernightRateCashflow::date)
                        .def("get_type", &qf::CompoundedOvernightRateCashflow::getType)
                        .def("get_spread", &qf::CompoundedOvernightRateCashflow::getSpread)
                        .def("get_gearing", &qf::CompoundedOvernightRateCashflow::getGearing)
                        .def("get_nominal", &qf::CompoundedOvernightRateCashflow::getNominal)
                        .def("set_nominal", &qf::CompoundedOvernightRateCashflow::setNominal)
                        .def("set_amortization", &qf::CompoundedOvernightRateCashflow::setAmortization)
                        .def("is_expired", &qf::CompoundedOvernightRateCashflow::isExpired)
                        .def("get_eq_rate_decimal_places", &qf::CompoundedOvernightRateCashflow::getEqRateDecimalPlaces)
                        .def("get_amount_derivatives", &qf::CompoundedOvernightRateCashflow::getAmountDerivatives)
                        .def("get_interest_rate_index_code",
                             &qf::CompoundedOvernightRateCashflow::getInterestRateIndexCode);

        m.def("show", py::overload_cast<const std::shared_ptr<qf::CompoundedOvernightRateCashflow> &>(&show));

        // CompoundedOvernightRateCashflow2
        py::class_<qf::CompoundedOvernightRateCashflow2, std::shared_ptr<qf::CompoundedOvernightRateCashflow2>,
                                qf::Cashflow>(
                                m, "CompoundedOvernightRateCashflow2")
                        .def(py::init<std::shared_ptr<qf::InterestRateIndex>,
                                const QCDate &,
                                const QCDate &,
                                const QCDate &,
                                const std::vector<QCDate> &,
                                double,
                                double,
                                bool,
                                shared_ptr<QCCurrency>,
                                double,
                                double,
                                const QCInterestRate &,
                                unsigned int,
                                unsigned int,
                                unsigned int>())
                        .def("amount", &qf::CompoundedOvernightRateCashflow2::amount)
                        .def("settlement_amount", &qf::CompoundedOvernightRateCashflow2::settlementAmount)
                        .def("settlement_currency", &qf::CompoundedOvernightRateCashflow2::settlementCurrency)
                        .def("ccy", &qf::CompoundedOvernightRateCashflow2::ccy)
                        .def("date", &qf::CompoundedOvernightRateCashflow2::date)
                        .def("get_type", &qf::CompoundedOvernightRateCashflow2::getType)
                        .def("get_start_date", &qf::CompoundedOvernightRateCashflow2::getStartDate)
                        .def("get_end_date", &qf::CompoundedOvernightRateCashflow2::getEndDate)
                        .def("get_settlement_date", &qf::CompoundedOvernightRateCashflow2::getSettlementDate)
                        .def("get_fixing_dates", &qf::CompoundedOvernightRateCashflow2::getFixingDates)
                        .def("get_interest_rate_index_code",
                             &qf::CompoundedOvernightRateCashflow2::getInterestRateIndexCode)
                        .def("get_nominal", &qf::CompoundedOvernightRateCashflow2::getNotional)
                        .def("get_amortization", &qf::CompoundedOvernightRateCashflow2::getAmortization)
                        .def("interest_from_spread", &qf::CompoundedOvernightRateCashflow2::interestFromSpread)
                        .def("interest", &qf::CompoundedOvernightRateCashflow2::interest)
                        .def("fixing", &qf::CompoundedOvernightRateCashflow2::fixing)
                        .def("accrued_interest", &qf::CompoundedOvernightRateCashflow2::accruedInterest)
                        .def("accrued_fixing", &qf::CompoundedOvernightRateCashflow2::accruedFixing)
                        .def("does_amortize", &qf::CompoundedOvernightRateCashflow2::doesAmortize)
                        .def("get_interest_rate_index", &qf::CompoundedOvernightRateCashflow2::getInterestRateIndex)
                        .def("get_interest_rate_index_code",
                             &qf::CompoundedOvernightRateCashflow2::getInterestRateIndexCode)
                        .def("get_spread", &qf::CompoundedOvernightRateCashflow2::getSpread)
                        .def("get_gearing", &qf::CompoundedOvernightRateCashflow2::getGearing)
                        .def("set_notional", &qf::CompoundedOvernightRateCashflow2::setNotional)
                        .def("set_fixings", &qf::CompoundedOvernightRateCashflow2::setFixings)
                        .def("get_fixings", &qf::CompoundedOvernightRateCashflow2::getFixings)
                        .def("settlement_amount", &qf::CompoundedOvernightRateCashflow2::settlementAmount)
                        .def("set_amortization", &qf::CompoundedOvernightRateCashflow2::setAmortization)
                        .def("is_expired", &qf::CompoundedOvernightRateCashflow2::isExpired)
                        .def("get_eq_rate_decimal_places",
                             &qf::CompoundedOvernightRateCashflow2::getEqRateDecimalPlaces)
                        .def("get_amount_derivatives", &qf::CompoundedOvernightRateCashflow2::getAmountDerivatives);

        m.def("show", py::overload_cast<const std::shared_ptr<qf::CompoundedOvernightRateCashflow2> &>(&show));

        // CompoundedOvernightRateMultiCurrencyCashflow2
        py::class_<qf::CompoundedOvernightRateMultiCurrencyCashflow2, std::shared_ptr<
                                        qf::CompoundedOvernightRateMultiCurrencyCashflow2>,
                                qf::CompoundedOvernightRateCashflow2>(
                                m, "CompoundedOvernightRateMultiCurrencyCashflow2")
                        .def(py::init<std::shared_ptr<qf::InterestRateIndex>,
                                const QCDate &,
                                const QCDate &,
                                const QCDate &,
                                const std::vector<QCDate> &,
                                double,
                                double,
                                bool,
                                shared_ptr<QCCurrency>,
                                double,
                                double,
                                const QCInterestRate &,
                                unsigned int,
                                unsigned int,
                                unsigned int,
                                const QCDate &,
                                std::shared_ptr<QCCurrency>,
                                std::shared_ptr<qf::FXRateIndex>>())
                        .def("settlement_currency",
                             &qf::CompoundedOvernightRateMultiCurrencyCashflow2::settlementCurrency)
                        .def("settlement_amount", &qf::CompoundedOvernightRateMultiCurrencyCashflow2::settlementAmount)
                        .def<void(qf::CompoundedOvernightRateMultiCurrencyCashflow2::*)(const qf::TimeSeries&)>(
                                "set_fx_rate_index_value",
                                &qf::CompoundedOvernightRateMultiCurrencyCashflow2::setFxRateIndexValue)
                        .def<void(qf::CompoundedOvernightRateMultiCurrencyCashflow2::*)(double)>(
                                "set_fx_rate_index_value",
                                &qf::CompoundedOvernightRateMultiCurrencyCashflow2::setFxRateIndexValue)
                        .def("get_fx_rate_index_code",
                             &qf::CompoundedOvernightRateMultiCurrencyCashflow2::getFXRateIndexCode)
                        .def("get_fx_rate_index_value",
                             &qf::CompoundedOvernightRateMultiCurrencyCashflow2::getFXRateIndexValue)
                        .def("get_fx_rate_index_fixing_date",
                             &qf::CompoundedOvernightRateMultiCurrencyCashflow2::getFXRateIndexFixingDate)
                        .def("get_fx_rate_index", &qf::CompoundedOvernightRateMultiCurrencyCashflow2::getFXRateIndex)
                        .def<double(qf::CompoundedOvernightRateMultiCurrencyCashflow2::*)(double)>(
                                "to_settlement_currency",
                                &qf::CompoundedOvernightRateMultiCurrencyCashflow2::toSettlementCurrency)
                        .def<double(
                                qf::CompoundedOvernightRateMultiCurrencyCashflow2::*)(double, const qf::TimeSeries&)>(
                                "to_settlement_currency",
                                &qf::CompoundedOvernightRateMultiCurrencyCashflow2::toSettlementCurrency);


        // Leg
        py::class_<qf::Leg>(m, "Leg")
                        .def(py::init<>())
                        .def("set_cashflow_at", &qf::Leg::setCashflowAt)
                        .def("get_cashflow_at", &qf::Leg::getCashflowAt)
                        .def("append_cashflow", &qf::Leg::appendCashflow)
                        .def("size", &qf::Leg::size)
                        .def("resize", &qf::Leg::resize)
                        .def("get_cashflows", &qf::Leg::getCashflows);

        // BusyAdjRules
        py::enum_<QCDate::QCBusDayAdjRules>(m, "BusyAdjRules")
                        .value("NO", QCDate::qcNo)
                        .value("FOLLOW", QCDate::qcFollow)
                        .value("MODFOLLOW", QCDate::qcModFollow)
                        .value("PREVIOUS", QCDate::qcPrev)
                        .value("MODPREVIOUS", QCDate::qcModPrev);

        // StubPeriod
        py::enum_<QCInterestRateLeg::QCStubPeriod>(m, "StubPeriod")
                        .value("NO", QCInterestRateLeg::qcNoStubPeriod)
                        .value("SHORTBACK", QCInterestRateLeg::qcShortBack)
                        .value("SHORTFRONT", QCInterestRateLeg::qcShortFront)
                        .value("LONGBACK", QCInterestRateLeg::qcLongBack)
                        .value("LONGFRONT", QCInterestRateLeg::qcLongFront)
                        .value("LONGFRONT2", QCInterestRateLeg::qcLongFront2)
                        .value("LONGFRONT3", QCInterestRateLeg::qcLongFront3)
                        .value("LONGFRONT4", QCInterestRateLeg::qcLongFront4)
                        .value("LONGFRONT5", QCInterestRateLeg::qcLongFront5)
                        .value("LONGFRONT6", QCInterestRateLeg::qcLongFront6)
                        .value("LONGFRONT7", QCInterestRateLeg::qcLongFront7)
                        .value("LONGFRONT8", QCInterestRateLeg::qcLongFront8)
                        .value("LONGFRONT9", QCInterestRateLeg::qcLongFront9)
                        .value("LONGFRONT10", QCInterestRateLeg::qcLongFront10)
                        .value("LONGFRONT11", QCInterestRateLeg::qcLongFront11)
                        .value("LONGFRONT12", QCInterestRateLeg::qcLongFront12)
                        .value("LONGFRONT13", QCInterestRateLeg::qcLongFront13)
                        .value("LONGFRONT14", QCInterestRateLeg::qcLongFront14);

        // CustomNotionalAmort
        py::class_<qf::CustomNotionalAmort>(m, "CustomNotionalAmort")
                        .def(py::init<>())
                        .def(py::init<size_t>())
                        .def("set_size", &qf::CustomNotionalAmort::setSize)
                        .def("get_size", &qf::CustomNotionalAmort::getSize)
                        .def("set_notional_amort_at", &qf::CustomNotionalAmort::setNotionalAmortAt)
                        .def("pushback_notional_amort", &qf::CustomNotionalAmort::pushbackNotionalAmort)
                        .def("get_notional_at", &qf::CustomNotionalAmort::getNotionalAt)
                        .def("get_amort_at", &qf::CustomNotionalAmort::getAmortAt);

        // LegFactory
        py::class_<qf::LegFactory>(m, "LegFactory")
                        .def_static(
                                "build_bullet_fixed_rate_leg",
                                &qf::LegFactory::buildBulletFixedRateLeg,
                                "Builds a Leg containing only cashflows of type FixedRateCashflow. Amortization is BULLET",
                                py::arg("rec_pay"),
                                py::arg("start_date"),
                                py::arg("end_date"),
                                py::arg("bus_adj_rule"),
                                py::arg("settlement_periodicity"),
                                py::arg("settlement_stub_period"),
                                py::arg("settlement_calendar"),
                                py::arg("settlement_lag"),
                                py::arg("initial_notional"),
                                py::arg("amort_is_cashflow"),
                                py::arg("interest_rate"),
                                py::arg("notional_currency"),
                                py::arg("is_bond"))
                        .def_static(
                                "build_custom_amort_fixed_rate_leg",
                                &qf::LegFactory::buildCustomAmortFixedRateLeg,
                                "Builds a Leg containing only cashflows of type FixedRateCashflow. Amortization is CUSTOM",
                                py::arg("rec_pay"),
                                py::arg("start_date"),
                                py::arg("end_date"),
                                py::arg("bus_adj_rule"),
                                py::arg("settlement_periodicity"),
                                py::arg("settlement_stub_period"),
                                py::arg("settlement_calendar"),
                                py::arg("settlement_lag"),
                                py::arg("notional_and_amort"),
                                py::arg("amort_is_cashflow"),
                                py::arg("interest_rate"),
                                py::arg("notional_currency"))
                        .def_static(
                                "build_bullet_fixed_rate_mccy_leg",
                                &qf::LegFactory::buildBulletFixedRateMultiCurrencyLeg,
                                "Builds a Leg containing only cashflows of type FixedRateMultiCurrencyCashflow. Amortization is BULLET.",
                                py::arg("rec_pay"),
                                py::arg("start_date"),
                                py::arg("end_date"),
                                py::arg("bus_adj_rule"),
                                py::arg("settlement_periodicity"),
                                py::arg("settlement_stub_period"),
                                py::arg("settlement_calendar"),
                                py::arg("settlement_lag"),
                                py::arg("initial_notional"),
                                py::arg("amort_is_cashflow"),
                                py::arg("interest_rate"),
                                py::arg("notional_currency"),
                                py::arg("settlement_currency"),
                                py::arg("fx_rate_index"),
                                py::arg("fx_rate_index_fixing_lag"),
                                py::arg("is_bond"))
                        .def_static(
                                "build_custom_amort_fixed_rate_mccy_leg",
                                &qf::LegFactory::buildCustomAmortFixedRateMultiCurrencyLeg,
                                "Builds a Leg containing only cashflows of type FixedRateMultiCurrencyCashflow. Amortization is CUSTOM.",
                                py::arg("rec_pay"),
                                py::arg("start_date"),
                                py::arg("end_date"),
                                py::arg("bus_adj_rule"),
                                py::arg("settlement_periodicity"),
                                py::arg("settlement_stub_period"),
                                py::arg("settlement_calendar"),
                                py::arg("settlement_lag"),
                                py::arg("notional_and_amort"),
                                py::arg("amort_is_cashflow"),
                                py::arg("interest_rate"),
                                py::arg("notional_currency"),
                                py::arg("settlement_currency"),
                                py::arg("fx_rate_index"),
                                py::arg("fx_rate_index_fixing_lag"),
                                py::arg("is_bond"))
                        .def_static(
                                "build_bullet_ibor_leg",
                                &qf::LegFactory::buildBulletIborLeg,
                                "Builds a Leg containing only cashflows of type IborCashflow. Amortization is BULLET.",
                                py::arg("rec_pay"),
                                py::arg("start_date"),
                                py::arg("end_date"),
                                py::arg("bus_adj_rule"),
                                py::arg("settlement_periodicity"),
                                py::arg("stub_period"),
                                py::arg("settlement_calendar"),
                                py::arg("settlement_lag"),
                                py::arg("fixing_periodicity"),
                                py::arg("fixing_stub_period"),
                                py::arg("fixing_calendar"),
                                py::arg("fixing_lag"),
                                py::arg("interest_rate_index"),
                                py::arg("initial_notional"),
                                py::arg("amort_is_cashflow"),
                                py::arg("notional_currency"),
                                py::arg("spread"),
                                py::arg("gearing"))
                        .def_static(
                                "build_custom_amort_ibor_leg",
                                &qf::LegFactory::buildCustomAmortIborLeg,
                                "Builds a Leg containing only cashflows of type IborCashflow. Amortization is CUSTOM.",
                                py::arg("rec_pay"),
                                py::arg("start_date"),
                                py::arg("end_date"),
                                py::arg("bus_adj_rule"),
                                py::arg("settlement_periodicity"),
                                py::arg("stub_period"),
                                py::arg("settlement_calendar"),
                                py::arg("settlement_lag"),
                                py::arg("notional_and_amort"),
                                py::arg("fixing_periodicity"),
                                py::arg("fixing_stub_period"),
                                py::arg("fixing_calendar"),
                                py::arg("fixing_lag"),
                                py::arg("interest_rate_index"),
                                py::arg("amort_is_cashflow"),
                                py::arg("notional_currency"),
                                py::arg("spread"),
                                py::arg("gearing"))
                        .def_static(
                                "build_bullet_ibor_mccy_leg",
                                &qf::LegFactory::buildBulletIborMultiCurrencyLeg,
                                "Builds a Leg containing only cashflows of type IborMultiCurrencyCashflow. Amortization is BULLET.",
                                py::arg("rec_pay"),
                                py::arg("start_date"),
                                py::arg("end_date"),
                                py::arg("bus_adj_rule"),
                                py::arg("settlement_periodicity"),
                                py::arg("stub_period"),
                                py::arg("settlement_calendar"),
                                py::arg("settlement_lag"),
                                py::arg("fixing_periodicity"),
                                py::arg("fixing_stub_period"),
                                py::arg("fixing_calendar"),
                                py::arg("fixing_lag"),
                                py::arg("interest_rate_index"),
                                py::arg("initial_notional"),
                                py::arg("amort_is_cashflow"),
                                py::arg("notional_currency"),
                                py::arg("spread"),
                                py::arg("gearing"),
                                py::arg("settlement_currency"),
                                py::arg("fx_rate_index"),
                                py::arg("fx_rate_index_fixing_lag"))
                        .def_static(
                                "build_custom_amort_ibor_mccy_leg",
                                &qf::LegFactory::buildCustomAmortIborMultiCurrencyLeg,
                                "Builds a Leg containing only cashflows of type IborMultiCurrencyCashflow. Amortization is CUSTOM.",
                                py::arg("rec_pay"),
                                py::arg("start_date"),
                                py::arg("end_date"),
                                py::arg("bus_adj_rule"),
                                py::arg("settlement_periodicity"),
                                py::arg("stub_period"),
                                py::arg("settlement_calendar"),
                                py::arg("settlement_lag"),
                                py::arg("fixing_periodicity"),
                                py::arg("fixing_stub_period"),
                                py::arg("fixing_calendar"),
                                py::arg("fixing_lag"),
                                py::arg("interest_rate_index"),
                                py::arg("notional_and_amort"),
                                py::arg("amort_is_cashflow"),
                                py::arg("notional_currency"),
                                py::arg("spread"),
                                py::arg("gearing"),
                                py::arg("settlement_currency"),
                                py::arg("fx_rate_index"),
                                py::arg("fx_rate_index_fixing_lag"))
                        .def_static(
                                "build_bullet_overnight_index_leg",
                                &qf::LegFactory::buildBulletOvernightIndexLeg,
                                "Builds a Leg containing only cashflows of type OvernightIndexCashflow. Amortization is BULLET.",
                                py::arg("rec_pay"),
                                py::arg("start_date"),
                                py::arg("end_date"),
                                py::arg("bus_adj_rule"),
                                py::arg("fix_adj_rule"),
                                py::arg("settlement_periodicity"),
                                py::arg("stub_period"),
                                py::arg("settlement_calendar"),
                                py::arg("fixing_calendar"),
                                py::arg("settlement_lag"),
                                py::arg("initial_notional"),
                                py::arg("amort_is_cashflow"),
                                py::arg("spread"),
                                py::arg("gearing"),
                                py::arg("interest_rate"),
                                py::arg("index_name"),
                                py::arg("eq_rate_decimal_places"),
                                py::arg("notional_currency")
                        )
                        .def_static(
                                "build_bullet_overnight_index_multi_currency_leg",
                                &qf::LegFactory::buildBulletOvernightIndexMultiCurrencyLeg,
                                "Builds a Leg containing only cashflows of type OvernightIndexMultiCurrencyCashflow. Amortization is BULLET.",
                                py::arg("rec_pay"),
                                py::arg("start_date"),
                                py::arg("end_date"),
                                py::arg("bus_adj_rule"),
                                py::arg("fix_adj_rule"),
                                py::arg("settlement_periodicity"),
                                py::arg("stub_period"),
                                py::arg("settlement_calendar"),
                                py::arg("fixing_calendar"),
                                py::arg("settlement_lag"),
                                py::arg("initial_notional"),
                                py::arg("amort_is_cashflow"),
                                py::arg("spread"),
                                py::arg("gearing"),
                                py::arg("interest_rate"),
                                py::arg("index_name"),
                                py::arg("eq_rate_decimal_places"),
                                py::arg("notional_currency"),
                                py::arg("settlement_currency"),
                                py::arg("fx_rate_index"),
                                py::arg("fx_rate_index_fixing_lag")
                        )
                        .def_static(
                                "build_custom_amort_overnight_index_leg",
                                &qf::LegFactory::buildCustomAmortOvernightIndexLeg,
                                "Builds a Leg containing only cashflows of type OvernightIndexCashflow. Amortization is CUSTOM.",
                                py::arg("rec_pay"),
                                py::arg("start_date"),
                                py::arg("end_date"),
                                py::arg("bus_adj_rule"),
                                py::arg("fix_adj_rule"),
                                py::arg("settlement_periodicity"),
                                py::arg("stub_period"),
                                py::arg("settlement_calendar"),
                                py::arg("fixing_calendar"),
                                py::arg("settlement_lag"),
                                py::arg("notional_and_amort"),
                                py::arg("amort_is_cashflow"),
                                py::arg("spread"),
                                py::arg("gearing"),
                                py::arg("interest_rate"),
                                py::arg("index_name"),
                                py::arg("eq_rate_decimal_places"),
                                py::arg("notional_currency")
                        )
                        .def_static(
                                "build_custom_amort_overnight_index_multi_currency_leg",
                                &qf::LegFactory::buildCustomAmortOvernightIndexMultiCurrencyLeg,
                                "Builds a Leg containing only cashflows of type OvernightIndexMultiCurrencyCashflow. Amortization is CUSTOM.",
                                py::arg("rec_pay"),
                                py::arg("start_date"),
                                py::arg("end_date"),
                                py::arg("bus_adj_rule"),
                                py::arg("fix_adj_rule"),
                                py::arg("settlement_periodicity"),
                                py::arg("stub_period"),
                                py::arg("settlement_calendar"),
                                py::arg("fixing_calendar"),
                                py::arg("settlement_lag"),
                                py::arg("notional_and_amort"),
                                py::arg("amort_is_cashflow"),
                                py::arg("spread"),
                                py::arg("gearing"),
                                py::arg("interest_rate"),
                                py::arg("index_name"),
                                py::arg("eq_rate_decimal_places"),
                                py::arg("notional_currency"),
                                py::arg("settlement_currency"),
                                py::arg("fx_rate_index"),
                                py::arg("fx_rate_index_fixing_lag")
                        )
                        .def_static(
                                "build_bullet_icp_clp_leg",
                                &qf::LegFactory::buildBulletIcpClpLeg,
                                "Builds a Leg containing only cashflows of type IcpClpCashflow. Amortization is BULLET.",
                                py::arg("rec_pay"),
                                py::arg("start_date"),
                                py::arg("end_date"),
                                py::arg("bus_adj_rule"),
                                py::arg("settlement_periodicity"),
                                py::arg("stub_period"),
                                py::arg("settlement_calendar"),
                                py::arg("settlement_lag"),
                                py::arg("initial_notional"),
                                py::arg("amort_is_cashflow"),
                                py::arg("spread"),
                                py::arg("gearing")
                        )
                        .def_static(
                                "build_custom_amort_icp_clp_leg",
                                &qf::LegFactory::buildCustomAmortIcpClpLeg,
                                "Builds a Leg containing only cashflows of type IcpClpCashflow. Amortization is CUSTOM.",
                                py::arg("rec_pay"),
                                py::arg("start_date"),
                                py::arg("end_date"),
                                py::arg("bus_adj_rule"),
                                py::arg("settlement_periodicity"),
                                py::arg("stub_period"),
                                py::arg("settlement_calendar"),
                                py::arg("settlement_lag"),
                                py::arg("notional_and_amort"),
                                py::arg("amort_is_cashflow"),
                                py::arg("spread"),
                                py::arg("gearing")
                        )
                        .def_static(
                                "build_bullet_icp_clf_leg",
                                &qf::LegFactory::buildBulletIcpClfLeg,
                                "Builds a Leg containing only cashflows of type IcpClFCashflow. Amortization is BULLET.",
                                py::arg("rec_pay"),
                                py::arg("start_date"),
                                py::arg("end_date"),
                                py::arg("bus_adj_rule"),
                                py::arg("settlement_periodicity"),
                                py::arg("stub_period"),
                                py::arg("settlement_calendar"),
                                py::arg("settlement_lag"),
                                py::arg("initial_notional"),
                                py::arg("amort_is_cashflow"),
                                py::arg("spread"),
                                py::arg("gearing")
                        )
                        .def_static(
                                "build_custom_amort_icp_clf_leg",
                                &qf::LegFactory::buildCustomAmortIcpClfLeg,
                                "Builds a Leg containing only cashflows of type IcpClFCashflow. Amortization is CUSTOM.",
                                py::arg("rec_pay"),
                                py::arg("start_date"),
                                py::arg("end_date"),
                                py::arg("bus_adj_rule"),
                                py::arg("settlement_periodicity"),
                                py::arg("stub_period"),
                                py::arg("settlement_calendar"),
                                py::arg("settlement_lag"),
                                py::arg("notional_and_amort"),
                                py::arg("amort_is_cashflow"),
                                py::arg("spread"),
                                py::arg("gearing")
                        )
                        .def_static("build_bullet_fixed_rate_leg_2", &qf::LegFactory::buildBulletFixedRateLeg2)
                        .def_static("build_french_fixed_rate_leg_2", &qf::LegFactory::buildFrenchFixedRateLeg2)
                        .def_static("build_custom_amort_fixed_rate_leg_2",
                                    &qf::LegFactory::buildCustomAmortFixedRateLeg2)
                        .def_static("build_bullet_ibor2_leg", &qf::LegFactory::buildBulletIbor2Leg)
                        .def_static("build_custom_amort_ibor2_leg", &qf::LegFactory::buildCustomAmortIbor2Leg)
                        .def_static("build_bullet_icp_clp2_leg", &qf::LegFactory::buildBulletIcpClp2Leg)
                        .def_static("build_custom_amort_icp_clp2_leg", &qf::LegFactory::buildCustomAmortIcpClp2Leg)
                        .def_static("build_bullet_icp_clf_leg", &qf::LegFactory::buildBulletIcpClfLeg)
                        .def_static("build_custom_amort_icp_clf_leg", &qf::LegFactory::buildCustomAmortIcpClfLeg)
                        .def_static("customize_amortization", &qf::LegFactory::customizeAmortization)
                        .def_static("build_bullet_compounded_overnight_rate_leg",
                                    &qf::LegFactory::buildBulletCompoundedOvernightLeg)
                        .def_static(
                                "build_bullet_compounded_overnight_rate_leg_2",
                                &qf::LegFactory::buildBulletCompoundedOvernightRateLeg2,
                                "Builds a Leg containing only cashflows of type CompoundedOvernightRate2. Amortization is BULLET.",
                                py::arg("rec_pay"),
                                py::arg("start_date"),
                                py::arg("end_date"),
                                py::arg("bus_adj_rule"),
                                py::arg("settlement_periodicity"),
                                py::arg("settlement_stub_period"),
                                py::arg("settlement_calendar"),
                                py::arg("settlement_lag"),
                                py::arg("fixing_calendar"),
                                py::arg("interest_rate_index"),
                                py::arg("initial_notional"),
                                py::arg("cashflow_is_amort"),
                                py::arg("notional_currency"),
                                py::arg("spread"),
                                py::arg("gearing"),
                                py::arg("interest_rate"),
                                py::arg("eq_rate_decimal_places"),
                                py::arg("lookback"),
                                py::arg("lockout"))
                        .def_static(
                                "build_bullet_compounded_overnight_rate_mccy_leg_2",
                                &qf::LegFactory::buildBulletCompoundedOvernightRateMultiCurrencyLeg2,
                                "Builds a Leg containing only cashflows of type CompoundedOvernightRateMultiCurrencyCashflow2. Amortization is BULLET.",
                                py::arg("rec_pay"),
                                py::arg("start_date"),
                                py::arg("end_date"),
                                py::arg("bus_adj_rule"),
                                py::arg("settlement_periodicity"),
                                py::arg("settlement_stub_period"),
                                py::arg("settlement_calendar"),
                                py::arg("settlement_lag"),
                                py::arg("fixing_calendar"),
                                py::arg("interest_rate_index"),
                                py::arg("initial_notional"),
                                py::arg("cashflow_is_amort"),
                                py::arg("notional_currency"),
                                py::arg("spread"),
                                py::arg("gearing"),
                                py::arg("interest_rate"),
                                py::arg("eq_rate_decimal_places"),
                                py::arg("lookback"),
                                py::arg("lockout"),
                                py::arg("fx_rate_index_fixing_lag"),
                                py::arg("settlement_currency"),
                                py::arg("fx_rate_index"))
                        .def_static("build_custom_amort_compounded_overnight_rate_leg",
                                    &qf::LegFactory::buildCustomAmortCompoundedOvernightLeg)
                        .def_static(
                                "build_custom_amort_compounded_overnight_rate_leg_2",
                                &qf::LegFactory::buildCustomAmortCompoundedOvernightRateLeg2,
                                "Builds a Leg containing only cashflows of type CompoundedOvernightRate2. Amortization is CUSTOM.",
                                py::arg("rec_pay"),
                                py::arg("start_date"),
                                py::arg("end_date"),
                                py::arg("bus_adj_rule"),
                                py::arg("settlement_periodicity"),
                                py::arg("settlement_stub_period"),
                                py::arg("settlement_calendar"),
                                py::arg("settlement_lag"),
                                py::arg("fixing_calendar"),
                                py::arg("interest_rate_index"),
                                py::arg("notional_and_amort"),
                                py::arg("cashflow_is_amort"),
                                py::arg("notional_currency"),
                                py::arg("spread"),
                                py::arg("gearing"),
                                py::arg("interest_rate"),
                                py::arg("eq_rate_decimal_places"),
                                py::arg("lookback"),
                                py::arg("lockout"))
                        .def_static(
                                "build_custom_amort_compounded_overnight_rate_multi_currency_leg_2",
                                &qf::LegFactory::buildCustomAmortCompoundedOvernightRateMultiCurrencyLeg2,
                                "Builds a Leg containing only cashflows of type CompoundedOvernightRateMultiCurrency2. Amortization is CUSTOM.",
                                py::arg("rec_pay"),
                                py::arg("start_date"),
                                py::arg("end_date"),
                                py::arg("bus_adj_rule"),
                                py::arg("settlement_periodicity"),
                                py::arg("settlement_stub_period"),
                                py::arg("settlement_calendar"),
                                py::arg("settlement_lag"),
                                py::arg("fixing_calendar"),
                                py::arg("interest_rate_index"),
                                py::arg("notional_and_amort"),
                                py::arg("cashflow_is_amort"),
                                py::arg("notional_currency"),
                                py::arg("spread"),
                                py::arg("gearing"),
                                py::arg("interest_rate"),
                                py::arg("eq_rate_decimal_places"),
                                py::arg("lookback"),
                                py::arg("lockout"),
                                py::arg("fx_rate_index_fixing_lag"),
                                py::arg("settlement_currency"),
                                py::arg("fx_rate_index"));

        // long_vec
        py::bind_vector<std::vector<long>>(m, "long_vec");

        // double_vec
        py::bind_vector<std::vector<double>>(m, "double_vec");

        // QCCurve
        py::class_<QCCurve<long>, std::shared_ptr<QCCurve<long>>>(m, "QCCurve")
                        .def(py::init<std::vector<long> &, std::vector<double> &>())
                        .def("reset", &QCCurve<long>::reset)
                        .def("set_pair", &QCCurve<long>::setPair)
                        .def("set_ordinate_at_with_value", &QCCurve<long>::setOrdinateAtWithValue)
                        .def("set_abscissa_at_with_value", &QCCurve<long>::setAbscissaAtWithValue)
                        .def("get_values_at", &QCCurve<long>::getValuesAt)
                        .def("get_length", &QCCurve<long>::getLength);

        py::class_<std::pair<long, double>>(m, "CurvePoint");

        // QCInterpolator
        py::class_<QCInterpolator, PyQCInterpolator, std::shared_ptr<QCInterpolator>>(m, "QCInterpolator")
                        .def(py::init<shared_ptr<QCCurve<long>>>())
                        .def("interpolate_at", &QCInterpolator::interpolateAt)
                        .def("derivative_at", &QCInterpolator::derivativeAt)
                        .def("second_derivative_at", &QCInterpolator::secondDerivativeAt);

        // QCLinearInterpolator
        py::class_<QCLinearInterpolator, std::shared_ptr<QCLinearInterpolator>, QCInterpolator>(
                                m, "QCLinearInterpolator")
                        .def(py::init<shared_ptr<QCCurve<long>>>())
                        .def("interpolate_at", &QCLinearInterpolator::interpolateAt)
                        .def("derivative_at", &QCLinearInterpolator::derivativeAt)
                        .def("second_derivative_at", &QCLinearInterpolator::secondDerivativeAt);

        // InterestRateCurve
        py::class_<qf::InterestRateCurve, std::shared_ptr<qf::InterestRateCurve>, PyInterestRateCurve>(
                                m, "InterestRateCurve")
                        .def(py::init<shared_ptr<QCInterpolator>, QCInterestRate>())
                        .def("get_rate_at", &qf::InterestRateCurve::getRateAt)
                        .def("get_qc_interest_rate_at", &qf::InterestRateCurve::getQCInterestRateAt)
                        .def("get_discount_factor_at", &qf::InterestRateCurve::getDiscountFactorAt)
                        .def("get_forward_rate_with_rate", &qf::InterestRateCurve::getForwardRateWithRate)
                        .def("get_forward_rate", &qf::InterestRateCurve::getForwardRate)
                        .def("get_forward_wf", &qf::InterestRateCurve::getForwardWf)
                        .def("df_derivative_at", &qf::InterestRateCurve::dfDerivativeAt)
                        .def("wf_derivative_at", &qf::InterestRateCurve::wfDerivativeAt)
                        .def("fwd_wf_derivative_at", &qf::InterestRateCurve::fwdWfDerivativeAt);

        // ZeroCouponCurve
        py::class_<qf::ZeroCouponCurve, std::shared_ptr<qf::ZeroCouponCurve>, qf::InterestRateCurve>(
                                m, "ZeroCouponCurve")
                        .def(py::init<shared_ptr<QCInterpolator>, QCInterestRate>());

        // PresentValue
        py::class_<qf::PresentValue>(m, "PresentValue")
                        .def(py::init<>())
                        .def("pv", py::overload_cast<QCDate &, const std::shared_ptr<qf::Cashflow> &, QCInterestRate &>(
                                     &qf::PresentValue::pv))
                        .def("pv", py::overload_cast<QCDate &, const std::shared_ptr<qf::Cashflow> &, const
                                     std::shared_ptr<qf::InterestRateCurve> &>(&qf::PresentValue::pv))
                        .def("pv", py::overload_cast<QCDate &, const std::shared_ptr<qf::LinearInterestRateCashflow> &,
                                     const std::shared_ptr<qf::InterestRateCurve> &>(&qf::PresentValue::pv))
                        .def("pv", py::overload_cast<QCDate &, qf::Leg &, QCInterestRate &>(&qf::PresentValue::pv))
                        .def("pv", py::overload_cast<QCDate &, qf::Leg &, const std::shared_ptr<qf::InterestRateCurve>
                                     &>(&qf::PresentValue::pv))
                        .def("get_derivative", &qf::PresentValue::getDerivative)
                        .def("get_derivatives", &qf::PresentValue::getDerivatives)
                        .def("get_rate", &qf::PresentValue::getRate);

        // FixedRateBond
        py::class_<qf::FixedRateBond, std::shared_ptr<qf::FixedRateBond>>(m, "FixedRateBond")
                        .def(py::init<qf::Leg &>())
                        .def("present_value", &qf::FixedRateBond::presentValue)
                        .def("price", &qf::FixedRateBond::price)
                        .def("accrued_interest", &qf::FixedRateBond::accruedInterest)
                        .def("duracion", &qf::FixedRateBond::duration)
                        .def("convexidad", &qf::FixedRateBond::convexity)
                        .def("get_leg", &qf::FixedRateBond::getLeg);

        // ChileanFixedRateBond
        py::class_<qf::ChileanFixedRateBond, std::shared_ptr<qf::ChileanFixedRateBond>, qf::FixedRateBond>(
                                m, "ChileanFixedRateBond")
                        .def(py::init<qf::Leg &, const QCInterestRate &>())
                        .def("valor_par", &qf::ChileanFixedRateBond::valorPar)
                        .def("precio", &qf::ChileanFixedRateBond::price)
                        .def("valor_pago", &qf::ChileanFixedRateBond::settlementValue)
                        .def("precio2", &qf::ChileanFixedRateBond::price2)
                        .def("valor_pago2", &qf::ChileanFixedRateBond::settlementValue2);

        // get_column_names
        m.def("get_column_names", getColumnNames, "Muestra on objeto Leg como pandas.DataFrame",
              py::arg("type_of_cashflow"),
              py::arg("type_of_subcashflow")="");

        // ForwardRates
        py::class_<qf::ForwardRates>(m, "ForwardRates")
                        .def(py::init<>())
                        .def("set_rate_ibor_cashflow", &qf::ForwardRates::setRateIborCashflow)
                        .def("set_rate_icp_clp_cashflow", &qf::ForwardRates::setRateIcpClpCashflow)
                        .def("set_rate_icp_clp_cashflow2", &qf::ForwardRates::setRateIcpClpCashflow2)
                        .def("set_rate_overnight_index_cashflow", &qf::ForwardRates::setRateOvernightIndexCashflow)
                        .def("set_rates_icp_clp_leg", &qf::ForwardRates::setRatesIcpClpLeg)
                        .def("set_rates_icp_clp_leg2", &qf::ForwardRates::setRatesIcpClpLeg2)
                        .def("set_rates_overnight_index_leg", &qf::ForwardRates::setRatesOvernightIndexLeg)
                        .def("set_rates_ibor_leg", &qf::ForwardRates::setRatesIborLeg)
                        .def("set_rate_icp_clf_cashflow", &qf::ForwardRates::setRateIcpClfCashflow)
                        .def("set_rates_icp_clf_leg", &qf::ForwardRates::setRatesIcpClfLeg)
                        .def("set_rate_compounded_overnight_cashflow",
                             &qf::ForwardRates::setRateCompoundedOvernightCashflow)
                        .def("set_rates_compounded_overnight_leg", &qf::ForwardRates::setRatesCompoundedOvernightLeg);


#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
        m.attr("__version__") = "dev";
#endif
}
