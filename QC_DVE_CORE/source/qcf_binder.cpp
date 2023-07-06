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

#include <time/QCDate.h>
#include <time/QCBusinessCalendar.h>

#include <QcfinancialPybind11Helpers.h>

namespace qf = QCode::Financial;

PYBIND11_MAKE_OPAQUE(std::vector<QCDate>);

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
    py::class_<qf::InterestRateIndex, shared_ptr<qf::InterestRateIndex>>(m, "InterestRateIndex")
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
            .def(py::init<const QCDate&, double, std::shared_ptr<QCCurrency>>());

    m.def("show", py::overload_cast<const std::shared_ptr<qf::SimpleCashflow>&>(&show));

    // TimeSeries
    py::bind_map<std::map<QCDate, double>>(m, "time_series");






#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}

