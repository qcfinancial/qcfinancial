#define BOOST_PYTHON_STATIC_LIB
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include<memory>
#include<string>

#include "QCCurrency.h"
#include "QCYearFraction.h"
#include "QCDate.h"
#include "QCAct360.h"
#include "QCAct365.h"
#include "QC30360.h"
#include "QCWealthFactor.h"
#include "QCLinearWf.h"
#include "QCCompoundWf.h"
#include "QCContinousWf.h"
#include "QCInterestRate.h"
#include "QCBusinessCalendar.h"
#include "QCInterestRateLeg.h"

#include "Cashflow.h"
#include "FixedRateCashflow.h"
#include "IborCashflow.h"
#include "Simplecashflow.h"
#include "IcpClpCashflow.h"
#include "InterestRateIndex.h"
#include "LegFactory.h"
#include "Tenor.h"
#include "Leg.h"
#include "FXRate.h"
#include "FXRateIndex.h"


#include "Wrappers.h"

using namespace boost::python;
namespace qf = QCode::Financial;

BOOST_PYTHON_MODULE(QC_Financial)
{
	class_<QCCurrency, shared_ptr<QCCurrency>>("QCCurrency")
		.def("get_name", &QCCurrency::getName)
		.def("get_iso_code", &QCCurrency::getIsoCode)
		.def("get_iso_number", &QCCurrency::getIsoNumber)
		.def("get_decimal_places", &QCCurrency::getDecimalPlaces)
		.def("amount", &QCCurrency::amount)
		;

	implicitly_convertible<std::shared_ptr<QCCLF>, shared_ptr<QCCurrency>>();
	class_<QCCLF, std::shared_ptr<QCCLF>, bases<QCCurrency>>("QCCLF");

	implicitly_convertible<std::shared_ptr<QCBRL>, shared_ptr<QCCurrency>>();
	class_<QCBRL, std::shared_ptr<QCBRL>, bases<QCCurrency>>("QCBRL");

	implicitly_convertible<std::shared_ptr<QCCLP>, shared_ptr<QCCurrency>>();
	class_<QCCLP, std::shared_ptr<QCCLP>, bases<QCCurrency>>("QCCLP");

	implicitly_convertible<std::shared_ptr<QCMXN>, shared_ptr<QCCurrency>>();
	class_<QCMXN, std::shared_ptr<QCMXN>, bases<QCCurrency>>("QCMXN");

	implicitly_convertible<std::shared_ptr<QCCAD>, shared_ptr<QCCurrency>>();
	class_<QCCAD, std::shared_ptr<QCCAD>, bases<QCCurrency>>("QCCAD");

	implicitly_convertible<std::shared_ptr<QCUSD>, shared_ptr<QCCurrency>>();
	class_<QCUSD, std::shared_ptr<QCUSD>, bases<QCCurrency>>("QCUSD");

	implicitly_convertible<std::shared_ptr<QCEUR>, shared_ptr<QCCurrency>>();
	class_<QCEUR, std::shared_ptr<QCEUR>, bases<QCCurrency>>("QCEUR");

	implicitly_convertible<std::shared_ptr<QCGBP>, shared_ptr<QCCurrency>>();
	class_<QCGBP, std::shared_ptr<QCGBP>, bases<QCCurrency>>("QCGBP");

	class_<QCDate>("QCDate", init<int, int, int>())
		.def(init<>())
		.def(init<long>())
		.def("set_day", &QCDate::setDay)
		.def("set_month", &QCDate::setMonth)
		.def("set_year", &QCDate::setYear)
		.def("day", &QCDate::day)
		.def("month", &QCDate::month)
		.def("year", &QCDate::year)
		.def("week_day", &QCDate::weekDay)
		.def(self_ns::str(self_ns::self))
		;
	
	enum_<QCDate::QCWeekDay>("WeekDay")
		.value("MON", QCDate::qcMonday)
		.value("TUE", QCDate::qcTuesday)
		.value("WED", QCDate::qcWednesday)
		.value("THU", QCDate::qcThursday)
		.value("FRI", QCDate::qcFriday)
		.value("SAT", QCDate::qcSaturday)
		.value("SUN", QCDate::qcSunday)
		;

	enum_<QCDate::QCBusDayAdjRules>("BusyAdjRules")
		.value("NO", QCDate::qcNo)
		.value("FOLLOW", QCDate::qcFollow)
		.value("MODFOLLOW", QCDate::qcModFollow)
		.value("PREVIOUS", QCDate::qcPrev)
		.value("MODPREVIOUS", QCDate::qcModPrev)
		;

	class_<QCYearFraction>("QCYearFraction", no_init);

	implicitly_convertible<std::shared_ptr<QCAct360>, std::shared_ptr<QCYearFraction>>();
	
	double (QCAct360::*yf1_act360)(long) = &QCAct360::yf;
	double (QCAct360::*yf2_act360)(const QCDate&, const QCDate&) = &QCAct360::yf;

	class_<QCAct360, std::shared_ptr<QCAct360>, bases<QCYearFraction>>("QCAct360")
		.def("yf", yf1_act360)
		.def("yf", yf2_act360)
		.def("count_days", &QCAct360::countDays)
		;

	implicitly_convertible<std::shared_ptr<QCAct365>, std::shared_ptr<QCYearFraction>>();

	double (QCAct365::*yf1_act365)(long) = &QCAct365::yf;
	double (QCAct365::*yf2_act365)(const QCDate&, const QCDate&) = &QCAct365::yf;

	class_<QCAct365, std::shared_ptr<QCAct365>, bases<QCYearFraction>>("QCAct365")
		.def("yf", yf1_act365)
		.def("yf", yf2_act365)
		.def("count_days", &QCAct365::countDays)
		;

	implicitly_convertible<std::shared_ptr<QC30360>, std::shared_ptr<QCYearFraction>>();

	double (QC30360::*yf1_30360)(long) = &QC30360::yf;
	double (QC30360::*yf2_30360)(const QCDate&, const QCDate&) = &QC30360::yf;

	class_<QC30360, std::shared_ptr<QC30360>, bases<QCYearFraction>>("QC30360")
		.def("yf", yf1_30360)
		.def("yf", yf2_30360)
		.def("count_days", &QC30360::countDays)
		;

	class_<QCWealthFactor>("QCWealthFactor", no_init);

	implicitly_convertible<std::shared_ptr<QCLinearWf>, std::shared_ptr<QCWealthFactor>>();

	class_<QCLinearWf, std::shared_ptr<QCLinearWf>, bases<QCWealthFactor>>("QCLinearWf")
		.def("wf", &QCLinearWf::wf)
		.def("rate", &QCLinearWf::rate)
		;

	implicitly_convertible<std::shared_ptr<QCCompoundWf>, std::shared_ptr<QCWealthFactor>>();

	class_<QCCompoundWf, std::shared_ptr<QCCompoundWf>, bases<QCWealthFactor>>("QCCompoundWf")
		.def("wf", &QCCompoundWf::wf)
		.def("rate", &QCCompoundWf::rate)
		;

	implicitly_convertible<std::shared_ptr<QCContinousWf>, std::shared_ptr<QCWealthFactor>>();

	class_<QCContinousWf, std::shared_ptr<QCContinousWf>, bases<QCWealthFactor>>("QCContinousWf")
		.def("wf", &QCContinousWf::wf)
		.def("rate", &QCContinousWf::rate)
		;

	double (QCInterestRate::*wf1)(QCDate&, QCDate&) = &QCInterestRate::wf;
	double (QCInterestRate::*wf2)(long) = &QCInterestRate::wf;

	double (QCInterestRate::*dwf1)(QCDate&, QCDate&) = &QCInterestRate::dwf;
	double (QCInterestRate::*dwf2)(long) = &QCInterestRate::dwf;

	double (QCInterestRate::*grfwf1)(double, QCDate&, QCDate&) = &QCInterestRate::getRateFromWf;
	double (QCInterestRate::*grfwf2)(double, long) = &QCInterestRate::getRateFromWf;

	class_<QCInterestRate>("QCInterestRate", init<double, std::shared_ptr<QCYearFraction>, std::shared_ptr<QCWealthFactor>>())
		.def("get_value", &QCInterestRate::getValue)
		.def("set_value", &QCInterestRate::setValue)
		.def("wf", wf1)
		.def("wf", wf2)
		.def("dwf", dwf1)
		.def("dwf", dwf2)
		.def("get_rate_from_wf", grfwf1)
		.def("get_rate_from_wf", grfwf2)
		.def("yf", &QCInterestRate::yf)
		;

	enum_<qf::RecPay>("RecPay")
		.value("RECEIVE", qf::Receive)
		.value("PAY", qf::Pay)
		;

	class_<qf::Cashflow, std::shared_ptr<qf::Cashflow>>("Cashflow")
		.def("amount", &qf::Cashflow::amount)
		.def("date", &qf::Cashflow::date)
		.def("ccy", &qf::Cashflow::ccy)
		.def("isExpired", &qf::Cashflow::isExpired)
		;

	implicitly_convertible<std::shared_ptr<qf::FixedRateCashflow>, std::shared_ptr<qf::Cashflow>>();

	class_<qf::FixedRateCashflow, std::shared_ptr<qf::FixedRateCashflow>, bases<qf::Cashflow>>("FixedRateCashflow", init < QCDate&, QCDate&, QCDate&,
							  double, double, bool,
							  const QCInterestRate&,
							  shared_ptr < QCCurrency >> ())
							  .def("amount", &qf::FixedRateCashflow::amount)
							  .def("ccy", &qf::FixedRateCashflow::ccy)
							  .def("date", &qf::FixedRateCashflow::date)
							  .def("wrap", &qf::FixedRateCashflow::wrap)
							  .def("set_nominal", &qf::FixedRateCashflow::setNominal)
							  .def("set_amortization", &qf::FixedRateCashflow::setAmortization)
							  ;
	
	PyObject* (*show1)(qf::FixedRateCashflow) = wrappers::show;
	def("show", show1);

	PyObject* (*show11)(std::shared_ptr<qf::FixedRateCashflow>) = wrappers::show;
	def("show", show11);

	class_<qf::IborCashflow, std::shared_ptr<qf::IborCashflow>, bases<qf::Cashflow>>("IborCashflow", init <std::shared_ptr<qf::InterestRateIndex>, QCDate&, QCDate&, QCDate&, QCDate&,
							 double, double, bool,
							 shared_ptr <QCCurrency>,
							 double, double> ())
							 .def("amount", &qf::IborCashflow::amount)
							 .def("ccy", &qf::IborCashflow::ccy)
							 .def("date", &qf::IborCashflow::date)
							 .def("wrap", &qf::IborCashflow::wrap)
							 ;

	PyObject* (*show2)(qf::IborCashflow) = wrappers::show;
	def("show", show2);

	PyObject* (*show21)(std::shared_ptr<qf::IborCashflow>) = wrappers::show;
	def("show", show21);

	class_<qf::SimpleCashflow, std::shared_ptr<qf::SimpleCashflow>, bases<qf::Cashflow>>
		("SimpleCashflow", init <QCDate&, double, shared_ptr <QCCurrency>>())
		.def("amount", &qf::SimpleCashflow::amount)
		.def("ccy", &qf::SimpleCashflow::ccy)
		.def("date", &qf::SimpleCashflow::date)
		.def("wrap", &qf::SimpleCashflow::wrap)
		;

	PyObject* (*show3)(qf::SimpleCashflow) = wrappers::show;
	def("show", show3);

	PyObject* (*show31)(std::shared_ptr<qf::SimpleCashflow>) = wrappers::show;
	def("show", show31);

	class_<qf::SimpleMultiCurrencyCashflow, std::shared_ptr<qf::SimpleMultiCurrencyCashflow>, bases<qf::SimpleCashflow>>
		("SimpleMultiCurrencyCashflow", init <QCDate&, double, std::shared_ptr <QCCurrency>,
		QCDate&, std::shared_ptr <QCCurrency >, std::shared_ptr<qf::FXRateIndex>, double> ())
		.def("amount", &qf::SimpleMultiCurrencyCashflow::amount)
		.def("nominal", &qf::SimpleMultiCurrencyCashflow::nominal)
		.def("ccy", &qf::SimpleMultiCurrencyCashflow::ccy)
		.def("settlement_ccy", &qf::SimpleMultiCurrencyCashflow::settlementCcy)
		.def("set_fx_rate_index_value", &qf::SimpleMultiCurrencyCashflow::setFxRateIndexValue)
		.def("date", &qf::SimpleMultiCurrencyCashflow::date)
		.def("wrap", &qf::SimpleMultiCurrencyCashflow::wrap)
		;

	PyObject* (*show4)(qf::SimpleMultiCurrencyCashflow) = wrappers::show;
	def("show", show4);

	PyObject* (*show41)(std::shared_ptr<qf::SimpleMultiCurrencyCashflow>) = wrappers::show;
	def("show", show41);

	class_<qf::IcpClpCashflow, std::shared_ptr<qf::IcpClpCashflow>, bases<qf::Cashflow>>
		("IcpClpCashflow", init<QCDate&, QCDate&, QCDate&, double, double, bool, double, double, double, double>())
		.def("amount", &qf::IcpClpCashflow::amount)
		.def("accrued_interest", &qf::IcpClpCashflow::accruedInterest)
		.def("set_start_date_icp", &qf::IcpClpCashflow::setStartDateICP)
		.def("set_end_date_icp", &qf::IcpClpCashflow::setEndDateICP)
		.def("ccy", &qf::IcpClpCashflow::ccy)
		.def("date", &qf::IcpClpCashflow::date)
		.def("set_nominal", &qf::IcpClpCashflow::setNominal)
		.def("set_amortization", &qf::IcpClpCashflow::setAmortization)
		.def("get_rate_value", &qf::IcpClpCashflow::getRateValue)
		.def("get_type_of_rate", &qf::IcpClpCashflow::getTypeOfRate)
		;

	PyObject* (*show5)(qf::IcpClpCashflow) = wrappers::show;
	def("show", show5);

	PyObject* (*show51)(std::shared_ptr<qf::IcpClpCashflow>) = wrappers::show;
	def("show", show51);


	class_<DateList>("DateList")
		.def(vector_indexing_suite<DateList>())
		;

	class_<QCBusinessCalendar>("BusinessCalendar", init<const QCDate&, int>())
		.def("add_holiday", &QCBusinessCalendar::addHolyday)
		.def("next_busy_day", &QCBusinessCalendar::nextBusinessDay)
		.def("mod_next_busy_day", &QCBusinessCalendar::modNextBusinessDay)
		.def("prev_busy_day", &QCBusinessCalendar::previousBusinessDay)
		.def("shift", &QCBusinessCalendar::shift)
		.def("get_holydays", &QCBusinessCalendar::getHolidays)
		;

	enum_<QCInterestRateLeg::QCStubPeriod>("StubPeriod")
		.value("NO", QCInterestRateLeg::qcNoStubPeriod)
		.value("SHORTBACK", QCInterestRateLeg::qcShortBack)
		.value("SHORTFRONT", QCInterestRateLeg::qcShortFront)
		.value("LONGBACK", QCInterestRateLeg::qcLongBack)
		.value("LONGFRONT", QCInterestRateLeg::qcLongFront)
		.value("LONGFRONT2", QCInterestRateLeg::qcLongFront2)
		.value("LONGFRONT3", QCInterestRateLeg::qcLongFront3)
		.value("LONGFRONT4", QCInterestRateLeg::qcLongFront4)
		.value("LONGFRONT5", QCInterestRateLeg::qcLongFront5)
		;

	class_<qf::Leg>("Leg")
		.def("set_cashflow_at", &qf::Leg::setCashflowAt)
		.def("get_cashflow_at", &qf::Leg::getCashflowAt)
		.def("append_cashflow", &qf::Leg::appendCashflow)
		.def("size", &qf::Leg::size)
		.def("resize", &qf::Leg::resize)
		;

	class_<qf::CustomNotionalAmort>("CustomNotionalAmort")
		.def("set_size", &qf::CustomNotionalAmort::setSize)
		.def("get_size", &qf::CustomNotionalAmort::getSize)
		.def("set_notional_amort_at", &qf::CustomNotionalAmort::setNotionalAmortAt)
		.def("pushback_notional_amort", &qf::CustomNotionalAmort::pushbackNotionalAmort)
		.def("get_notional_at", &qf::CustomNotionalAmort::getNotionalAt)
		.def("get_amort_at", &qf::CustomNotionalAmort::getAmortAt)
		;

	class_<qf::Tenor>("Tenor", init<std::string>())
		.def("set_tenor", &qf::Tenor::setTenor)
		.def("get_years", &qf::Tenor::getYears)
		.def("get_months", &qf::Tenor::getMonths)
		.def("get_days", &qf::Tenor::getDays)
		;

	class_<qf::FinancialIndex, std::shared_ptr<qf::FinancialIndex>>("FinancialIndex", init<qf::FinancialIndex::AssetClass, std::string>())
		.def("get_asset_class", &qf::FinancialIndex::getAssetClass)
		.def("get_code", &qf::FinancialIndex::getCode)
		;

	enum_<qf::FinancialIndex::AssetClass>("AssetClass")
		.value("InterestRate", qf::FinancialIndex::InterestRate)
		.value("InterestRateVol", qf::FinancialIndex::InterestRateVolatility)
		.value("Fx", qf::FinancialIndex::Fx)
		.value("FxVol", qf::FinancialIndex::FxVolatility)
		.value("Equity", qf::FinancialIndex::Equity)
		.value("EquityVol", qf::FinancialIndex::EquityVolatility)
		.value("Commodity", qf::FinancialIndex::Commodity)
		.value("CommodityVol", qf::FinancialIndex::CommodityVolatility)
		.value("Credit", qf::FinancialIndex::Credit)
		.value("CreditVol", qf::FinancialIndex::CreditVolatility)
		;

	class_<qf::InterestRateIndex, std::shared_ptr<qf::InterestRateIndex>, bases<qf::FinancialIndex>>
		("InterestRateIndex", init<std::string, QCInterestRate, qf::Tenor, qf::Tenor, QCBusinessCalendar,
		QCBusinessCalendar, std::shared_ptr<QCCurrency>>())
		.def("set_rate_value", &qf::InterestRateIndex::setRateValue)
		.def("get_start_date", &qf::InterestRateIndex::getStartDate)
		.def("get_end_date", &qf::InterestRateIndex::getEndDate)
		.def("get_tenor", &qf::InterestRateIndex::getTenor)
		.def("get_rate", &qf::InterestRateIndex::getRate)
		;

	class_<qf::LegFactory>("LegFactory", no_init)
		.def("build_bullet_fixed_rate_leg", &qf::LegFactory::buildBulletFixedRateLeg)
		.staticmethod("build_bullet_fixed_rate_leg")
		.def("build_custom_amort_fixed_rate_leg", &qf::LegFactory::buildCustomAmortFixedRateLeg)
		.staticmethod("build_custom_amort_fixed_rate_leg")
		.def("build_bullet_ibor_leg", &qf::LegFactory::buildBulletIborLeg)
		.staticmethod("build_bullet_ibor_leg")
		.def("build_custom_amort_ibor_leg", &qf::LegFactory::buildCustomAmortIborLeg)
		.staticmethod("build_custom_amort_ibor_leg")
		.def("build_bullet_icp_clp_leg", &qf::LegFactory::buildBulletIcpClpLeg)
		.staticmethod("build_bullet_icp_clp_leg")
		.def("build_custom_amort_icp_clp_leg", &qf::LegFactory::buildCustomAmortIcpClpLeg)
		.staticmethod("build_custom_amort_icp_clp_leg")
		;

	class_<qf::FXRate, std::shared_ptr<qf::FXRate>>
		("FXRate", init<std::shared_ptr<QCCurrency>, std::shared_ptr<QCCurrency>>())
		.def("get_code", &qf::FXRate::getCode)
		;

	class_<qf::FXRateIndex, std::shared_ptr<qf::FXRateIndex>, bases<qf::FinancialIndex>>
		("FXRateIndex", init<std::shared_ptr<qf::FXRate>, std::string, qf::Tenor, qf::Tenor, QCBusinessCalendar>())
		.def("fixing_date", &qf::FXRateIndex::fixingDate)
		.def("value_date", &qf::FXRateIndex::valueDate)
		;

}