#define BOOST_PYTHON_STATIC_LIB
#define BOOST_PYTHON_MAX_ARITY 25
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <boost/python/docstring_options.hpp>

#include<memory>
#include<string>
#include <tuple>

#include "QCCurrency.h"
#include "QCYearFraction.h"
#include "QCDate.h"
#include "QCAct360.h"
#include "QCAct365.h"
#include "QC30360.h"
#include "QCAct30.h"
#include "QC3030.h"
#include "QCWealthFactor.h"
#include "QCLinearWf.h"
#include "QCCompoundWf.h"
#include "QCContinousWf.h"
#include "QCInterestRate.h"
#include "QCBusinessCalendar.h"
#include "QCInterestRateLeg.h"
#include "QCCurve.h"
#include "QCInterpolator.h"
#include "QCLinearInterpolator.h"
#include "QCCurrencyConverter.h"

#include "Cashflow.h"
#include "LinearInterestRateCashflow.h"
#include "FixedRateCashflow.h"
#include "FixedRateMultiCurrencyCashflow.h"
#include "IborCashflow.h"
#include "Simplecashflow.h"
#include "IcpClpCashflow.h"
#include "IcpClfCashflow.h"
#include "InterestRateIndex.h"
#include "LegFactory.h"
#include "Tenor.h"
#include "Leg.h"
#include "AssetFactory.h"
#include "FXRate.h"
#include "IndexFactory.h"
#include "FXRateIndex.h"
#include "InterestRateCurve.h"
#include "PresentValue.h"
#include "FixedRateBond.h"
#include "ChileanFixedRateBond.h"
#include "ForwardRates.h"
#include "TypeAliases.h"
#include "Quanto.h"

#include "FixedRateCashflow2.h"

#include "Wrappers.h"
#include "Include/Wrappers.h"

using namespace boost::python;
namespace qf = QCode::Financial;

