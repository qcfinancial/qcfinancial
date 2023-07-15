//
// Created by Alvaro Diaz on 18-06-23.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

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

#include <time/QCDate.h>
#include <time/QCBusinessCalendar.h>

#include <QcfinancialPybind11Helpers.h>

namespace qf = QCode::Financial;

// PYBIND11_MAKE_OPAQUE(std::vector<QCDate>);

PYBIND11_MODULE(qcfinancial, m)
{
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

    m.def("subtract", [](int i, int j) { return i - j; }, R"pbdoc(
        Subtract two numbers

        Some other explanation about the subtract function.
    )pbdoc");

    // QCDate
    py::class_<QCDate>
    pyQCDate(m, "QCDate", R"pbdoc(
        Permite representar una fecha en calendario gregoriano.
    )pbdoc");

    pyQCDate.def(py::init<int, int, int>(), R"pbdoc(
- d: int, es el día de la fecha.
- m: int, es el mes de la fecha.
- y: int, es el año de la fecha.)pbdoc")
            .def(py::init<>())
            .def(py::init<long>())
            .def(py::init<std::string&>())
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
            .def("__str__", &QCDate::description, py::arg("es_iso") = false);

    m.def("build_qcdate_from_string", [](std::string& strDate){
        return QCDate{strDate};
    });

    py::bind_vector<std::vector<QCDate>>(m, "DateList");
    py::bind_vector<std::vector<double>>(m, "double_vec");

    py::enum_<QCDate::QCWeekDay>(pyQCDate, "WeekDay")
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

    py::class_<QCPEN, std::shared_ptr<QCPEN>, QCCurrency>(m, "QCCPEN")
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
    py::class_<QCYearFraction, shared_ptr<QCYearFraction>>(m, "QCYearFraction", "Clase madre de todas las fracciones de año.");

    py::class_<QCAct360, shared_ptr<QCAct360>, QCYearFraction>(m, "QCAct360", "Representa la fracción de año Act360.")
            .def(py::init<>())
            .def("yf", py::overload_cast<const QCDate&, const QCDate&>(&QCAct360::yf),
                    R"pbdoc(Calcula la fracción de año entre dos fechas:)pbdoc")
            .def("yf", py::overload_cast<long>(&QCAct360::yf))
            .def("count_days", &QCAct360::countDays)
            .def("__str__", &QCAct360::description);

    py::class_<QCAct365, shared_ptr<QCAct365>, QCYearFraction>(m, "QCAct365")
            .def(py::init<>())
            .def("yf", py::overload_cast<const QCDate&, const QCDate&>(&QCAct365::yf))
            .def("yf", py::overload_cast<long>(&QCAct365::yf))
            .def("count_days", &QCAct365::countDays)
            .def("__str__", &QCAct365::description);

    py::class_<QCAct30, shared_ptr<QCAct30>, QCYearFraction>(m, "QCAct30")
            .def(py::init<>())
            .def("yf", py::overload_cast<const QCDate&, const QCDate&>(&QCAct30::yf))
            .def("yf", py::overload_cast<long>(&QCAct30::yf))
            .def("count_days", &QCAct30::countDays)
            .def("__str__", &QCAct30::description);

    py::class_<QCActAct, shared_ptr<QCActAct>, QCYearFraction>(m, "QCActAct")
            .def(py::init<>())
            .def("yf", py::overload_cast<const QCDate&, const QCDate&>(&QCActAct::yf))
            .def("yf", py::overload_cast<long>(&QCActAct::yf))
            .def("count_days", &QCActAct::countDays)
            .def("__str__", &QCActAct::description);

    py::class_<QC30360, shared_ptr<QC30360>, QCYearFraction>(m, "QC30360")
            .def(py::init<>())
            .def("yf", py::overload_cast<const QCDate&, const QCDate&>(&QC30360::yf))
            .def("yf", py::overload_cast<long>(&QC30360::yf))
            .def("count_days", &QC30360::countDays)
            .def("__str__", &QC30360::description);

    py::class_<QC3030, shared_ptr<QC3030>, QCYearFraction>(m, "QC3030")
            .def(py::init<>())
            .def("yf", py::overload_cast<const QCDate&, const QCDate&>(&QC3030::yf))
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
            .def("wf", py::overload_cast<QCDate&, QCDate&>(&QCInterestRate::wf))
            .def("wf", py::overload_cast<long>(&QCInterestRate::wf))
            .def("dwf", py::overload_cast<QCDate&, QCDate&>(&QCInterestRate::dwf))
            .def("dwf", py::overload_cast<long>(&QCInterestRate::dwf))
            .def("d2wf", py::overload_cast<QCDate&, QCDate&>(&QCInterestRate::d2wf))
            .def("d2wf", py::overload_cast<long>(&QCInterestRate::d2wf))
            .def("get_rate_from_wf", py::overload_cast<double, QCDate&, QCDate&>(&QCInterestRate::getRateFromWf))
            .def("get_rate_from_wf", py::overload_cast<double, long>(&QCInterestRate::getRateFromWf))
            .def("yf", &QCInterestRate::yf)
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
            .def(py::init<std::shared_ptr<qf::FXRate>, std::string, qf::Tenor, qf::Tenor, QCBusinessCalendar>())
            .def("fixing_date", &qf::FXRateIndex::fixingDate)
            .def("value_date", &qf::FXRateIndex::valueDate)
            .def("convert", &qf::FXRateIndex::convert);

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
            .def(py::init<const QCDate&, double, std::shared_ptr<QCCurrency>>())
            .def("get_type", &qf::SimpleCashflow::getType);

    m.def("show", py::overload_cast<const std::shared_ptr<qf::SimpleCashflow>&>(&show));

    // TimeSeries
    py::bind_map<std::map<QCDate, double>>(m, "time_series");

    // SimpleMultiCurrencyCashflow
    py::class_<qf::SimpleMultiCurrencyCashflow, qf::SimpleCashflow, std::shared_ptr<qf::SimpleMultiCurrencyCashflow>>(
            m, "SimpleMultiCurrencyCashflow")
            .def(py::init<QCDate&, double, std::shared_ptr<QCCurrency>, QCDate&, std::shared_ptr<QCCurrency>,
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
            .def(py::init<QCDate &, QCDate &, QCDate &, double, double, bool, const QCInterestRate &, shared_ptr<QCCurrency>>())
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
            .def("get_type", &qf::FixedRateCashflow::getType);

    m.def("show", py::overload_cast<const std::shared_ptr<qf::FixedRateCashflow>&>(&show));

    // FixedRateMulticurrencyCashflow
    py::class_<qf::FixedRateMultiCurrencyCashflow, std::shared_ptr<qf::FixedRateMultiCurrencyCashflow>, qf::FixedRateCashflow>
            (m, "FixedRateMultiCurrencyCashflow")
            .def(py::init<
                    QCDate &, QCDate &, QCDate &,
                    double, double, bool,
                    const QCInterestRate &, shared_ptr<QCCurrency>, QCDate &,
                    shared_ptr<QCCurrency>, shared_ptr<qf::FXRateIndex>, double>())
                    .def("amount", &qf::FixedRateMultiCurrencyCashflow::amount)
                    .def("get_fx_publish_date", &qf::FixedRateMultiCurrencyCashflow::getFXPublishDate)
                    .def("settlement_currency", &qf::FixedRateMultiCurrencyCashflow::settlementCcy)
                    .def("set_fx_rate_index_value", &qf::FixedRateMultiCurrencyCashflow::setFxRateIndexValue)
                    .def("accrued_interest", &qf::FixedRateCashflow::accruedInterest)
                    .def("accrued_interest", &qf::FixedRateMultiCurrencyCashflow::accruedInterest)
                    .def("get_amortization", &qf::FixedRateCashflow::getAmortization)
                    .def("get_amortization", &qf::FixedRateMultiCurrencyCashflow::getAmortization)
                    .def("get_fx_rate_index", &qf::FixedRateMultiCurrencyCashflow::getFXRateIndex)
                    .def("get_fx_rate_index_code", &qf::FixedRateMultiCurrencyCashflow::getFXRateIndexCode)
                    .def("get_type", &qf::FixedRateMultiCurrencyCashflow::getType);

    // LinearInterestRateCashflow
    py::class_<qf::LinearInterestRateCashflow, PyLinearInterestRateCashflow, std::shared_ptr<qf::LinearInterestRateCashflow>>(
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
            .def("interest", py::overload_cast<const qf::TimeSeries &>(&qf::LinearInterestRateCashflow::interest))
            .def("fixing", py::overload_cast<>(&qf::LinearInterestRateCashflow::fixing))
            .def("fixing", py::overload_cast<const qf::TimeSeries &>(&qf::LinearInterestRateCashflow::fixing))
            .def("accrued_interest", py::overload_cast<const QCDate &>(&qf::LinearInterestRateCashflow::accruedInterest))
            .def("accrued_interest",py::overload_cast<const QCDate &, const qf::TimeSeries &>(&qf::LinearInterestRateCashflow::accruedInterest))
            .def("accrued_fixing", py::overload_cast<const QCDate &>(&qf::LinearInterestRateCashflow::accruedFixing))
            .def("accrued_fixing",py::overload_cast<const QCDate &, const qf::TimeSeries &>(&qf::LinearInterestRateCashflow::accruedFixing));

    // FixedRateCashflow2
    py::class_<qf::FixedRateCashflow2, qf::LinearInterestRateCashflow, std::shared_ptr<qf::FixedRateCashflow2>>(
            m, "FixedRateCashflow2")
            .def(py::init<QCDate &, QCDate &, QCDate &, double, double, bool, const QCInterestRate &, shared_ptr<QCCurrency>>())
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
            .def < double(qf::FixedRateCashflow2::*)() > ("interest", &qf::FixedRateCashflow2::interest)
            .def < double(qf::FixedRateCashflow2::*)(const qf::TimeSeries&)>("interest", &qf::FixedRateCashflow2::interest)
            .def < double(qf::FixedRateCashflow2::*)() > ("fixing", &qf::FixedRateCashflow2::fixing)
            .def < double(qf::FixedRateCashflow2::*)(const qf::TimeSeries&)>("fixing", &qf::FixedRateCashflow2::fixing)
            .def <double(qf::FixedRateCashflow2::*)(const QCDate&)>("accrued_interest", &qf::FixedRateCashflow2::accruedInterest)
            .def < double(qf::FixedRateCashflow2::*)(const QCDate&, const qf::TimeSeries&)>("accrued_interest", &qf::FixedRateCashflow2::accruedInterest)
            .def < double(qf::FixedRateCashflow2::*)(const QCDate&)>("accrued_fixing", &qf::FixedRateCashflow2::accruedFixing)
            .def <double(qf::FixedRateCashflow2::*)(const QCDate&, const qf::TimeSeries&)>("accrued_fixing", &qf::FixedRateCashflow2::accruedFixing)
            .def("does_amortize", &qf::FixedRateCashflow2::doesAmortize);

    m.def("show", py::overload_cast<const std::shared_ptr<qf::FixedRateCashflow2>&>(&show));

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
            .def("set_interest_rate_value", &qf::IborCashflow::setInterestRateValue)
            .def("get_interest_rate_value", &qf::IborCashflow::getInterestRateValue)
            .def("set_nominal", &qf::IborCashflow::setNominal)
            .def("get_nominal", &qf::IborCashflow::getNominal)
            .def("set_amortization", &qf::IborCashflow::setAmortization)
            .def("get_amortization", &qf::IborCashflow::getAmortization)
            .def("get_type", &qf::IborCashflow::getType);

    m.def("show", py::overload_cast<const std::shared_ptr<qf::IborCashflow>&>(&show));

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
            .def < double(qf::IborCashflow2::*)() > ("interest", &qf::IborCashflow2::interest)
            .def < double(qf::IborCashflow2::*)(const qf::TimeSeries&)>("interest", &qf::IborCashflow2::interest)
            .def < double(qf::IborCashflow2::*)() > ("fixing", &qf::IborCashflow2::fixing)
            .def < double(qf::IborCashflow2::*)(const qf::TimeSeries&)>("fixing", &qf::IborCashflow2::fixing)
            .def < double(qf::IborCashflow2::*)(const QCDate&)>("accrued_interest", &qf::IborCashflow2::accruedInterest)
            .def < double(qf::IborCashflow2::*)(const QCDate&, const qf::TimeSeries&)>("accrued_interest",&qf::IborCashflow2::accruedInterest)
            .def < double(qf::IborCashflow2::*)(const QCDate&)>("accrued_fixing",&qf::IborCashflow2::accruedFixing)
            .def < double(qf::IborCashflow2::*)(const QCDate&, const qf::TimeSeries&)>("accrued_fixing",&qf::IborCashflow2::accruedFixing)
            .def("does_amortize", &qf::IborCashflow2::doesAmortize);

    m.def("show", py::overload_cast<const std::shared_ptr<qf::IborCashflow2>&>(&show));

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
            .def("settlement_currency", &qf::IborMultiCurrencyCashflow::settlementCcy)
            .def("set_fx_rate_index_value", &qf::IborMultiCurrencyCashflow::setFxRateIndexValue)
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

    m.def("show", py::overload_cast<const std::shared_ptr<qf::IcpClpCashflow>&>(&show));

    // IcpClpCashflow2
    py::class_<qf::IcpClpCashflow2, std::shared_ptr<qf::IcpClpCashflow2>, qf::LinearInterestRateCashflow>(
            m, "IcpClpCashflow2")
            .def(py::init<QCDate &, QCDate &, QCDate &, double, double, bool, double, double, bool, double, double>())
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
            .def < double(qf::IcpClpCashflow2::*)() > ("interest", &qf::IcpClpCashflow2::interest)
            .def < double(qf::IcpClpCashflow2::*)(const qf::TimeSeries&)>("interest", &qf::IcpClpCashflow2::interest)
            .def < double(qf::IcpClpCashflow2::*)() > ("fixing", &qf::IcpClpCashflow2::fixing)
            .def < double(qf::IcpClpCashflow2::*)(const qf::TimeSeries&)>("fixing", &qf::IcpClpCashflow2::fixing)
            .def < double(qf::IcpClpCashflow2::*)(const QCDate&)>("accrued_interest", &qf::IcpClpCashflow2::accruedInterest)
            .def < double(qf::IcpClpCashflow2::*)(const QCDate&, const qf::TimeSeries&)>("accrued_interest",&qf::IcpClpCashflow2::accruedInterest)
            .def < double(qf::IcpClpCashflow2::*)(const QCDate&)>("accrued_fixing", &qf::IcpClpCashflow2::accruedFixing)
            .def < double(qf::IcpClpCashflow2::*)(const QCDate&, const qf::TimeSeries&)>("accrued_fixing",&qf::IcpClpCashflow2::accruedFixing)
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

    m.def("show", py::overload_cast<const std::shared_ptr<qf::IcpClpCashflow2>&>(&show));

    // IcpClfCashflow
    py::class_<qf::IcpClfCashflow, std::shared_ptr<qf::IcpClfCashflow>, qf::IcpClpCashflow>(
            m, "IcpClfCashflow")
            .def(py::init<QCDate &, QCDate &, QCDate &, double, double, bool, double, double, vector<double>>())
            .def("amount", &qf::IcpClfCashflow::amount)
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
    py::class_<qf::CompoundedOvernightRateCashflow, std::shared_ptr<qf::CompoundedOvernightRateCashflow>, qf::LinearInterestRateCashflow>(
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
            .def("set_nominal", &qf::CompoundedOvernightRateCashflow::setNominal)
            .def("set_amortization", &qf::CompoundedOvernightRateCashflow::setAmortization)
            .def("is_expired", &qf::CompoundedOvernightRateCashflow::isExpired)
            .def("get_eq_rate_decimal_places", &qf::CompoundedOvernightRateCashflow::getEqRateDecimalPlaces)
            .def("get_amount_derivatives", &qf::CompoundedOvernightRateCashflow::getAmountDerivatives)
            .def("get_interest_rate_index_code", &qf::CompoundedOvernightRateCashflow::getInterestRateIndexCode);

    m.def("show", py::overload_cast<const std::shared_ptr<qf::CompoundedOvernightRateCashflow>&>(&show));

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}