BOOST_PYTHON_MODULE(QC_Financial)
{
	docstring_options doc_options(true);

	class_<QCCurrency, shared_ptr<QCCurrency>>("QCCurrency")
		.def("get_name", &QCCurrency::getName)
		.def("get_iso_code", &QCCurrency::getIsoCode)
		.def("get_iso_number", &QCCurrency::getIsoNumber)
		.def("get_decimal_places", &QCCurrency::getDecimalPlaces)
		.def("amount", &QCCurrency::amount)
		.def("__str__", &QCCurrency::getIsoCode)
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

	implicitly_convertible<std::shared_ptr<QCJPY>, shared_ptr<QCCurrency>>();
	class_<QCJPY, std::shared_ptr<QCJPY>, bases<QCCurrency>>("QCJPY");

	def("get_qccurrency_from_code", &qf::getQCCurrencyFromCode);

	std::tuple<unsigned long, int>(QCDate::*mddiff_1)(const QCDate&, std::vector<QCDate>&, QCDate::QCBusDayAdjRules) const = 
		&QCDate::monthDiffDayRemainder;
	std::tuple<unsigned long, int>(QCDate::*mddiff_2)(const QCDate&, shared_ptr<vector<QCDate>>, QCDate::QCBusDayAdjRules) const = 
		&QCDate::monthDiffDayRemainder;

	class_<std::tuple<unsigned long, int>>("tupla_uint_int");
	def("first", &wrappers::first);
	def("second", &wrappers::second);

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
		.def("description", &QCDate::description)
		.def("add_months", &QCDate::addMonths)
		.def("add_days", &QCDate::addDays)
		.def("day_diff", &QCDate::dayDiff)
		.def("month_diff_day_remainder", mddiff_1)
		.def("month_diff_day_remainder", mddiff_2)
		.def("__lt__", &QCDate::operator<)
		.def("__le__", &QCDate::operator<=)
		.def("__eq__", &QCDate::operator==)
		.def("__ne__", &QCDate::operator!=)
		.def("__ge__", &QCDate::operator>=)
		.def("__gt__", &QCDate::operator>)
		.def("__hash__", &QCDate::excelSerial)
		.def(self_ns::str(self_ns::self))
		;

	def("build_qcdate_from_string", &wrappers::buildQCDateFromString);

	
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
		.def("__str__", &QCAct360::description)
		;

	implicitly_convertible<std::shared_ptr<QCAct365>, std::shared_ptr<QCYearFraction>>();

	double (QCAct365::*yf1_act365)(long) = &QCAct365::yf;
	double (QCAct365::*yf2_act365)(const QCDate&, const QCDate&) = &QCAct365::yf;

	class_<QCAct365, std::shared_ptr<QCAct365>, bases<QCYearFraction>>("QCAct365")
		.def("yf", yf1_act365)
		.def("yf", yf2_act365)
		.def("count_days", &QCAct365::countDays)
		.def("__str__", &QCAct365::description)
		;

	implicitly_convertible<std::shared_ptr<QC30360>, std::shared_ptr<QCYearFraction>>();

	double (QC30360::*yf1_30360)(long) = &QC30360::yf;
	double (QC30360::*yf2_30360)(const QCDate&, const QCDate&) = &QC30360::yf;

	class_<QC30360, std::shared_ptr<QC30360>, bases<QCYearFraction>>("QC30360")
		.def("yf", yf1_30360)
		.def("yf", yf2_30360)
		.def("count_days", &QC30360::countDays)
		.def("__str__", &QC30360::description)
		;

	implicitly_convertible<std::shared_ptr<QC3030>, std::shared_ptr<QCYearFraction>>();

	double (QC3030::*yf1_3030)(long) = &QC3030::yf;
	double (QC3030::*yf2_3030)(const QCDate&, const QCDate&) = &QC3030::yf;

	class_<QC3030, std::shared_ptr<QC3030>, bases<QCYearFraction>>("QC3030")
		.def("yf", yf1_3030)
		.def("yf", yf2_3030)
		.def("count_days", &QC3030::countDays)
		.def("__str__", &QC3030::description)
		;

	implicitly_convertible<std::shared_ptr<QCAct30>, std::shared_ptr<QCYearFraction>>();

	double (QCAct30::*yf1_Act30)(long) = &QCAct30::yf;
	double (QCAct30::*yf2_Act30)(const QCDate&, const QCDate&) = &QCAct30::yf;

	class_<QCAct30, std::shared_ptr<QCAct30>, bases<QCYearFraction>>("QCAct30")
		.def("yf", yf1_Act30)
		.def("yf", yf2_Act30)
		.def("count_days", &QCAct30::countDays)
		.def("__str__", &QCAct30::description)
		;

	class_<QCWealthFactor>("QCWealthFactor", no_init);

	implicitly_convertible<std::shared_ptr<QCLinearWf>, std::shared_ptr<QCWealthFactor>>();

	class_<QCLinearWf, std::shared_ptr<QCLinearWf>, bases<QCWealthFactor>>("QCLinearWf")
		.def("wf", &QCLinearWf::wf)
		.def("rate", &QCLinearWf::rate)
		.def("__str__", &QCLinearWf::description)
		;

	implicitly_convertible<std::shared_ptr<QCCompoundWf>, std::shared_ptr<QCWealthFactor>>();

	class_<QCCompoundWf, std::shared_ptr<QCCompoundWf>, bases<QCWealthFactor>>("QCCompoundWf")
		.def("wf", &QCCompoundWf::wf)
		.def("rate", &QCCompoundWf::rate)
		.def("__str__", &QCCompoundWf::description)
		;

	implicitly_convertible<std::shared_ptr<QCContinousWf>, std::shared_ptr<QCWealthFactor>>();

	class_<QCContinousWf, std::shared_ptr<QCContinousWf>, bases<QCWealthFactor>>("QCContinousWf")
		.def("wf", &QCContinousWf::wf)
		.def("rate", &QCContinousWf::rate)
		.def("__str__", &QCContinousWf::description)
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
		.def("__str__", &QCInterestRate::description)
		;

	enum_<qf::RecPay>("RecPay")
		.value("RECEIVE", qf::Receive)
		.value("PAY", qf::Pay)
		;

	class_<QCCurrencyConverter>("CurrencyConverter")
		.def<double (QCCurrencyConverter::*)(double, shared_ptr<QCCurrency>, double, QCode::Financial::FXRateIndex&)>("convert", &QCCurrencyConverter::convert)
		.def("get_fx_rate_mkt_code", &QCCurrencyConverter::getFxRateMktCode)
		;

	class_<wrappers::CashflowWrap, boost::noncopyable>("Cashflow")
		.def("amount", pure_virtual(&qf::Cashflow::amount))
		.def("date", pure_virtual(&qf::Cashflow::date))
		.def("ccy", pure_virtual(&qf::Cashflow::ccy))
		.def("isExpired", pure_virtual(&qf::Cashflow::isExpired))
		;

	register_ptr_to_python<std::shared_ptr<qf::Cashflow>>();
	implicitly_convertible<std::shared_ptr<wrappers::CashflowWrap>, std::shared_ptr<qf::Cashflow>>();

	class_< wrappers::LinearInterestRateCashflowWrap, boost::noncopyable, bases<qf::Cashflow>>("LinearInterestRateCashflow")
		.def("get_initial_currency", pure_virtual(&qf::LinearInterestRateCashflow::getInitialCcy))
		.def("does_amortize", pure_virtual(&qf::LinearInterestRateCashflow::doesAmortize))
		.def("get_start_date", pure_virtual(&qf::LinearInterestRateCashflow::getStartDate), return_value_policy<copy_const_reference>())
		.def("get_end_date", pure_virtual(&qf::LinearInterestRateCashflow::getEndDate), return_value_policy<copy_const_reference>())
		.def("get_settlement_date", pure_virtual(&qf::LinearInterestRateCashflow::getSettlementDate), return_value_policy<copy_const_reference>())
		.def("get_fixing_dates", pure_virtual(&qf::LinearInterestRateCashflow::getFixingDates), return_value_policy<copy_const_reference>())
		.def("get_nominal", pure_virtual(&qf::LinearInterestRateCashflow::getNominal))
		.def("nominal", pure_virtual(&qf::LinearInterestRateCashflow::nominal))
		.def("get_amortization", pure_virtual(&qf::LinearInterestRateCashflow::getAmortization))
		.def("interest", pure_virtual<double (qf::LinearInterestRateCashflow::*)()>(&qf::LinearInterestRateCashflow::interest))
		.def("interest", pure_virtual<double (qf::LinearInterestRateCashflow::*)(const qf::TimeSeries&)>(&qf::LinearInterestRateCashflow::interest))
		.def("fixing", pure_virtual<double (qf::LinearInterestRateCashflow::*)()>(&qf::LinearInterestRateCashflow::fixing))
		.def("fixing", pure_virtual<double (qf::LinearInterestRateCashflow::*)(const qf::TimeSeries&)>(&qf::LinearInterestRateCashflow::fixing))
		.def("accrued_interest", pure_virtual<double (qf::LinearInterestRateCashflow::*)(const QCDate&)>(&qf::LinearInterestRateCashflow::accruedInterest))
		.def("accrued_interest", pure_virtual<double (qf::LinearInterestRateCashflow::*)(const QCDate&, const qf::TimeSeries&)>(&qf::LinearInterestRateCashflow::accruedInterest))
		.def("accrued_fixing", pure_virtual<double (qf::LinearInterestRateCashflow::*)(const QCDate&)>(&qf::LinearInterestRateCashflow::accruedFixing))
		.def("accrued_fixing", pure_virtual<double (qf::LinearInterestRateCashflow::*)(const QCDate&, const qf::TimeSeries&)>(&qf::LinearInterestRateCashflow::accruedFixing))
		;

	register_ptr_to_python<std::shared_ptr<qf::LinearInterestRateCashflow>>();
	implicitly_convertible<std::shared_ptr<wrappers::LinearInterestRateCashflowWrap>, std::shared_ptr<qf::LinearInterestRateCashflow>>();

	implicitly_convertible<std::shared_ptr<qf::FixedRateCashflow2>, std::shared_ptr<qf::LinearInterestRateCashflow>>();
	
	class_<QCode::Financial::DateList>("DateList")
		.def(vector_indexing_suite<QCode::Financial::DateList>())
		;
	
	class_<qf::FixedRateCashflow2, std::shared_ptr<qf::FixedRateCashflow2>, bases<qf::LinearInterestRateCashflow>>
		("FixedRateCashflow2", init < QCDate&, QCDate&, QCDate&, double, double, bool, const QCInterestRate&, shared_ptr < QCCurrency >> ())
		.def("amount", &qf::FixedRateCashflow2::amount)
		.def("ccy", &qf::FixedRateCashflow2::ccy)
		.def("date", &qf::FixedRateCashflow2::date)
		.def("get_start_date", &qf::FixedRateCashflow2::getStartDate, return_value_policy<copy_const_reference>())
		.def("get_end_date", &qf::FixedRateCashflow2::getEndDate, return_value_policy<copy_const_reference>())
		.def("get_settlement_date", &qf::FixedRateCashflow2::getSettlementDate, return_value_policy<copy_const_reference>())
		.def("get_fixing_dates", &qf::FixedRateCashflow2::getFixingDates, return_value_policy<copy_const_reference>())
		.def("get_nominal", &qf::FixedRateCashflow2::getNominal)
		.def("nominal", &qf::FixedRateCashflow2::nominal)
		.def("get_amortization", &qf::FixedRateCashflow2::getAmortization)
		.def<double (qf::FixedRateCashflow2::*)()>("interest", &qf::FixedRateCashflow2::interest)
		.def<double (qf::FixedRateCashflow2::*)(const qf::TimeSeries&)>("interest", &qf::FixedRateCashflow2::interest)
		.def<double(qf::FixedRateCashflow2::*)()>("fixing", &qf::FixedRateCashflow2::fixing)
		.def<double(qf::FixedRateCashflow2::*)(const qf::TimeSeries&)>("fixing", &qf::FixedRateCashflow2::fixing)
		.def<double (qf::FixedRateCashflow2::*)(const QCDate&)>("accrued_interest", &qf::FixedRateCashflow2::accruedInterest)
		.def<double (qf::FixedRateCashflow2::*)(const QCDate&, const qf::TimeSeries&)>("accrued_interest", &qf::FixedRateCashflow2::accruedInterest)
		.def<double (qf::FixedRateCashflow2::*)(const QCDate&)>("accrued_fixing", &qf::FixedRateCashflow2::accruedFixing)
		.def<double (qf::FixedRateCashflow2::*)(const QCDate&, const qf::TimeSeries&)>("accrued_fixing", &qf::FixedRateCashflow2::accruedFixing)
		.def("does_amortize", &qf::FixedRateCashflow2::doesAmortize)
		.def("wrap", &qf::FixedRateCashflow2::wrap)
		;

	class_<qf::FixedRateCashflowWrapper, std::shared_ptr<qf::FixedRateCashflowWrapper>>("FixedRateCashflowWrapper", no_init)
		.def("start_date", wrappers::startDate)
		.def("end_date", wrappers::endDate)
		.def("settlement_date", wrappers::settlementDate)
		.def("nominal", wrappers::nominal)
		.def("amortization", wrappers::amortization)
		.def("interest", wrappers::interest)
		.def("does_amortize", wrappers::doesAmortize)
		.def("currency", wrappers::currency)
		.def("interest_rate", wrappers::interestRate)
		;


	implicitly_convertible<std::shared_ptr<qf::FixedRateCashflow>, std::shared_ptr<qf::Cashflow>>();

	class_ < qf::QuantoCashflow, std::shared_ptr<qf::QuantoCashflow>, bases<qf::Cashflow>>
		("QuantoCashflow", init<std::shared_ptr<qf::Cashflow>, qf::FXRateIndex, const qf::TimeSeries&, QCDate>())
		.def("amount", &qf::QuantoCashflow::amount)
		.def("ccy", &qf::QuantoCashflow::ccy)
		.def("date", &qf::QuantoCashflow::date)
		;

	implicitly_convertible<std::shared_ptr<qf::QuantoCashflow>, std::shared_ptr<qf::Cashflow>>();

	class_ < qf::QuantoLinearInterestRateCashflow, std::shared_ptr<qf::QuantoLinearInterestRateCashflow>>
		("QuantoLinearInterestRateCashflow", init<std::shared_ptr<qf::LinearInterestRateCashflow>, qf::FXRateIndex, const qf::TimeSeries&, QCDate>())
		.def("amount", &qf::QuantoLinearInterestRateCashflow::amount)
		.def("ccy", &qf::QuantoLinearInterestRateCashflow::ccy)
		.def("get_initial_ccy", &qf::QuantoLinearInterestRateCashflow::getInitialCcy)
		.def("date", &qf::QuantoLinearInterestRateCashflow::date)
		.def("get_start_date", &qf::QuantoLinearInterestRateCashflow::getStartDate, return_value_policy<copy_const_reference>())
		.def("get_end_date", &qf::QuantoLinearInterestRateCashflow::getEndDate, return_value_policy<copy_const_reference>())
		.def("get_settlement_date", &qf::QuantoLinearInterestRateCashflow::getSettlementDate, return_value_policy<copy_const_reference>())
		.def("get_fixing_dates", &qf::QuantoLinearInterestRateCashflow::getFixingDates, return_value_policy<copy_const_reference>())
		.def("get_fx_rate_index_fixing_date", &qf::QuantoLinearInterestRateCashflow::getFxRateIndexFixingDate, return_value_policy<copy_const_reference>())
		.def("get_nominal", &qf::QuantoLinearInterestRateCashflow::getNominal)
		.def("nominal", &qf::QuantoLinearInterestRateCashflow::nominal)
		.def("get_amortization", &qf::QuantoLinearInterestRateCashflow::getAmortization)
		.def("amortization", &qf::QuantoLinearInterestRateCashflow::amortization)
		.def<double (qf::QuantoLinearInterestRateCashflow::*)()>("interest", &qf::QuantoLinearInterestRateCashflow::interest)
		.def<double (qf::QuantoLinearInterestRateCashflow::*)(const qf::TimeSeries&)>("interest", &qf::QuantoLinearInterestRateCashflow::interest)
		.def<double (qf::QuantoLinearInterestRateCashflow::*)()>("fixing", &qf::QuantoLinearInterestRateCashflow::fixing)
		.def<double (qf::QuantoLinearInterestRateCashflow::*)(const qf::TimeSeries&)>("fixing", &qf::QuantoLinearInterestRateCashflow::fixing)
		.def<double (qf::QuantoLinearInterestRateCashflow::*)(const QCDate&)>("accrued_interest", &qf::QuantoLinearInterestRateCashflow::accruedInterest)
		.def<double (qf::QuantoLinearInterestRateCashflow::*)(const QCDate&, const qf::TimeSeries&)>("accrued_interest", &qf::QuantoLinearInterestRateCashflow::accruedInterest)
		.def<double (qf::QuantoLinearInterestRateCashflow::*)(const QCDate&)>("accrued_fixing", &qf::QuantoLinearInterestRateCashflow::accruedFixing)
		.def<double (qf::QuantoLinearInterestRateCashflow::*)(const QCDate&, const qf::TimeSeries&)>("accrued_fixing", &qf::QuantoLinearInterestRateCashflow::accruedFixing)
		;
	
	implicitly_convertible<std::shared_ptr<qf::QuantoLinearInterestRateCashflow>, std::shared_ptr<qf::Cashflow>>();
	implicitly_convertible<std::shared_ptr<qf::QuantoLinearInterestRateCashflow>, std::shared_ptr<qf::LinearInterestRateCashflow>>();

	class_<qf::FixedRateCashflow, std::shared_ptr<qf::FixedRateCashflow>, bases<qf::Cashflow>>
		("FixedRateCashflow",
		               init < QCDate&, QCDate&, QCDate&,
							  double, double, bool,
							  const QCInterestRate&,
							  shared_ptr < QCCurrency >> ())
							  .def("amount", &qf::FixedRateCashflow::amount)
							  .def("accrued_interest", &qf::FixedRateCashflow::accruedInterest)
							  .def("ccy", &qf::FixedRateCashflow::ccy)
							  .def("get_settlement_date", &qf::FixedRateCashflow::date)
							  .def("get_start_date", &qf::FixedRateCashflow::getStartDate)
							  .def("get_end_date", &qf::FixedRateCashflow::getEndDate)
							  .def("wrap", &qf::FixedRateCashflow::wrap)
							  .def("set_nominal", &qf::FixedRateCashflow::setNominal)
							  .def("get_nominal", &qf::FixedRateCashflow::getNominal)
							  .def("set_amortization", &qf::FixedRateCashflow::setAmortization)
							  .def("get_amortization", &qf::FixedRateCashflow::getAmortization)
							  .def("wrap", &qf::FixedRateCashflow::wrap)
							  ;
	PyObject* (*show1)(qf::FixedRateCashflow) = wrappers::show;
	def("show", show1);

	PyObject* (*show11)(std::shared_ptr<qf::FixedRateCashflow>) = wrappers::show;
	def("show", show11);

	implicitly_convertible<std::shared_ptr<qf::FixedRateMultiCurrencyCashflow>, 
		std::shared_ptr<qf::Cashflow>>();
	

	class_<qf::FXVariation >("FXVariation", init<double, double>())
		.def_readwrite("interest_variation", &qf::FXVariation::interestVariation)
		.def_readwrite("nominal_variation", &qf::FXVariation::nominalVariation)
		;

	class_<qf::FixedRateMultiCurrencyCashflow, std::shared_ptr<qf::FixedRateMultiCurrencyCashflow>, 
		bases<qf::FixedRateCashflow>>("FixedRateMultiCurrencyCashflow",
		init < QCDate&, QCDate&, QCDate&,
		double, double, bool,
		const QCInterestRate&,
		shared_ptr < QCCurrency >,
		QCDate&, shared_ptr<QCCurrency>,
		shared_ptr<qf::FXRateIndex>, double> ())
		.def("amount", &qf::FixedRateMultiCurrencyCashflow::amount)
		.def("settlement_currency", &qf::FixedRateMultiCurrencyCashflow::settlementCcy)
		.def("set_fx_rate_index_value", &qf::FixedRateMultiCurrencyCashflow::setFxRateIndexValue)
		.def("accrued_interest", &qf::FixedRateCashflow::accruedInterest)
		.def("accrued_interest", &qf::FixedRateMultiCurrencyCashflow::accruedInterest)
		.def("get_amortization", &qf::FixedRateCashflow::getAmortization)
		.def("get_amortization", &qf::FixedRateMultiCurrencyCashflow::getAmortization)
		.def("accrued_fx_variation", &qf::FixedRateMultiCurrencyCashflow::accruedFXVariation)
		.def("get_fx_rate_index", &qf::FixedRateMultiCurrencyCashflow::getFXRateIndex)
		.def("get_fx_rate_index_code", &qf::FixedRateMultiCurrencyCashflow::getFXRateIndexCode)
		.def("wrap", &qf::FixedRateMultiCurrencyCashflow::wrap)
		;

	PyObject* (*show0)(qf::FixedRateMultiCurrencyCashflow) = wrappers::show;
	def("show", show0);

	PyObject* (*show01)(std::shared_ptr<qf::FixedRateMultiCurrencyCashflow>) = wrappers::show;
	def("show", show01);

	class_<qf::IborCashflow, std::shared_ptr<qf::IborCashflow>, bases<qf::Cashflow>>("IborCashflow",
		init <std::shared_ptr<qf::InterestRateIndex>, QCDate&, QCDate&, QCDate&, QCDate&,
							 double, double, bool,
							 shared_ptr <QCCurrency>,
							 double, double> ())
							 .def("amount", &qf::IborCashflow::amount)
							 .def("accrued_interest", &qf::IborCashflow::accruedInterest)
							 .def("ccy", &qf::IborCashflow::ccy)
							 .def("get_fixing_date", &qf::IborCashflow::getFixingDate)
							 .def("get_start_date", &qf::IborCashflow::getStartDate)
							 .def("get_end_date", &qf::IborCashflow::getEndDate)
							 .def("get_settlement_date", &qf::IborCashflow::date)
							 .def("wrap", &qf::IborCashflow::wrap)
							 .def("set_interest_rate_value", &qf::IborCashflow::setInterestRateValue)
							 .def("get_interest_rate_value", &qf::IborCashflow::getInterestRateValue)
							 .def("set_nominal", &qf::IborCashflow::setNominal)
							 .def("get_nominal", &qf::IborCashflow::getNominal)
							 .def("set_amortization", &qf::IborCashflow::setAmortization)
							 .def("get_amortization", &qf::IborCashflow::getAmortization)
							 ;
	PyObject* (*show2)(qf::IborCashflow) = wrappers::show;
	def("show", show2);

	PyObject* (*show21)(std::shared_ptr<qf::IborCashflow>) = wrappers::show;
	def("show", show21);

	class_<qf::IborMultiCurrencyCashflow, std::shared_ptr<qf::IborMultiCurrencyCashflow>,
		bases<qf::IborCashflow >> ("IborMultiCurrencyCashflow",
		init < std::shared_ptr<qf::InterestRateIndex>,
		const QCDate&, const QCDate&, const QCDate&, const QCDate&,
		double, double, bool,
		std::shared_ptr<QCCurrency>,
		double, double,
		const QCDate&,
		std::shared_ptr<QCCurrency>,
		std::shared_ptr<qf::FXRateIndex>,
		double>())
		.def("amount", &qf::IborMultiCurrencyCashflow::amount)
		.def("settlement_currency", &qf::IborMultiCurrencyCashflow::settlementCcy)
		.def("set_fx_rate_index_value", &qf::IborMultiCurrencyCashflow::setFxRateIndexValue)
		.def("accrued_interest", &qf::IborCashflow::accruedInterest)
		.def("accrued_interest", &qf::IborMultiCurrencyCashflow::accruedInterest)
		.def("accrued_fx_variation", &qf::IborMultiCurrencyCashflow::accruedFXVariation)
		.def("wrap", &qf::IborMultiCurrencyCashflow::wrap)
		;

	PyObject* (*show20)(qf::IborMultiCurrencyCashflow) = wrappers::show;
	def("show", show20);

	PyObject* (*show201)(std::shared_ptr<qf::IborMultiCurrencyCashflow>) = wrappers::show;
	def("show", show201);


	implicitly_convertible<std::shared_ptr<qf::SimpleCashflow>, std::shared_ptr<qf::Cashflow>>();

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

	class_<qf::SimpleMultiCurrencyCashflow, std::shared_ptr<qf::SimpleMultiCurrencyCashflow>,
		bases<qf::SimpleCashflow>>
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
		;

	PyObject* (*show5)(qf::IcpClpCashflow) = wrappers::show;
	def("show", show5);

	PyObject* (*show51)(std::shared_ptr<qf::IcpClpCashflow>) = wrappers::show;
	def("show", show51);

	class_<std::vector<double> >("double_vec")
		.def(vector_indexing_suite<std::vector<double> >())
		;

	class_<qf::IcpClfCashflow, std::shared_ptr<qf::IcpClfCashflow>, bases<qf::IcpClpCashflow>>
		("IcpClfCashflow", init<QCDate&, QCDate&, QCDate&, double, double, bool, double, double, vector<double>>())
		.def("amount", &qf::IcpClfCashflow::amount)
		.def("ccy", &qf::IcpClfCashflow::ccy)
		.def("accrued_interest", &qf::IcpClfCashflow::accruedInterest)
		.def("get_tra", &qf::IcpClfCashflow::getTra)
		.def("get_rate_value", &qf::IcpClfCashflow::getRateValue)
		.def("set_start_date_uf", &qf::IcpClfCashflow::setStartDateUf)
		.def("get_start_date_uf", &qf::IcpClfCashflow::getStartDateUf)
		.def("set_end_date_uf", &qf::IcpClfCashflow::setEndDateUf)
		.def("get_end_date_uf", &qf::IcpClfCashflow::getEndDateUf)
		.def("set_tra_decimal_places", &qf::IcpClfCashflow::setTraDecimalPlaces)
		;

	PyObject* (*show6)(qf::IcpClfCashflow) = wrappers::show;
	def("show", show6);

	PyObject* (*show61)(std::shared_ptr<qf::IcpClfCashflow>) = wrappers::show;
	def("show", show61);

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
		.value("LONGFRONT6", QCInterestRateLeg::qcLongFront6)
		.value("LONGFRONT7", QCInterestRateLeg::qcLongFront7)
		.value("LONGFRONT8", QCInterestRateLeg::qcLongFront8)
		.value("LONGFRONT9", QCInterestRateLeg::qcLongFront9)
		.value("LONGFRONT10", QCInterestRateLeg::qcLongFront10)
		.value("LONGFRONT11", QCInterestRateLeg::qcLongFront11)
		.value("LONGFRONT12", QCInterestRateLeg::qcLongFront12)
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
		.def("get_string", &qf::Tenor::getString)
		.def("__str__", &qf::Tenor::getString)
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
		.def("build_bullet_fixed_rate_leg_2", &qf::LegFactory::buildBulletFixedRateLeg2)
		.staticmethod("build_bullet_fixed_rate_leg_2")
		.def("build_bullet_fixed_rate_mccy_leg", &qf::LegFactory::buildBulletFixedRateMultiCurrencyLeg)
		.staticmethod("build_bullet_fixed_rate_mccy_leg")
		.def("build_custom_amort_fixed_rate_leg", &qf::LegFactory::buildCustomAmortFixedRateLeg)
		.staticmethod("build_custom_amort_fixed_rate_leg")
		.def("build_custom_amort_fixed_rate_leg_2", &qf::LegFactory::buildCustomAmortFixedRateLeg2)
		.staticmethod("build_custom_amort_fixed_rate_leg_2")
		.def("build_bullet_ibor_leg", &qf::LegFactory::buildBulletIborLeg)
		.staticmethod("build_bullet_ibor_leg")
		.def("build_bullet_ibor_mccy_leg", &qf::LegFactory::buildBulletIborMultiCurrencyLeg)
		.staticmethod("build_bullet_ibor_mccy_leg")
		.def("build_custom_amort_ibor_leg", &qf::LegFactory::buildCustomAmortIborLeg)
		.staticmethod("build_custom_amort_ibor_leg")
		.def("build_bullet_icp_clp_leg", &qf::LegFactory::buildBulletIcpClpLeg)
		.staticmethod("build_bullet_icp_clp_leg")
		.def("build_custom_amort_icp_clp_leg", &qf::LegFactory::buildCustomAmortIcpClpLeg)
		.staticmethod("build_custom_amort_icp_clp_leg")
		.def("build_bullet_icp_clf_leg", &qf::LegFactory::buildBulletIcpClfLeg)
		.staticmethod("build_bullet_icp_clf_leg")
		.def("build_custom_amort_icp_clf_leg", &qf::LegFactory::buildCustomAmortIcpClfLeg)
		.staticmethod("build_custom_amort_icp_clf_leg")
		;

	class_<qf::FXRate, std::shared_ptr<qf::FXRate>>
		("FXRate", init<std::shared_ptr<QCCurrency>, std::shared_ptr<QCCurrency>>())
		.def("get_code", &qf::FXRate::getCode)
		;

	def("get_fx_rate_from_code", &qf::getFXRateFromCode);

	class_<qf::FXRateIndex, std::shared_ptr<qf::FXRateIndex>, bases<qf::FinancialIndex>>
		("FXRateIndex", init<std::shared_ptr<qf::FXRate>, std::string, qf::Tenor, qf::Tenor, QCBusinessCalendar>())
		.def("fixing_date", &qf::FXRateIndex::fixingDate)
		.def("value_date", &qf::FXRateIndex::valueDate)
		.def("convert", &qf::FXRateIndex::convert)
		;

	def("get_fx_rate_index_from_fx_rate_code", &qf::getFXRateIndexFromFXRateCode);

	class_<std::vector<long>>("long_vec")
		.def(vector_indexing_suite<std::vector<long>>())
		;

	class_<qf::TimeSeries>("time_series")
		.def(map_indexing_suite<qf::TimeSeries>())
		;

	class_<std::pair<long, double> >("CurvePoint")
		.def_readwrite("tenor", &std::pair<long, double>::first)
		.def_readwrite("value", &std::pair<long, double>::second);

	class_<QCCurve<long>, std::shared_ptr<QCCurve<long>>>
		("QCCurve", init <std::vector<long>&, std::vector<double>&>())
		.def("reset", &QCCurve<long>::reset)
		.def("set_pair", &QCCurve<long>::setPair)
		.def("set_ordinate_at_with_value", &QCCurve<long>::setOrdinateAtWithValue)
		.def("set_abscissa_at_with_value", &QCCurve<long>::setAbscissaAtWithValue)
		.def("get_values_at", &QCCurve<long>::getValuesAt)
		.def("get_length", &QCCurve<long>::getLength)
		;

	class_<wrappers::QCInterpolatorWrap, boost::noncopyable>("QCInterpolator")
		.def("interpolate_at", pure_virtual(&QCInterpolator::interpolateAt))
		.def("derivative_at", pure_virtual(&QCInterpolator::derivativeAt))
		.def("second_derivative_at", pure_virtual(&QCInterpolator::secondDerivativeAt))
		;

	implicitly_convertible<std::shared_ptr<QCLinearInterpolator>, shared_ptr<QCInterpolator>>();
	class_<QCLinearInterpolator, std::shared_ptr<QCLinearInterpolator>, bases<QCInterpolator>>
		("QCLinearInterpolator", init<shared_ptr<QCCurve<long>>>());

	class_<wrappers::InterestRateCurveWrap, std::shared_ptr<wrappers::InterestRateCurveWrap>, boost::noncopyable>
		("InterestRateCurve", init<shared_ptr<QCInterpolator>, QCInterestRate>())
		.def("get_rate_at", pure_virtual(&qf::InterestRateCurve::getRateAt))
		.def("get_qc_interest_rate_at", pure_virtual(&qf::InterestRateCurve::getQCInterestRateAt))
		.def("get_discount_factor_at", pure_virtual(&qf::InterestRateCurve::getDiscountFactorAt))
		.def("get_forward_rate_with_rate", pure_virtual(&qf::InterestRateCurve::getForwardRateWithRate))
		.def("get_forward_rate", pure_virtual(&qf::InterestRateCurve::getForwardRate))
		.def("get_forward_wf", pure_virtual(&qf::InterestRateCurve::getForwardWf))
		.def("df_derivative_at", pure_virtual(&qf::InterestRateCurve::dfDerivativeAt))
		.def("fwd_wf_derivative_at", pure_virtual(&qf::InterestRateCurve::fwdWfDerivativeAt))
		;
	
	implicitly_convertible<std::shared_ptr<qf::ZeroCouponCurve>, shared_ptr<qf::InterestRateCurve>>();
	class_<qf::ZeroCouponCurve, std::shared_ptr<qf::ZeroCouponCurve>, bases<qf::InterestRateCurve>>
		("ZeroCouponCurve", init<shared_ptr<QCInterpolator>, QCInterestRate>());

	double (qf::PresentValue::*pv1)(QCDate&, const std::shared_ptr<qf::Cashflow>&, QCInterestRate&) = &qf::PresentValue::pv;
	double (qf::PresentValue::*pv2)(QCDate&, const std::shared_ptr<qf::Cashflow>&,
		const std::shared_ptr<qf::InterestRateCurve>&) = &qf::PresentValue::pv;
	double (qf::PresentValue::*pv3)(QCDate&, qf::Leg&, QCInterestRate&) = &qf::PresentValue::pv;
	double (qf::PresentValue::*pv4)(QCDate&, qf::Leg&, const std::shared_ptr<qf::InterestRateCurve>&) = &qf::
		PresentValue::pv;
	class_<qf::PresentValue>("PresentValue")
		.def("pv", pv1)
		.def("pv", pv2)
		.def("pv", pv3)
		.def("pv", pv4)
		.def("get_derivative", &qf::PresentValue::getDerivative)
		.def("get_derivatives", &qf::PresentValue::getDerivatives)
		.def("get_rate", &qf::PresentValue::getRate)
		;

	class_<qf::FixedRateBond, std::shared_ptr<qf::FixedRateBond>>
		("FixedRateBond", init<qf::Leg&>())
		.def("present_value", &qf::FixedRateBond::presentValue)
		.def("price", &qf::FixedRateBond::price)
		.def("accrued_interest", &qf::FixedRateBond::accruedInterest)
		.def("duracion", &qf::FixedRateBond::duration)
		.def("convexidad", &qf::FixedRateBond::convexity)
		.def("get_leg", &qf::FixedRateBond::getLeg)
		;

	implicitly_convertible<std::shared_ptr<qf::ChileanFixedRateBond>, shared_ptr<qf::FixedRateBond>>();
	class_<qf::ChileanFixedRateBond, std::shared_ptr<qf::ChileanFixedRateBond>, bases<qf::FixedRateBond>>
		("ChileanFixedRateBond", init<qf::Leg&, const QCInterestRate&>())
		.def("valor_par", &qf::ChileanFixedRateBond::valorPar)
		.def("precio", &qf::ChileanFixedRateBond::price)
		.def("valor_pago", &qf::ChileanFixedRateBond::settlementValue)
		;

	class_<qf::ForwardRates>("ForwardRates")
		.def("set_rate_ibor_cashflow", &qf::ForwardRates::setRateIborCashflow)
		.def("set_rate_icp_clp_cashflow", &qf::ForwardRates::setRateIcpClpCashflow)
		.def("set_rates_icp_clp_leg", &qf::ForwardRates::setRatesIcpClpLeg)
		.def("set_rates_ibor_leg", &qf::ForwardRates::setRatesIborLeg)
		;

};
