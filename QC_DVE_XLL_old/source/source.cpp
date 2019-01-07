
#include<cppinterface.h>
#include <algorithm>
#include <string>
#include <utility>
#include <memory>

#pragma warning (disable : 4996)

#include "QCTest.h"
#include "QCDate.h"
#include "QCAct360.h"
#include "QCAct365.h"
#include "QC30360.h"
#include "QCActAct.h"
#include "QCLinearWf.h"
#include "QCCompoundWf.h"
#include "QCContinousWf.h"
#include "QCInterestRate.h"
#include "QCInterestRateCurve.h"
#include "QCInterestRateLeg.h"
#include "QCFixedRatePayoff.h"
#include "QCFloatingRatePayoff.h"
#include "QCIcpClpPayoff.h"
#include "QCIcpClfPayoff.h"
#include "QCTimeDepositPayoff.h"
#include "QCCurve.h"
#include "QCInterpolator.h"
#include "QCLinearInterpolator.h"
#include "QCLogLinearInterpolator.h"
#include "QCZeroCouponInterestRateCurve.h"
#include "QCProjectingInterestRateCurve.h"
#include "QCFactoryFunctions.h"
#include "QCHelperFunctions.h"
#include "QCInterestRatePeriodsFactory.h"
#include "HelperFunctions.h"
#include "QCZeroCurveBootstrappingFromRatesAndFixedLegs.h"
#include "QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs.h"
#include "QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs.h"

using namespace std;

#define BASIS_POINT .0001
const string LICENSE_MSG = "Creasys Front Desk: producto no licenciado.";

CellMatrix QCBootZeroRatesFixedLegs(int xlValueDate,
	CellMatrix xlInputRates,
	CellMatrix xlInputFixedLegs,
	CellMatrix calendar,
	string interpolator,
	string curveWf,
	string curveYf)
{
	if (!HelperFunctions::checkAuthKey())
		throw runtime_error(LICENSE_MSG);

	//Define fecha
	QCDate valueDate{ xlValueDate };

	//Define calendario
	vector<QCDate> scl;
	unsigned int cuantasFechas = calendar.RowsInStructure();
	scl.resize(cuantasFechas);
	for (unsigned int i = 0; i < cuantasFechas; ++i)
	{
		scl.at(i) = QCDate{ (long)calendar(i, 0).NumericValue() };
	}

	unsigned int cuantasTasas = xlInputRates.RowsInStructure();
	vector<shared_ptr<QCTimeDepositPayoff>> inputRates;
	inputRates.resize(cuantasTasas);

	unsigned int cuantosSwaps = xlInputFixedLegs.RowsInStructure();
	vector<shared_ptr<QCFixedRatePayoff>> inputFixedLegs;
	inputFixedLegs.resize(cuantosSwaps);

	vector<long> tenors;
	tenors.resize(cuantasTasas + cuantosSwaps);
	for (unsigned int i = 0; i < cuantasTasas; ++i)
	{
		QCDate fecha{ (long)xlInputRates(i, 1).NumericValue() };
		tenors.at(i) = valueDate.dayDiff(fecha);
	}

	for (unsigned int i = cuantasTasas; i < cuantasTasas + cuantosSwaps; ++i)
	{
		QCDate fecha{ (long)xlInputFixedLegs(i - cuantasTasas, 2).NumericValue() };
		tenors.at(i) = valueDate.dayDiff(fecha);
	}

	vector<double> rates;
	rates.resize(cuantasTasas + cuantosSwaps);
	for (unsigned int i = 0; i < cuantasTasas + cuantosSwaps; ++i)
	{
		rates.at(i) = 0.0;
	}
	
	QCHelperFunctions::lowerCase(curveYf);
	QCHelperFunctions::lowerCase(curveWf);

	QCZrCpnCrvShrdPtr curve = QCFactoryFunctions::zrCpnCrvShrdPtr(tenors, rates, interpolator,
		curveYf, curveWf);

	for (unsigned int i = 0; i < cuantasTasas; ++i)
	{
		QCInterestRateLeg tdLeg = QCFactoryFunctions::buildTimeDepositLeg(
			"R",
			QCDate{ (long)xlInputRates(i, 0).NumericValue() },
			QCDate{ (long)xlInputRates(i, 1).NumericValue() },
			1.0);
		string yf{ xlInputRates(i, 3).StringValue() };
		string wf{ xlInputRates(i, 4).StringValue() };
		QCHelperFunctions::lowerCase(yf);
		QCHelperFunctions::lowerCase(wf);
		QCIntrstRtShrdPtr intRate = QCFactoryFunctions::intRateSharedPtr(
			xlInputRates(i, 2).NumericValue(), yf, wf);
		QCTimeDepositPayoff tdPayoff{ intRate, make_shared<QCInterestRateLeg>(tdLeg), valueDate, curve };
		inputRates.at(i) = make_shared<QCTimeDepositPayoff>(tdPayoff);
	}

	vector<tuple<QCDate, double, double>> amortIfCustom;
	for (unsigned int i = 0; i < cuantosSwaps; ++i)
	{
		//Se construye el interest rate
		string wf = xlInputFixedLegs(i, 11).StringValue();
		QCHelperFunctions::lowerCase(wf);
		string yf = xlInputFixedLegs(i, 12).StringValue();
		QCHelperFunctions::lowerCase(yf);
		shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
			xlInputFixedLegs(i, 9).NumericValue(), yf, wf);
		
		//buildFixedRateLeg
		//Este pedazo de código debe quedar en una HelperFunction de QC_DVE_XLL
		QCInterestRateLeg tmpFixedLeg = QCFactoryFunctions::buildFixedRateLeg2(
			"R",			//receive or pay
			QCDate{ (long)xlInputFixedLegs(i, 1).NumericValue() },			//start date
			QCDate{ (long)xlInputFixedLegs(i, 2).NumericValue() },			//end date
			scl,	//settlement calendar
			(int)xlInputFixedLegs(i, 4).NumericValue(),			//settlement lag
			QCHelperFunctions::stringToQCStubPeriod(xlInputFixedLegs(i, 5).StringValue()),	//stub period
			xlInputFixedLegs(i, 6).StringValue(),			//periodicity
			QCHelperFunctions::stringToQCBusDayAdjRule(xlInputFixedLegs(i, 7).StringValue()),//end date adjustment
			QCHelperFunctions::stringToQCAmortization(xlInputFixedLegs(i, 8).StringValue()),	//amortization
			amortIfCustom, //amortization and notional by date
			1.0);			//notional

		//buildFixedRatePayoff
		shared_ptr<QCFixedRatePayoff> tmpIntRatePayoff = shared_ptr<QCFixedRatePayoff>(
			new QCFixedRatePayoff{ tmpIntRate, make_shared<QCInterestRateLeg>(tmpFixedLeg), curve, valueDate,
			nullptr });

		//Se agrega el payoff al vector de fixed legs
		inputFixedLegs.at(i) = tmpIntRatePayoff;
	}

	//Se da de alta el objeto bootstrapeador
	QCZeroCurveBootstrappingFromRatesAndFixedLegs boot{ valueDate, inputRates, inputFixedLegs, curve };

	boot.generateCurveAndDerivatives();
	 
	CellMatrix result(cuantasTasas + cuantosSwaps, cuantasTasas + cuantosSwaps + 2);
	for (size_t i = 0; i < cuantasTasas + cuantosSwaps; ++i)
	{
		pair<long, double> temp = curve->getValuesAt(i);
		result(i, 0) = temp.first;
		result(i, 1) = temp.second;
		for (size_t j = 0; j < cuantasTasas + cuantosSwaps; ++j)
		{
			result(i, 2 + j) = boot.getDerivativeAt(i, j);
		}
	}
	return result;

}

CellMatrix QCBootZeroRatesFwdsFixedLegs(double xlValueDate,
	double fx,
	CellMatrix xlInputRates,
	CellMatrix xlInputForwards,
	int whichLeg,
	CellMatrix xlInputFixedLegs,
	string curveInterpolator,
	string curveYf,
	string curveWf,
	CellMatrix holidays,
	CellMatrix auxCurve,
	string auxCurveInterpolator,
	string auxCurveYf,
	string auxCurveWf)
{
	if (!HelperFunctions::checkAuthKey())
		throw runtime_error(LICENSE_MSG);

	try
	{
		//Se construye la fecha de proceso
		QCDate valueDate{ static_cast<long>(xlValueDate) };

		//Se construye el vector con las fechas de los feriados
		vector<QCDate> dateVector;
		HelperFunctions::buildVectorHolidays(holidays, dateVector);

		//Se construye el objeto curva auxiliar
		size_t puntosAuxCurva = auxCurve.RowsInStructure();
		vector<long> auxTenors;
		auxTenors.resize(puntosAuxCurva);
		vector<double> auxRates;
		auxRates.resize(puntosAuxCurva);
		for (size_t i = 0; i < puntosAuxCurva; ++i)
		{
			auxTenors.at(i) = static_cast<long>(auxCurve(i, 0).NumericValue());
			auxRates.at(i) = auxCurve(i, 1).NumericValue();
		}

		QCHelperFunctions::lowerCase(auxCurveInterpolator);
		QCHelperFunctions::lowerCase(auxCurveYf);
		QCHelperFunctions::lowerCase(auxCurveWf);
		auto auxZrCrvPtr = QCFactoryFunctions::zrCpnCrvShrdPtr(auxTenors,
			auxRates, auxCurveInterpolator, auxCurveYf, auxCurveWf);

		//Se construye un vector con los datos de las tasas cero iniciales
		vector<HelperFunctions::ZeroRate> zeroRateVector;
		HelperFunctions::buildZeroRateVector(xlInputRates, zeroRateVector);
		cout << "\tboot_zero_rates_fwds_fixed_legs: finished zero rates." << endl;

		//Se construye un vector con los datos de los fwds iniciales
		vector<HelperFunctions::FwdIndex> fwdIndexVector;
		HelperFunctions::buildFwdVector(xlInputForwards, fwdIndexVector);
		cout << "\tboot_zero_rates_fwds_fixed_legs: finished fx fwds." << endl;

		//Se construye un vector con las patas fijas iniciales
		vector<HelperFunctions::SwapIndex> swapIndexVector;
		HelperFunctions::buildFixedRateIndexVector(xlInputFixedLegs, valueDate, dateVector, swapIndexVector);
		cout << "\tboot_zero_rates_fwds_fixed_legs: finished fixed rate legs." << endl;

		size_t cuantasTasas = zeroRateVector.size();
		vector<shared_ptr<QCTimeDepositPayoff>> inputRates;
		inputRates.resize(cuantasTasas);
		cout << "\tboot_zero_rates_fwds_fixed_legs: inputRates resized" << endl;

		size_t cuantosFwds = fwdIndexVector.size();
		vector<shared_ptr<QCFXForward>> inputForwards;
		inputForwards.resize(cuantosFwds);
		cout << "\tboot_zero_rates_fwds_fixed_legs: inputForwards resized" << endl;

		size_t cuantosSwaps = swapIndexVector.size();
		vector<shared_ptr<QCFixedRatePayoff>> inputFixedLegs;
		inputFixedLegs.resize(cuantosSwaps);
		cout << "\tboot_zero_rates_fwds_fixed_legs: inputFixedLegs resized" << endl;

		vector<long> tenors;
		tenors.resize(cuantasTasas + cuantosFwds + cuantosSwaps);
		for (size_t i = 0; i < cuantasTasas; ++i)
		{
			tenors.at(i) = valueDate.dayDiff(get<1>(zeroRateVector.at(i)));
		}

		for (size_t i = cuantasTasas; i < cuantasTasas + cuantosFwds; ++i)
		{
			tenors.at(i) = valueDate.dayDiff(get<2>(fwdIndexVector.at(i - cuantasTasas)));
		}

		for (size_t i = cuantasTasas + cuantosFwds; i < cuantasTasas + cuantosFwds + cuantosSwaps; ++i)
		{
			tenors.at(i) = valueDate.dayDiff(get<2>(swapIndexVector.at(i - cuantasTasas - cuantosFwds)));
		}
		cout << "\tboot_zero_rates_fwds_fixed_legs: curve tenors ok" << endl;

		vector<double> rates;
		rates.resize(cuantasTasas + cuantosFwds + cuantosSwaps);
		for (size_t i = 0; i < cuantasTasas + cuantosFwds + cuantosSwaps; ++i)
		{
			rates.at(i) = 0.0;
		}
		cout << "\tboot_zero_rates_fwds_fixed_legs: curve rates ok" << endl;

		QCHelperFunctions::lowerCase(curveInterpolator);
		QCHelperFunctions::lowerCase(curveYf);
		QCHelperFunctions::lowerCase(curveWf);
		QCZrCpnCrvShrdPtr curve = QCFactoryFunctions::zrCpnCrvShrdPtr(tenors, rates, curveInterpolator,
			curveYf, curveWf);
		cout << "\tboot_zero_rates_fwds_fixed_legs: curve initialized" << endl;

		for (unsigned int i = 0; i < cuantasTasas; ++i)
		{
			QCInterestRateLeg tdLeg = QCFactoryFunctions::buildTimeDepositLeg(
				"R",
				get<0>(zeroRateVector.at(i)),
				get<1>(zeroRateVector.at(i)),
				1.0);
			cout << "\tboot_zero_rates_fixed_legs: time deposit leg " << i << " initialized" << endl;
			QCIntrstRtShrdPtr intRate = QCFactoryFunctions::intRateSharedPtr(
				get<2>(zeroRateVector.at(i)),
				get<3>(zeroRateVector.at(i)),
				get<4>(zeroRateVector.at(i)));
			cout << "\tboot_zero_rates_fixed_legs: interest rate " << i << " initialized" << endl;
			QCTimeDepositPayoff tdPayoff{ intRate, make_shared<QCInterestRateLeg>(tdLeg), valueDate, curve };
			inputRates.at(i) = make_shared<QCTimeDepositPayoff>(tdPayoff);
			cout << "\tboot_zero_rates_fixed_legs: time deposit payoff " << i << " initialized" << endl;

		}

		map<QCCurrencyConverter::QCFxRateEnum, double> fxRate;
		vector<shared_ptr<QCDiscountBondPayoff>> legs;
		shared_ptr<map<QCDate, double>> fixings;
		legs.resize(2);
		fxRate.insert(pair<QCCurrencyConverter::QCFxRateEnum, double>(get<6>(fwdIndexVector.at(0)), fx));
		fxRate.insert(pair<QCCurrencyConverter::QCFxRateEnum, double>(get<7>(fwdIndexVector.at(0)), 1.0));
		for (unsigned int i = 0; i < cuantosFwds; ++i)
		{
			//strong currency
			QCInterestRateLeg dbLeg0 = QCFactoryFunctions::buildDiscountBondLeg(
				"R", valueDate, get<2>(fwdIndexVector.at(i)),
				0, 0, dateVector, dateVector, get<0>(fwdIndexVector.at(i)));

			//weak currency
			QCInterestRateLeg dbLeg1 = QCFactoryFunctions::buildDiscountBondLeg(
				"P", valueDate, get<2>(fwdIndexVector.at(i)),
				0, 0, dateVector, dateVector, get<1>(fwdIndexVector.at(i)));

			//En este bloque estamos asumiendo que la curva a construir es la de la
			//moneda fuerte del par de los fwds. Hay que insertar un if para distinguir
			//si es asi o corresponde a la moneda debil.
			auto dbPayoff0 = make_shared<QCDiscountBondPayoff>(
				make_shared<QCInterestRateLeg>(dbLeg0),
				curve, valueDate, fixings,
				get<4>(fwdIndexVector.at(i)), get<6>(fwdIndexVector.at(i)));
			legs.at(0) = dbPayoff0;

			auto dbPayoff1 = make_shared<QCDiscountBondPayoff>(
				make_shared<QCInterestRateLeg>(dbLeg1),
				auxZrCrvPtr, valueDate, fixings,
				get<5>(fwdIndexVector.at(i)), get<7>(fwdIndexVector.at(i)));
			legs.at(1) = dbPayoff1;

			auto fxFwd = make_shared<QCFXForward>(legs, get<3>(fwdIndexVector.at(i)),
				make_shared<map<QCCurrencyConverter::QCFxRateEnum, double>>(fxRate));
			cout << "\tboot_zero_rates_fwds_fixed_legs: Fx Forward " << i << " initialized" << endl;

			inputForwards.at(i) = fxFwd;
		}


		vector<tuple<QCDate, double, double>> amortIfCustom;
		for (unsigned int i = 0; i < cuantosSwaps; ++i)
		{
			//Se construye el interest rate
			shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
				get<8>(swapIndexVector.at(i)),
				get<10>(swapIndexVector.at(i)),
				get<11>(swapIndexVector.at(i)));
			cout << "\tboot_zero_rates_fixed_legs: swap interest rate " << i << " initialized" << endl;

			//buildFixedRateLeg
			//Este pedazo de código debe quedar en una HelperFunction de QC_DVE_XLL
			QCInterestRateLeg tmpFixedLeg = QCFactoryFunctions::buildFixedRateLeg2(
				get<0>(swapIndexVector.at(i)),			//receive or pay
				get<1>(swapIndexVector.at(i)),			//start date
				get<2>(swapIndexVector.at(i)),			//end date
				dateVector,								//settlement calendar
				get<3>(swapIndexVector.at(i)),			//settlement lag
				get<4>(swapIndexVector.at(i)),			//stub period
				get<5>(swapIndexVector.at(i)),			//periodicity
				get<6>(swapIndexVector.at(i)),			//end date adjustment
				get<7>(swapIndexVector.at(i)),			//amortization
				amortIfCustom,							//amortization and notional by date
				get<9>(swapIndexVector.at(i)));			//notional
			cout << "\tboot_zero_rates_fixed_legs: fixed rate interest leg " << i << " initialized" << endl;

			//buildFixedRatePayoff
			shared_ptr<QCFixedRatePayoff> tmpIntRatePayoff = shared_ptr<QCFixedRatePayoff>(
				new QCFixedRatePayoff{ tmpIntRate, make_shared<QCInterestRateLeg>(tmpFixedLeg), curve, valueDate,
				nullptr });
			cout << "\tboot_zero_rates_fixed_legs: fixed rate interest payoff " << i << " initialized" << endl;

			//Se agrega el payoff al vector de fixed legs
			inputFixedLegs.at(i) = tmpIntRatePayoff;
		}

		//Se da de alta el objeto bootstrapeador
		QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs boot{ valueDate, inputRates,
			inputForwards, 0, inputFixedLegs, curve };
		cout << "\tboot_zero_rates_fwds_fixed_legs: bootstrapping object initialized" << endl;

		boot.generateCurveAndDerivatives();
		cout << "\tboot_zero_rates_fwds_fixed_legs: curve bootstrapped" << endl;

		vector<tuple<long, double, vector<double>>> result; //plazo, tasa, derivadas
		size_t tamagno = curve->getLength();
		result.resize(tamagno);
		for (size_t i = 0; i < tamagno; ++i)
		{
			get<2>(result.at(i)).resize(tamagno);
			pair<long, double> temp = curve->getValuesAt(i);
			get<0>(result.at(i)) = temp.first;
			get<1>(result.at(i)) = temp.second;
			for (size_t j = 0; j < tamagno; ++j)
			{
				get<2>(result.at(i)).at(j) = boot.getDerivativeAt(i, j);
			}
		}
		cout << "\tboot_zero_rates_fwds_fixed_legs: results obtained" << endl;

		CellMatrix curveAndDeltas(tamagno, tamagno + 2);
		for (size_t i = 0; i < tamagno; ++i)
		{
			curveAndDeltas(i, 0) = get<0>(result.at(i));
			curveAndDeltas(i, 1) = get<1>(result.at(i));
			for (size_t j = 0; j < tamagno; ++j)
			{
				curveAndDeltas(i, j + 2) = get<2>(result.at(i)).at(j);
			}
		}
		cout << "\tboot_zero_rates_fwds_fixed_legs: output prepared" << endl;

		return curveAndDeltas;

	}
	catch (exception& e)
	{
		string msg = "Error en el bootstrapping Rates&Fwds&Fixed_Legs. " + string(e.what());
		throw runtime_error(msg);
	}
}

CellMatrix QCBootZeroRatesFwdsFloatingLegs(double xlValueDate,
	double fx,
	double fixing,
	CellMatrix xlInputRates,
	CellMatrix xlInputForwards,
	string fwdPoints,
	string fwdsHolidays,
	int whichLeg,
	CellMatrix xlInputFloatingLegs,
	CellMatrix xlInputIndexChars,
	string curveInterpolator,
	string curveYf,
	string curveWf,
	CellMatrix holidays,
	CellMatrix fwdsCurve,
	string fwdsCurveInterpolator,
	string fwdsCurveYf,
	string fwdsCurveWf,
	CellMatrix floatCurve,
	string floatCurveInterpolator,
	string floatCurveYf,
	string floatCurveWf)
{
	if (!HelperFunctions::checkAuthKey())
		throw runtime_error(LICENSE_MSG);
	try
	{
		//Se construye la fecha de proceso
		QCDate valueDate{ static_cast<long>(xlValueDate) };

		//Se construye el vector con las fechas de los feriados
		map<string, vector<QCDate>> holidayMap;
		HelperFunctions::buildHolidays(holidays, holidayMap);

		//Vector de fixings con el fixing inicial de las patas flotantes
		map<QCDate, double> tsFixing;
		tsFixing.insert(pair<QCDate, double>(valueDate, fixing));

		//Se construye el objeto curva auxiliar de los fwds
		size_t puntosFwdsCurva = fwdsCurve.RowsInStructure();
		vector<long> fwdsTenors;
		fwdsTenors.resize(puntosFwdsCurva);
		vector<double> fwdsRates;
		fwdsRates.resize(puntosFwdsCurva);
		for (size_t i = 0; i < puntosFwdsCurva; ++i)
		{
			fwdsTenors.at(i) = static_cast<long>(fwdsCurve(i, 0).NumericValue());
			fwdsRates.at(i) = fwdsCurve(i, 1).NumericValue();
		}

		QCHelperFunctions::lowerCase(fwdsCurveInterpolator);
		QCHelperFunctions::lowerCase(fwdsCurveYf);
		QCHelperFunctions::lowerCase(fwdsCurveWf);
		auto fwdZrCrvPtr = QCFactoryFunctions::zrCpnCrvShrdPtr(fwdsTenors,
			fwdsRates, fwdsCurveInterpolator, fwdsCurveYf, fwdsCurveWf);

		//Se construye el objeto curva auxiliar de las patas flotantes
		size_t puntosFloatCurva = floatCurve.RowsInStructure();
		vector<long> floatTenors;
		floatTenors.resize(puntosFloatCurva);
		vector<double> floatRates;
		floatRates.resize(puntosFloatCurva);
		for (size_t i = 0; i < puntosFloatCurva; ++i)
		{
			floatTenors.at(i) = static_cast<long>(floatCurve(i, 0).NumericValue());
			floatRates.at(i) = floatCurve(i, 1).NumericValue();
		}

		QCHelperFunctions::lowerCase(floatCurveInterpolator);
		QCHelperFunctions::lowerCase(floatCurveYf);
		QCHelperFunctions::lowerCase(floatCurveWf);
		auto floatZrCrvPtr = QCFactoryFunctions::zrCpnCrvShrdPtr(floatTenors,
			floatRates, floatCurveInterpolator, floatCurveYf, floatCurveWf);


		//Se construye un vector con los datos de las tasas cero iniciales
		vector<HelperFunctions::ZeroRate> zeroRateVector;
		HelperFunctions::buildZeroRateVector(xlInputRates, zeroRateVector);

		//Se construye un vector con los datos de los fwds iniciales
		vector<HelperFunctions::FwdIndex> fwdIndexVector;
		QCHelperFunctions::lowerCase(fwdPoints);
		if (fwdPoints == "si")
		{
			HelperFunctions::buildFwdVector(xlInputForwards, fwdIndexVector, fx);
		}
		else
		{
			HelperFunctions::buildFwdVector(xlInputForwards, fwdIndexVector);
		}

		//Se construye un vector con las patas flotantes iniciales
		vector<HelperFunctions::FloatIndex> floatIndexVector;
		HelperFunctions::buildFloatingRateIndexVector(xlInputFloatingLegs, fixing, floatIndexVector);

		//Se construye un mapa con las caracteristicas del indice flotante
		map<string, pair<string, string>> mapIndexChars;
		HelperFunctions::buildStringPairStringMap(xlInputIndexChars, mapIndexChars);

		//se construye la curva que se quiere bootstrapear
		size_t cuantasTasas = zeroRateVector.size();
		vector<shared_ptr<QCTimeDepositPayoff>> inputRates;
		inputRates.resize(cuantasTasas);

		size_t cuantosFwds = fwdIndexVector.size();
		vector<shared_ptr<QCFXForward>> inputForwards;
		inputForwards.resize(cuantosFwds);

		size_t cuantosSwaps = floatIndexVector.size();
		vector<shared_ptr<QCFloatingRatePayoff>> inputFloatingLegs;
		inputFloatingLegs.resize(cuantosSwaps);

		vector<long> tenors;
		tenors.resize(cuantasTasas + cuantosFwds + cuantosSwaps);
		for (size_t i = 0; i < cuantasTasas; ++i)
		{
			tenors.at(i) = valueDate.dayDiff(get<1>(zeroRateVector.at(i)));
		}

		for (size_t i = cuantasTasas; i < cuantasTasas + cuantosFwds; ++i)
		{
			tenors.at(i) = valueDate.dayDiff(get<2>(fwdIndexVector.at(i - cuantasTasas)));
		}

		for (size_t i = cuantasTasas + cuantosFwds; i < cuantasTasas + cuantosFwds + cuantosSwaps; ++i)
		{
			tenors.at(i) = valueDate.dayDiff(get<2>(floatIndexVector.at(i - cuantasTasas - cuantosFwds)));
		}

		vector<double> rates;
		rates.resize(cuantasTasas + cuantosFwds + cuantosSwaps);
		for (size_t i = 0; i < cuantasTasas + cuantosFwds + cuantosSwaps; ++i)
		{
			rates.at(i) = 0.0;
		}

		QCHelperFunctions::lowerCase(curveInterpolator);
		QCHelperFunctions::lowerCase(curveYf);
		QCHelperFunctions::lowerCase(curveWf);
		QCZrCpnCrvShrdPtr curve = QCFactoryFunctions::zrCpnCrvShrdPtr(tenors, rates, curveInterpolator,
			curveYf, curveWf);

		//Se construyen las tasas
		for (unsigned int i = 0; i < cuantasTasas; ++i)
		{
			QCInterestRateLeg tdLeg = QCFactoryFunctions::buildTimeDepositLeg(
				"R",
				get<0>(zeroRateVector.at(i)),
				get<1>(zeroRateVector.at(i)),
				1.0);
			cout << "\tboot_zero_rates_fixed_legs: time deposit leg " << i << " initialized" << endl;
			QCIntrstRtShrdPtr intRate = QCFactoryFunctions::intRateSharedPtr(
				get<2>(zeroRateVector.at(i)),
				get<3>(zeroRateVector.at(i)),
				get<4>(zeroRateVector.at(i)));
			cout << "\tboot_zero_rates_fixed_legs: interest rate " << i << " initialized" << endl;
			QCTimeDepositPayoff tdPayoff{ intRate, make_shared<QCInterestRateLeg>(tdLeg), valueDate, curve };
			inputRates.at(i) = make_shared<QCTimeDepositPayoff>(tdPayoff);
			cout << "\tboot_zero_rates_fixed_legs: time deposit payoff " << i << " initialized" << endl;

		}

		//Se construyen los fwds
		map<QCCurrencyConverter::QCFxRateEnum, double> fxRate;
		vector<shared_ptr<QCDiscountBondPayoff>> legs;
		shared_ptr<map<QCDate, double>> fixings;
		legs.resize(2);
		fxRate.insert(pair<QCCurrencyConverter::QCFxRateEnum, double>(get<6>(fwdIndexVector.at(0)), fx));
		fxRate.insert(pair<QCCurrencyConverter::QCFxRateEnum, double>(get<7>(fwdIndexVector.at(0)), 1.0));
		for (unsigned int i = 0; i < cuantosFwds; ++i)
		{
			//strong currency
			QCInterestRateLeg dbLeg0 = QCFactoryFunctions::buildDiscountBondLeg(
				"R", valueDate, get<2>(fwdIndexVector.at(i)),
				0, 0, HelperFunctions::getHolidays(holidayMap, fwdsHolidays),
				HelperFunctions::getHolidays(holidayMap, fwdsHolidays), get<0>(fwdIndexVector.at(i)));

			//weak currency
			QCInterestRateLeg dbLeg1 = QCFactoryFunctions::buildDiscountBondLeg(
				"P", valueDate, get<2>(fwdIndexVector.at(i)),
				0, 0, HelperFunctions::getHolidays(holidayMap, fwdsHolidays),
				HelperFunctions::getHolidays(holidayMap, fwdsHolidays), get<1>(fwdIndexVector.at(i)));

			//En este bloque estamos asumiendo que la curva a construir es la de la
			//moneda fuerte del par de los fwds. Hay que insertar un if para distinguir
			//si es asi o corresponde a la moneda debil.
			auto dbPayoff0 = make_shared<QCDiscountBondPayoff>(
				make_shared<QCInterestRateLeg>(dbLeg0),
				curve, valueDate, fixings,
				get<4>(fwdIndexVector.at(i)), get<6>(fwdIndexVector.at(i)));
			legs.at(0) = dbPayoff0;

			auto dbPayoff1 = make_shared<QCDiscountBondPayoff>(
				make_shared<QCInterestRateLeg>(dbLeg1),
				fwdZrCrvPtr, valueDate, fixings,
				get<5>(fwdIndexVector.at(i)), get<7>(fwdIndexVector.at(i)));
			legs.at(1) = dbPayoff1;

			auto fxFwd = make_shared<QCFXForward>(legs, get<3>(fwdIndexVector.at(i)),
				make_shared<map<QCCurrencyConverter::QCFxRateEnum, double>>(fxRate));

			inputForwards.at(i) = fxFwd;
		}

		vector<tuple<QCDate, double, double>> amortIfCustom;
		for (unsigned int i = 0; i < cuantosSwaps; ++i)
		{
			//Se construye el interest rate
			shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
				get<HelperFunctions::QCFloatIndex::qcRate>(floatIndexVector.at(i)),
				get<HelperFunctions::QCFloatIndex::qcYearFraction>(floatIndexVector.at(i)),
				get<HelperFunctions::QCFloatIndex::qcWealthFactor>(floatIndexVector.at(i)));

			//buildFloatingRateLeg
			//Este pedazo de código debe quedar en una HelperFunction de QC_DVE_XLL
			string settCalendar = get<HelperFunctions::QCFloatIndex::qcSettlementCalendar>(floatIndexVector.at(i));
			string fixCalendar = get<HelperFunctions::QCFloatIndex::qcFixingCalendar>(floatIndexVector.at(i));
			cout << "A" << endl;
			QCInterestRateLeg tmpFloatingLeg = QCFactoryFunctions::buildFloatingRateLeg2(
				get<HelperFunctions::QCFloatIndex::qcReceivePay>(floatIndexVector.at(i)),
				get<HelperFunctions::QCFloatIndex::qcStartDate>(floatIndexVector.at(i)),
				get<HelperFunctions::QCFloatIndex::qcEndDate>(floatIndexVector.at(i)),
				HelperFunctions::getHolidays(holidayMap, settCalendar),
				get<HelperFunctions::QCFloatIndex::qcSettlementLag>(floatIndexVector.at(i)),
				get<HelperFunctions::QCFloatIndex::qcStubPeriod>(floatIndexVector.at(i)),
				get<HelperFunctions::QCFloatIndex::qcPeriodicity>(floatIndexVector.at(i)),
				get<HelperFunctions::QCFloatIndex::qcEndDateAdjustment>(floatIndexVector.at(i)),
				get<HelperFunctions::QCFloatIndex::qcAmortization>(floatIndexVector.at(i)),
				amortIfCustom,
				get<HelperFunctions::QCFloatIndex::qcFixingLag>(floatIndexVector.at(i)),
				get<HelperFunctions::QCFloatIndex::qcFixingStubPeriod>(floatIndexVector.at(i)),
				get<HelperFunctions::QCFloatIndex::qcFixingPeriodicity>(floatIndexVector.at(i)),
				HelperFunctions::getHolidays(holidayMap, fixCalendar),
				HelperFunctions::getIndexChars(mapIndexChars, get<HelperFunctions::QCFloatIndex::qcInterestRateIndex>(floatIndexVector.at(i))),
				get<HelperFunctions::QCFloatIndex::qcNotional>(floatIndexVector.at(i)));

			//buildFloatingRatePayoff
			shared_ptr<QCFloatingRatePayoff> tmpIntRatePayoff = shared_ptr<QCFloatingRatePayoff>(
				new QCFloatingRatePayoff{ tmpIntRate,
				get<HelperFunctions::QCFloatIndex::qcSpread>(floatIndexVector.at(i)), 1.0,
				make_shared<QCInterestRateLeg>(tmpFloatingLeg), floatZrCrvPtr, curve, valueDate,
				make_shared<map<QCDate, double>>(tsFixing) });

			//Se agrega el payoff al vector de fixed legs
			inputFloatingLegs.at(i) = tmpIntRatePayoff;
		}

		//Se da de alta el objeto bootstrapeador
		QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs boot{ valueDate, inputRates, inputForwards,
			0, inputFloatingLegs, curve };

		boot.generateCurveAndDerivatives();

		vector<tuple<long, double, vector<double>>> result; //plazo, tasa, derivadas
		size_t tamagno = curve->getLength();
		result.resize(tamagno);
		for (size_t i = 0; i < tamagno; ++i)
		{
			get<2>(result.at(i)).resize(tamagno);
			pair<long, double> temp = curve->getValuesAt(i);
			get<0>(result.at(i)) = temp.first;
			get<1>(result.at(i)) = temp.second;
			for (size_t j = 0; j < tamagno; ++j)
			{
				get<2>(result.at(i)).at(j) = boot.getDerivativeAt(i, j);
			}
		}

		CellMatrix curveAndDeltas(tamagno, tamagno + 2);
		for (size_t i = 0; i < tamagno; ++i)
		{
			curveAndDeltas(i, 0) = get<0>(result.at(i));
			curveAndDeltas(i, 1) = get<1>(result.at(i));
			for (size_t j = 0; j < tamagno; ++j)
			{
				curveAndDeltas(i, j + 2) = get<2>(result.at(i)).at(j);
			}
		}

		return curveAndDeltas;

	}
	catch (exception& e)
	{
		string msg = "Error en el bootstrapping Rates&Fwds&Fixed_Legs. " + string(e.what());
		throw runtime_error(msg);
	}
}


CellMatrix checkBoostrappingFwds(int xlValueDate,
	CellMatrix xlInputRates,
	CellMatrix xlInputForwards,
	int whichLeg,
	CellMatrix xlInputFixedLegs,
	CellMatrix calendar,
	CellMatrix auxCurve,
	double fx,
	string interpolator)
{
	//Define fecha
	QCDate valueDate{ xlValueDate };

	//Define calendario
	vector<QCDate> scl;
	unsigned int cuantasFechas = calendar.RowsInStructure();
	scl.resize(cuantasFechas);
	for (unsigned int i = 0; i < cuantasFechas; ++i)
	{
		scl.at(i) = QCDate{ (long)calendar(i, 0).NumericValue() };
	}

	//Vamos a construir el shared_ptr de QCZeroCouponCurve
	int puntosCurva = auxCurve.RowsInStructure();
	vector<long> auxTenors;
	auxTenors.resize(puntosCurva);
	vector<double> auxRates;
	auxRates.resize(puntosCurva);
	for (int i = 0; i < puntosCurva; ++i)
	{
		auxTenors.at(i) = static_cast<long>(auxCurve(i, 0).NumericValue());
		auxRates.at(i) = auxCurve(i, 1).NumericValue();
	}

	//definir la curva cero cupon de descuento
	QCZrCpnCrvShrdPtr auxCrvPtr = QCFactoryFunctions::zrCpnCrvShrdPtr(auxTenors,
		auxRates, "linear", "act/365", "com");

	shared_ptr<map<QCDate, double>> fixings;
	
	unsigned int cuantasTasas = xlInputRates.RowsInStructure();
	vector<shared_ptr<QCTimeDepositPayoff>> inputRates;
	inputRates.resize(cuantasTasas);

	unsigned int cuantosForwards = xlInputForwards.RowsInStructure();
	vector<shared_ptr<QCFXForward>> inputForwards;
	inputForwards.resize(cuantosForwards);

	unsigned int cuantosSwaps = xlInputFixedLegs.RowsInStructure();
	vector<shared_ptr<QCFixedRatePayoff>> inputFixedLegs;
	inputFixedLegs.resize(cuantosSwaps);

	vector<long> tenors;
	tenors.resize(cuantasTasas + cuantosForwards + cuantosSwaps);
	for (unsigned int i = 0; i < cuantasTasas; ++i)
	{
		QCDate fecha{ (long)xlInputRates(i, 1).NumericValue() };
		tenors.at(i) = valueDate.dayDiff(fecha);
	}

	for (unsigned int i = cuantasTasas; i < cuantasTasas + cuantosForwards; ++i)
	{
		QCDate fecha{ (long)xlInputForwards(i - cuantasTasas, 1).NumericValue() };
		tenors.at(i) = valueDate.dayDiff(fecha);
	}

	for (unsigned int i = cuantasTasas + cuantosForwards; i < cuantasTasas + cuantosForwards + 
		cuantosSwaps; ++i)
	{
		QCDate fecha{ (long)xlInputFixedLegs(i - cuantasTasas - cuantosForwards, 2).NumericValue() };
		tenors.at(i) = valueDate.dayDiff(fecha);
	}

	vector<double> rates;
	rates.resize(cuantasTasas + cuantosForwards + cuantosSwaps);
	for (unsigned int i = 0; i < cuantasTasas + cuantosForwards + cuantosSwaps; ++i)
	{
		rates.at(i) = 0.0;
	}

	QCZrCpnCrvShrdPtr curve = QCFactoryFunctions::zrCpnCrvShrdPtr(
		tenors, rates, "LINEAR", "act/365", "com");

	for (unsigned int i = 0; i < cuantasTasas; ++i)
	{
		QCInterestRateLeg tdLeg = QCFactoryFunctions::buildTimeDepositLeg(
			"R",
			QCDate{ (long)xlInputRates(i, 0).NumericValue() },
			QCDate{ (long)xlInputRates(i, 1).NumericValue() },
			1.0);
		string yf{ xlInputRates(i, 3).StringValue() };
		string wf{ xlInputRates(i, 4).StringValue() };
		QCHelperFunctions::lowerCase(yf);
		QCHelperFunctions::lowerCase(wf);
		QCIntrstRtShrdPtr intRate = QCFactoryFunctions::intRateSharedPtr(
			xlInputRates(i, 2).NumericValue(), yf, wf);
		QCTimeDepositPayoff tdPayoff{ intRate, make_shared<QCInterestRateLeg>(tdLeg), valueDate, curve };
		inputRates.at(i) = make_shared<QCTimeDepositPayoff>(tdPayoff);
	}

	map<QCCurrencyConverter::QCFxRateEnum, double> fxRate;
	vector<shared_ptr<QCDiscountBondPayoff>> legs;
	legs.resize(2);
	fxRate.insert(pair<QCCurrencyConverter::QCFxRateEnum, double>(QCCurrencyConverter::qcCLFCLP, fx));
	fxRate.insert(pair<QCCurrencyConverter::QCFxRateEnum, double>(QCCurrencyConverter::qcCLPCLP, 1.0));
	for (unsigned int i = 0; i < cuantosForwards; ++i)
	{
		QCInterestRateLeg dbLeg0 = QCFactoryFunctions::buildDiscountBondLeg(
			"R",
			QCDate{ (long)xlInputForwards(i, 0).NumericValue() },
			QCDate{ (long)xlInputForwards(i, 1).NumericValue() },
			0, 0, scl, scl, 1.0); //Esta esta en UF
		
		QCInterestRateLeg dbLeg1 = QCFactoryFunctions::buildDiscountBondLeg(
			"P",
			QCDate{ (long)xlInputForwards(i, 0).NumericValue() },
			QCDate{ (long)xlInputForwards(i, 1).NumericValue() },
			0, 0, scl, scl, xlInputForwards(i, 3).NumericValue());
		
		auto dbPayoff0 = make_shared<QCDiscountBondPayoff>(
			make_shared<QCInterestRateLeg>(dbLeg0),
			curve, valueDate, fixings,
			QCCurrencyConverter::qcCLF, QCCurrencyConverter::qcCLFCLP);
		legs.at(0) = dbPayoff0;
		
		auto dbPayoff1 = make_shared<QCDiscountBondPayoff>(
			make_shared<QCInterestRateLeg>(dbLeg1),
			auxCrvPtr, valueDate, fixings,
			QCCurrencyConverter::qcCLP, QCCurrencyConverter::qcCLPCLP);
		legs.at(1) = dbPayoff1;
		
		auto fxFwd = make_shared<QCFXForward>(legs, QCCurrencyConverter::qcCLP,
			make_shared<map<QCCurrencyConverter::QCFxRateEnum, double>>(fxRate));
		
		inputForwards.at(i) = fxFwd;
	}

	vector<tuple<QCDate, double, double>> amortIfCustom;
	for (unsigned int i = 0; i < cuantosSwaps; ++i)
	{
		//Se construye el interest rate
		string wf = xlInputFixedLegs(i, 11).StringValue();
		QCHelperFunctions::lowerCase(wf);
		string yf = xlInputFixedLegs(i, 12).StringValue();
		QCHelperFunctions::lowerCase(yf);
		shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
			xlInputFixedLegs(i, 9).NumericValue(), yf, wf);

		//buildFixedRateLeg
		//Este pedazo de código debe quedar en una HelperFunction de QC_DVE_XLL
		QCInterestRateLeg tmpFixedLeg = QCFactoryFunctions::buildFixedRateLeg2(
			"R",			//receive or pay
			QCDate{ (long)xlInputFixedLegs(i, 1).NumericValue() },			//start date
			QCDate{ (long)xlInputFixedLegs(i, 2).NumericValue() },			//end date
			scl,	//settlement calendar
			(int)xlInputFixedLegs(i, 4).NumericValue(),			//settlement lag
			QCHelperFunctions::stringToQCStubPeriod(xlInputFixedLegs(i, 5).StringValue()),	//stub period
			xlInputFixedLegs(i, 6).StringValue(),			//periodicity
			QCHelperFunctions::stringToQCBusDayAdjRule(xlInputFixedLegs(i, 7).StringValue()),//end date adjustment
			QCHelperFunctions::stringToQCAmortization(xlInputFixedLegs(i, 8).StringValue()),	//amortization
			amortIfCustom, //amortization and notional by date
			1.0);			//notional

		//buildFixedRatePayoff
		shared_ptr<QCFixedRatePayoff> tmpIntRatePayoff = shared_ptr<QCFixedRatePayoff>(
			new QCFixedRatePayoff{ tmpIntRate, make_shared<QCInterestRateLeg>(tmpFixedLeg), curve, valueDate,
			nullptr });

		//Se agrega el payoff al vector de fixed legs
		inputFixedLegs.at(i) = tmpIntRatePayoff;
	}

	//Tengo que dar de alta el objeto bootstrapeador
	QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs boot{ valueDate, inputRates,
		inputForwards, (unsigned int)whichLeg, inputFixedLegs, curve };
	
	boot.generateCurve();

	CellMatrix result(cuantasTasas + cuantosForwards + cuantosSwaps, 2);
	for (unsigned int i = 0; i < cuantasTasas + cuantosForwards + cuantosSwaps; ++i)
	{
		pair<long, double> temp = curve->getValuesAt(i);
		result(i, 0) = temp.first;
		result(i, 1) = temp.second;
	}
	return result;

}

CellMatrix checkBoostrappingFwdsFloating(int xlValueDate,
	CellMatrix xlInputRates,
	CellMatrix xlInputForwards,
	int whichLeg,
	CellMatrix xlInputFloatingLegs,
	CellMatrix holidays,
	CellMatrix intRateIndexChars,
	CellMatrix auxCurveFwd,
	CellMatrix auxCurveFloating,
	double fx,
	double fixing,
	string interpolator)
{
	//Define fecha
	QCDate valueDate{ xlValueDate };

	//holidays: nombre, fecha. Se construye un map<string, vector<QCDate>> que para cada nombre de calendario
	//tenga todas las fechas
	map<string, vector<QCDate>> mapHolidays;
	HelperFunctions::buildHolidays(holidays, mapHolidays);

	//Metemos las caracteristicas de los indices en esta estructura
	map<string, pair<string, string>> indexChars; //en el pair viene el tenor y el start date rule
	HelperFunctions::buildStringPairStringMap(intRateIndexChars, indexChars);


	//Se construye el shared_ptr de QCZeroCouponCurve de la curva auxiliar de los fwd
	int puntosCurva = auxCurveFwd.RowsInStructure();
	vector<long> auxTenorsFwd;
	auxTenorsFwd.resize(puntosCurva);
	vector<double> auxRatesFwd;
	auxRatesFwd.resize(puntosCurva);
	for (int i = 0; i < puntosCurva; ++i)
	{
		auxTenorsFwd.at(i) = static_cast<long>(auxCurveFwd(i, 0).NumericValue());
		auxRatesFwd.at(i) = auxCurveFwd(i, 1).NumericValue();
	}

	//definir la curva cero cupon auxiliar de forwards
	shared_ptr<QCInterestRateCurve> auxCrvFwdPtr = QCFactoryFunctions::intRtCrvShrdPtr(auxTenorsFwd,
		auxRatesFwd, "linear", "com", "act/365", QCInterestRateCurve::qcZeroCouponCurve);

	//Se construye el shared_ptr de QCZeroCouponCurve de la curva auxiliar de las patas flotantes
	puntosCurva = auxCurveFloating.RowsInStructure();
	vector<long> auxTenorsFloating;
	auxTenorsFloating.resize(puntosCurva);
	vector<double> auxRatesFloating;
	auxRatesFloating.resize(puntosCurva);
	for (int i = 0; i < puntosCurva; ++i)
	{
		auxTenorsFloating.at(i) = static_cast<long>(auxCurveFloating(i, 0).NumericValue());
		auxRatesFloating.at(i) = auxCurveFloating(i, 1).NumericValue();
	}

	//definir la curva cero cupon auxiliar de patas flotantes
	shared_ptr<QCInterestRateCurve> auxCrvFloatingPtr = QCFactoryFunctions::intRtCrvShrdPtr(auxTenorsFloating,
		auxRatesFloating, "linear", "com", "act/365", QCInterestRateCurve::qcZeroCouponCurve);

	shared_ptr<map<QCDate, double>> fixings;

	unsigned int cuantasTasas = xlInputRates.RowsInStructure();
	vector<shared_ptr<QCTimeDepositPayoff>> inputRates;
	inputRates.resize(cuantasTasas);

	unsigned int cuantosForwards = xlInputForwards.RowsInStructure();
	vector<shared_ptr<QCFXForward>> inputForwards;
	inputForwards.resize(cuantosForwards);

	unsigned int cuantosSwaps = xlInputFloatingLegs.RowsInStructure();
	vector<shared_ptr<QCFloatingRatePayoff>> inputFloatingLegs;
	inputFloatingLegs.resize(cuantosSwaps);

	vector<long> tenors;
	tenors.resize(cuantasTasas + cuantosForwards + cuantosSwaps);
	for (unsigned int i = 0; i < cuantasTasas; ++i)
	{
		QCDate fecha{ (long)xlInputRates(i, 1).NumericValue() };
		tenors.at(i) = valueDate.dayDiff(fecha);
	}

	for (unsigned int i = cuantasTasas; i < cuantasTasas + cuantosForwards; ++i)
	{
		QCDate fecha{ (long)xlInputForwards(i - cuantasTasas, 1).NumericValue() };
		tenors.at(i) = valueDate.dayDiff(fecha);
	}

	for (unsigned int i = cuantasTasas + cuantosForwards; i < cuantasTasas + cuantosForwards +
		cuantosSwaps; ++i)
	{
		QCDate fecha{ (long)xlInputFloatingLegs(i - cuantasTasas - cuantosForwards, 2).NumericValue() };
		tenors.at(i) = valueDate.dayDiff(fecha);
	}

	vector<double> rates;
	rates.resize(cuantasTasas + cuantosForwards + cuantosSwaps);
	for (unsigned int i = 0; i < cuantasTasas + cuantosForwards + cuantosSwaps; ++i)
	{
		rates.at(i) = 0.0;
	}

	shared_ptr<QCInterestRateCurve> curve = QCFactoryFunctions::intRtCrvShrdPtr(
		tenors, rates, "LINEAR", "com", "act/365", QCInterestRateCurve::qcZeroCouponCurve);

	for (unsigned int i = 0; i < cuantasTasas; ++i)
	{
		QCInterestRateLeg tdLeg = QCFactoryFunctions::buildTimeDepositLeg(
			"R",
			QCDate{ (long)xlInputRates(i, 0).NumericValue() },
			QCDate{ (long)xlInputRates(i, 1).NumericValue() },
			1.0);
		string yf{ xlInputRates(i, 3).StringValue() };
		string wf{ xlInputRates(i, 4).StringValue() };
		QCHelperFunctions::lowerCase(yf);
		QCHelperFunctions::lowerCase(wf);
		QCIntrstRtShrdPtr intRate = QCFactoryFunctions::intRateSharedPtr(
			xlInputRates(i, 2).NumericValue(), yf, wf);
		QCTimeDepositPayoff tdPayoff{ intRate, make_shared<QCInterestRateLeg>(tdLeg), valueDate, curve };
		inputRates.at(i) = make_shared<QCTimeDepositPayoff>(tdPayoff);
	}

	map<QCCurrencyConverter::QCFxRateEnum, double> fxRate;
	vector<shared_ptr<QCDiscountBondPayoff>> legs;
	legs.resize(2);
	fxRate.insert(pair<QCCurrencyConverter::QCFxRateEnum, double>(QCCurrencyConverter::qcUSDCLP, fx));
	fxRate.insert(pair<QCCurrencyConverter::QCFxRateEnum, double>(QCCurrencyConverter::qcCLPCLP, 1.0));
	for (unsigned int i = 0; i < cuantosForwards; ++i)
	{
		QCInterestRateLeg dbLeg0 = QCFactoryFunctions::buildDiscountBondLeg(
			"R",
			QCDate{ (long)xlInputForwards(i, 0).NumericValue() },
			QCDate{ (long)xlInputForwards(i, 1).NumericValue() },
			0, 0, mapHolidays.at("SCL"), mapHolidays.at("SCL"), 1.0); //Esta esta en USD

		double weakNotional = xlInputForwards(i, 3).NumericValue() + fx;
		QCInterestRateLeg dbLeg1 = QCFactoryFunctions::buildDiscountBondLeg(
			"P",
			QCDate{ (long)xlInputForwards(i, 0).NumericValue() },
			QCDate{ (long)xlInputForwards(i, 1).NumericValue() },
			0, 0, mapHolidays.at("SCL"), mapHolidays.at("SCL"), weakNotional);

		auto dbPayoff0 = make_shared<QCDiscountBondPayoff>(
			make_shared<QCInterestRateLeg>(dbLeg0),
			curve, valueDate, fixings,
			QCCurrencyConverter::qcUSD, QCCurrencyConverter::qcUSDCLP);
		legs.at(0) = dbPayoff0;

		auto dbPayoff1 = make_shared<QCDiscountBondPayoff>(
			make_shared<QCInterestRateLeg>(dbLeg1),
			auxCrvFwdPtr, valueDate, fixings,
			QCCurrencyConverter::qcCLP, QCCurrencyConverter::qcCLPCLP);
		legs.at(1) = dbPayoff1;

		auto fxFwd = make_shared<QCFXForward>(legs, QCCurrencyConverter::qcCLP,
			make_shared<map<QCCurrencyConverter::QCFxRateEnum, double>>(fxRate));

		inputForwards.at(i) = fxFwd;
	}

	vector<tuple<QCDate, double, double>> amortIfCustom;
	map<QCDate, double> liborFixing;
	liborFixing.insert(make_pair(QCDate{ 13, 3, 2017 }, fixing));
	for (unsigned int i = 0; i < cuantosSwaps; ++i)
	{
		//Se construye el interest rate
		string wf = xlInputFloatingLegs(i, 18).StringValue();
		QCHelperFunctions::lowerCase(wf);
		string yf = xlInputFloatingLegs(i, 19).StringValue();
		QCHelperFunctions::lowerCase(yf);
		shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
			xlInputFloatingLegs(i, 10).NumericValue(), yf, wf);

		//buildFloatingRateLeg
		//Este pedazo de código debe quedar en una HelperFunction de QC_DVE_XLL
		QCInterestRateLeg tmpFloatingLeg = QCFactoryFunctions::buildFloatingRateLeg2(
			"R",			//receive or pay
			QCDate{ (long)xlInputFloatingLegs(i, 1).NumericValue() },			//start date
			QCDate{ (long)xlInputFloatingLegs(i, 2).NumericValue() },			//end date
			mapHolidays.at("SCL"),	//settlement calendar
			(int)xlInputFloatingLegs(i, 4).NumericValue(),			//settlement lag
			QCHelperFunctions::stringToQCStubPeriod(xlInputFloatingLegs(i, 5).StringValue()),	//stub period
			xlInputFloatingLegs(i, 6).StringValue(),			//periodicity
			QCHelperFunctions::stringToQCBusDayAdjRule(xlInputFloatingLegs(i, 7).StringValue()),//end date adjustment
			QCHelperFunctions::stringToQCAmortization(xlInputFloatingLegs(i, 8).StringValue()),	//amortization
			amortIfCustom, //amortization and notional by date
			(int)xlInputFloatingLegs(i, 15).NumericValue(),	
			QCHelperFunctions::stringToQCStubPeriod(xlInputFloatingLegs(i, 12).StringValue()),//fixing stub period
			xlInputFloatingLegs(i, 13).StringValue(),			//periodicity
			mapHolidays.at("LONDON"),
			indexChars.at("Libor USD 3M"),
			1.0);			//notional

		//buildFloatingRatePayoff
		shared_ptr<QCFloatingRatePayoff> tmpIntRatePayoff = shared_ptr<QCFloatingRatePayoff>(
			new QCFloatingRatePayoff{ tmpIntRate, xlInputFloatingLegs(i, 11).NumericValue(), 1.0,
			make_shared<QCInterestRateLeg>(tmpFloatingLeg),
			auxCrvFloatingPtr, curve, valueDate,
			make_shared<map<QCDate, double>>(liborFixing) });

		//Se agrega el payoff al vector de fixed legs
		inputFloatingLegs.at(i) = tmpIntRatePayoff;
	}

	//Tengo que dar de alta el objeto bootstrapeador
	QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs boot{ valueDate, inputRates,
		inputForwards, (unsigned int)whichLeg, inputFloatingLegs,
		dynamic_pointer_cast<QCZeroCouponCurve>(curve) };
	boot.generateCurve();

	CellMatrix result(cuantasTasas + cuantosForwards + cuantosSwaps, 2);
	for (unsigned int i = 0; i < cuantasTasas + cuantosForwards + cuantosSwaps; ++i)
	{
		pair<long, double> temp = curve->getValuesAt(i);
		result(i, 0) = temp.first;
		result(i, 1) = temp.second;
	}
	return result;

}

double QCAddTenorToDate(string tenor, double startDate, CellMatrix holidays, string adjRule)
{
	long months{ QCHelperFunctions::tenor(tenor) };
	QCDate startDateOk{ static_cast<long>(startDate) };
	QCDate endDate{ startDateOk.addMonths(months) };
	vector<QCDate> holidaysOk;
	size_t numHolidays{ holidays.RowsInStructure() };
	holidaysOk.resize(numHolidays);
	for (size_t i = 0; i < numHolidays; ++i)
	{
		holidaysOk.at(i) = QCDate((long)holidays(i, 0).NumericValue());
	}
	QCDate::QCBusDayAdjRules rule{ QCHelperFunctions::stringToQCBusDayAdjRule(adjRule) };
	endDate = endDate.businessDay(holidaysOk, rule);
	return (double)endDate.excelSerial();
}

double QCAddChileTenorToDate(string tenor, double startDate, CellMatrix holidays)
{
	string adjRule{ "PREV" };
	long months{ QCHelperFunctions::tenor(tenor) };
	QCDate startDateOk{ static_cast<long>(startDate) };
	QCDate endDate{ startDateOk.addMonths(months) };
	vector<QCDate> holidaysOk;
	size_t numHolidays{ holidays.RowsInStructure() };
	holidaysOk.resize(numHolidays);
	for (size_t i = 0; i < numHolidays; ++i)
	{
		holidaysOk.at(i) = QCDate((long)holidays(i, 0).NumericValue());
	}
	QCDate::QCBusDayAdjRules rule{ QCHelperFunctions::stringToQCBusDayAdjRule(adjRule) };
	endDate.setDay(9);
	endDate = endDate.businessDay(holidaysOk, rule);
	return (double)endDate.excelSerial();
}

double QCBusinessDate(double startDate, CellMatrix holidays, string adjRule)
{
	QCDate startDateOk{ static_cast<long>(startDate) };
	vector<QCDate> holidaysOk;
	size_t numHolidays{ holidays.RowsInStructure() };
	holidaysOk.resize(numHolidays);
	for (size_t i = 0; i < numHolidays; ++i)
	{
		holidaysOk.at(i) = QCDate((long)holidays(i, 0).NumericValue());
	}
	QCDate::QCBusDayAdjRules rule{ QCHelperFunctions::stringToQCBusDayAdjRule(adjRule) };
	startDateOk = startDateOk.businessDay(holidaysOk, rule);
	return static_cast<double>(startDateOk.excelSerial());

}

double QCGetDateFromString(string f)
{
	QCDate fecha{ f };
	return static_cast<double>(fecha.excelSerial());
}

double QCGetYearFraction(int startDate, int endDate, string yf)
{
	QCDate stDate{ startDate };
	QCDate eDate{ endDate };
	transform(yf.begin(), yf.end(), yf.begin(), ::toupper);
	if (yf == "ACT360")
	{
		QCAct360 act360;
		return act360.yf(stDate, eDate);
	}
	else if (yf == "ACT365")
	{
		QCAct365 act365;
		return act365.yf(stDate, eDate);
	}
	else if (yf == "30360")
	{
		QC30360 qc30360;
		return qc30360.yf(stDate, eDate);
	}
	else
	{
		QCActAct actAct;
		return actAct.yf(stDate, eDate);
	}
}

double QCGetWealthFactor(double rate, double stDt, double endDt, string yf, string wf)
{
	QCDate startDate{ static_cast<long>(stDt) };
	QCDate endDate{ static_cast<long>(endDt) };

	QCHelperFunctions::lowerCase(yf);
	QCHelperFunctions::lowerCase(wf);
	auto ratePtr = QCFactoryFunctions::intRateSharedPtr(rate, yf, wf);
	return ratePtr->wf(startDate, endDate);
}

double QCGetDiscountFactorFromCurve(CellMatrix curva,
	string curveInterpolator,
	string yf,
	string wf,
	double plazo)
{
	auto curvaCero = HelperFunctions::buildZeroCouponCurve(curva, curveInterpolator, yf, wf);
	return curvaCero->getDiscountFactorAt(static_cast<long>(plazo));
}

double QCGetDiscountFactorFwdFromCurve(CellMatrix curva,
	string curveInterpolator,
	string yf,
	string wf,
	double plazo1,
	double plazo2)
{
	auto curvaCero = HelperFunctions::buildZeroCouponCurve(curva, curveInterpolator, yf, wf);
	return curvaCero->getDiscountFactorFwd(static_cast<long>(plazo1), static_cast<long>(plazo2));
}

double QCGetWealthFactorFwdFromCurve(CellMatrix curva,
	string curveInterpolator,
	string yf,
	string wf,
	double plazo1,
	double plazo2)
{
	auto curvaCero = HelperFunctions::buildZeroCouponCurve(curva, curveInterpolator, yf, wf);
	return curvaCero->getForwardWf(static_cast<long>(plazo1), static_cast<long>(plazo2));
}

double QCGetFwdRateFromCurve(CellMatrix curva,
	string curveInterpolator,
	string curveYf,
	string curveWf,
	double plazo1,
	double plazo2,
	string rateYf,
	string rateWf)
{
	auto curvaCero = HelperFunctions::buildZeroCouponCurve(curva, curveInterpolator, curveYf, curveWf);
	double wf = curvaCero->getForwardWf(static_cast<long>(plazo1), static_cast<long>(plazo2));
	QCHelperFunctions::lowerCase(rateYf);
	QCHelperFunctions::lowerCase(rateWf);
	auto ir = QCFactoryFunctions::intRateSharedPtr(0, rateYf, rateWf);
	return ir->getRateFromWf(wf, static_cast<long>(plazo2 - plazo1));
}

CellMatrix cashFlow(CellMatrix tablaDesarrollo, double tasa, int fecha, string yf, string wf)
{
	QCInterestRateLeg::QCInterestRatePeriods periods;
	unsigned int filas = tablaDesarrollo.RowsInStructure();

	for (unsigned int i = 0; i < filas; ++i)
	{
		periods.push_back(
			make_tuple(tablaDesarrollo(i, 0).NumericValue(),
			static_cast<bool>(tablaDesarrollo(i, 1).NumericValue()),
			tablaDesarrollo(i, 2).NumericValue(),
			(bool)tablaDesarrollo(i, 3).NumericValue(),
			tablaDesarrollo(i, 4).NumericValue(),
			QCDate{ (long)tablaDesarrollo(i, 5).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 6).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 7).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 8).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 9).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 10).NumericValue() }
		));
	}

	/*QCInterestRate, QCInterestRateLeg, QCDate, QCZeroCouponCurve y QCTimeSeriesShrdPointer*/
	
	QCInterestRateLeg intLeg{ periods, filas - 1 };
	auto intLegPtr = make_shared<QCInterestRateLeg>(intLeg);

	//Contruimos shared_ptr de QCYearFraction. Por ahora solo QCAct360
	shared_ptr<QCYearFraction> act360Ptr(new QCAct360);
	//QCAct360 act360;
	//QCYrFrctnShrdPtr act360Ptr = make_shared<QCYearFraction>(act360);

	//Construimos un shared pointer de QCWealthFactor. Por ahora solo lineal.
	shared_ptr<QCWealthFactor> linPtr(new QCLinearWf);
	//QCLinearWf lin;
	//QCWlthFctrShrdPtr linPtr = make_shared<QCWealthFactor>(lin);

	//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
	QCInterestRate intRate{ tasa, act360Ptr, linPtr };
	QCIntrstRtShrdPtr intRatePtr = make_shared<QCInterestRate>(intRate);

	//Vamos a construir los shared_ptr de QCZeroCouponCurve y QCTimeSeries y los dejamos nulos
	QCZrCpnCrvShrdPtr crvPtr;
	QCTimeSeriesShrdPtr timeSrsPtr;

	//Construimos el objeto QCDate con la fecha de valorizacion
	QCDate valDate{ fecha }; //constructor que toma un Excel serial
	
	//Finalmente contruimos el payoff
	QCFixedRatePayoff fxRtPff{ intRatePtr, intLegPtr, crvPtr, valDate, timeSrsPtr };
	QCIntrstRtPffShrdPtr fxRtPffPtr = make_shared<QCFixedRatePayoff>(fxRtPff);

	CellMatrix result(50,3);

	fxRtPffPtr->payoff();
	for (int i = 0; i < fxRtPffPtr->payoffSize(); ++i)
	{
		//En cada elemento de payoff viene fecha, label, monto
		QCDate fechaTemp = get<0>(fxRtPffPtr->getCashflowAt(i));
		int label = get<1>(fxRtPffPtr->getCashflowAt(i));
		double monto = get<2>(fxRtPffPtr->getCashflowAt(i));

		result(i, 0) = fechaTemp.excelSerial();
		result(i, 1) = label;
		result(i, 2) = monto;
	}

	return result;
}

double pvFixed1(CellMatrix tablaDesarrollo,
	CellMatrix curva,
	double tasa,
	int fecha,
	string yf,
	string wf)
{
	QCInterestRateLeg::QCInterestRatePeriods periods;
	unsigned int filas = tablaDesarrollo.RowsInStructure();

	for (size_t i = 0; i < filas; ++i)
	{
		periods.push_back(
			make_tuple(tablaDesarrollo(i, 0).NumericValue(),
			(bool)tablaDesarrollo(i, 1).NumericValue(),
			tablaDesarrollo(i, 2).NumericValue(),
			(bool)tablaDesarrollo(i, 3).NumericValue(),
			tablaDesarrollo(i, 4).NumericValue(),
			QCDate{ (long)tablaDesarrollo(i, 5).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 6).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 7).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 8).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 9).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 10).NumericValue() }
		));
	}

	/*QCInterestRate, QCInterestRateLeg, QCDate, QCZeroCouponCurve y QCTimeSeriesShrdPointer*/

	//Construimos un shared pointer de QCInterestRateLeg
	QCInterestRateLeg intLeg{ periods, filas - 1 };
	auto intLegPtr = make_shared<QCInterestRateLeg>(intLeg);

	//Contruimos shared_ptr de QCYearFraction. Por ahora solo QCAct360
	shared_ptr<QCYearFraction> act360Ptr(new QCAct360);

	//Construimos un shared pointer de QCWealthFactor. Por ahora solo lineal.
	shared_ptr<QCWealthFactor> linPtr(new QCLinearWf);

	//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
	QCInterestRate intRate{ tasa, act360Ptr, linPtr };
	QCIntrstRtShrdPtr intRatePtr = make_shared<QCInterestRate>(intRate);

	//Vamos a construir el shared_ptr de QCZeroCouponCurve
	int puntosCurva = curva.RowsInStructure();
	vector<long> tenors;
	tenors.resize(puntosCurva);
	vector<double> rates;
	rates.resize(puntosCurva);
	for (int i = 0; i < puntosCurva; ++i)
	{
		tenors.at(i) = static_cast<long>(curva(i, 0).NumericValue());
		rates.at(i) = curva(i, 1).NumericValue();
	}

	shared_ptr<QCCurve<long>> crvPtr(new QCCurve<long>{ tenors, rates });
	shared_ptr<QCInterpolator> interpol(new QCLinearInterpolator{ crvPtr });
	//definir un interest rate y meterlo al constructor
	QCZrCpnCrvShrdPtr zrCrvPtr(new QCZeroCouponInterestRateCurve{ interpol, intRate });

	//El shared_ptr de QCTimeSeries lo dejamos nulo
	QCTimeSeriesShrdPtr timeSrsPtr;

	//Construimos el objeto QCDate con la fecha de valorizacion
	QCDate valDate{ fecha }; //constructor que toma un Excel serial

	//Finalmente contruimos el payoff
	QCFixedRatePayoff fxRtPff{ intRatePtr, intLegPtr, zrCrvPtr, valDate, timeSrsPtr };
	QCIntrstRtPffShrdPtr fxRtPffPtr = make_shared<QCFixedRatePayoff>(fxRtPff);

	return fxRtPffPtr->presentValue();
}

double pvFixed(CellMatrix tablaDesarrollo,
	CellMatrix curva,
	double tasa,
	int fecha,
	string yf,
	string wf,
	string curveYf,
	string curveWf)
{
	QCInterestRateLeg::QCInterestRatePeriods periods;
	unsigned int filas = tablaDesarrollo.RowsInStructure();

	for (size_t i = 0; i < filas; ++i)
	{
		periods.push_back(
			make_tuple(tablaDesarrollo(i, 0).NumericValue(),
			(bool)tablaDesarrollo(i, 1).NumericValue(),
			tablaDesarrollo(i, 2).NumericValue(),
			(bool)tablaDesarrollo(i, 3).NumericValue(),
			tablaDesarrollo(i, 4).NumericValue(),
			QCDate{ (long)tablaDesarrollo(i, 5).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 6).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 7).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 8).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 9).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 10).NumericValue() }
		));
	}

	//QCInterestRate, QCInterestRateLeg, QCDate, QCZeroCouponCurve y QCTimeSeriesShrdPointer

	//Construimos un shared pointer de QCInterestRateLeg
	QCInterestRateLeg intLeg{ periods, filas - 1 };
	auto intLegPtr = make_shared<QCInterestRateLeg>(intLeg);

	//Contruimos shared_ptr de QCYearFraction.
	shared_ptr<QCYearFraction> yfShrdPtr = QCFactoryFunctions::yfSharedPtr(yf);

	//Construimos un shared pointer de QCWealthFactor.
	shared_ptr<QCWealthFactor> wfShrdPtr = QCFactoryFunctions::wfSharedPtr(wf);

	//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
	//Este se usara para instanciar la QCFixedRateLeg
	QCInterestRate intRate{ tasa, yfShrdPtr, wfShrdPtr };
	QCIntrstRtShrdPtr intRatePtr = make_shared<QCInterestRate>(intRate);

	//Vamos a construir el shared_ptr de QCZeroCouponCurve
	int puntosCurva = curva.RowsInStructure();
	vector<long> tenors;
	tenors.resize(puntosCurva);
	vector<double> rates;
	rates.resize(puntosCurva);
	for (int i = 0; i < puntosCurva; ++i)
	{
		tenors.at(i) = static_cast<long>(curva(i, 0).NumericValue());
		rates.at(i) = curva(i, 1).NumericValue();
	}

	//definir la curva cero cupon de descuento
	QCZrCpnCrvShrdPtr zrCrvPtr = QCFactoryFunctions::zrCpnCrvShrdPtr(tenors,
		rates, "linear", curveYf, curveWf);

	//El shared_ptr de QCTimeSeries lo dejamos nulo
	QCTimeSeriesShrdPtr timeSrsPtr;

	//Construimos el objeto QCDate con la fecha de valorizacion
	QCDate valDate{ fecha }; //constructor que toma un Excel serial

	//Finalmente contruimos el payoff
	QCFixedRatePayoff fxRtPff{ intRatePtr, intLegPtr, zrCrvPtr, valDate, timeSrsPtr };
	QCIntrstRtPffShrdPtr fxRtPffPtr = make_shared<QCFixedRatePayoff>(fxRtPff);

	return fxRtPffPtr->presentValue();
}

//Ultima version para pricing
double QCPvFixedRateLeg(CellMatrix tablaDesarrollo,
	CellMatrix nominalAmortizacion,
	double tasa,
	double fecha,
	string yf,
	string wf,
	CellMatrix curva,
	string curveInterpolator,
	string curveYf,
	string curveWf)
{
	if (!HelperFunctions::checkAuthKey())
		throw runtime_error(LICENSE_MSG);

	QCInterestRateLeg::QCInterestRatePeriods periods;
	unsigned int filas = tablaDesarrollo.RowsInStructure();

	periods.resize(filas);
	for (size_t i = 0; i < filas; ++i)
	{
		periods.at(i) = make_tuple(tablaDesarrollo(i, 0).NumericValue(),
			(bool)tablaDesarrollo(i, 1).NumericValue(),
			nominalAmortizacion(i, 1).NumericValue(),
			(bool)tablaDesarrollo(i, 3).NumericValue(),
			nominalAmortizacion(i, 0).NumericValue(),
			QCDate{ (long)tablaDesarrollo(i, 5).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 6).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 7).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 8).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 9).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 10).NumericValue() }
		);
	}

	//QCInterestRate, QCInterestRateLeg, QCDate, QCZeroCouponCurve y QCTimeSeriesShrdPointer

	//Construimos un shared pointer de QCInterestRateLeg
	QCInterestRateLeg intLeg{ periods, filas - 1 };
	auto intLegPtr = make_shared<QCInterestRateLeg>(intLeg);

	//Contruimos shared_ptr de QCYearFraction.
	QCHelperFunctions::lowerCase(yf);
	auto yfShrdPtr = QCFactoryFunctions::yfSharedPtr(yf);

	//Construimos un shared pointer de QCWealthFactor.
	QCHelperFunctions::lowerCase(wf);
	auto wfShrdPtr = QCFactoryFunctions::wfSharedPtr(wf);

	//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
	//Este se usara para instanciar la QCFixedRateLeg
	QCInterestRate intRate{ tasa, yfShrdPtr, wfShrdPtr };
	auto intRatePtr = make_shared<QCInterestRate>(intRate);

	//Vamos a construir el shared_ptr de QCZeroCouponCurve
	size_t puntosCurva = curva.RowsInStructure();
	vector<long> tenors;
	tenors.resize(puntosCurva);
	vector<double> rates;
	rates.resize(puntosCurva);
	for (size_t i = 0; i < puntosCurva; ++i)
	{
		tenors.at(i) = static_cast<long>(curva(i, 0).NumericValue());
		rates.at(i) = curva(i, 1).NumericValue();
	}

	//Definir la curva cero cupon de descuento
	QCHelperFunctions::lowerCase(curveInterpolator);
	QCHelperFunctions::lowerCase(curveYf);
	QCHelperFunctions::lowerCase(curveWf);
	auto zrCrvPtr = QCFactoryFunctions::zrCpnCrvShrdPtr(tenors,
		rates, curveInterpolator, curveYf, curveWf);

	//El shared_ptr de QCTimeSeries lo dejamos nulo
	QCTimeSeriesShrdPtr timeSrsPtr;

	//Construimos el objeto QCDate con la fecha de valorizacion
	QCDate valDate{ (long)fecha }; //constructor que toma un Excel serial

	//Finalmente contruimos el payoff
	QCFixedRatePayoff fxRtPff{ intRatePtr, intLegPtr, zrCrvPtr, valDate, timeSrsPtr };

	return fxRtPff.presentValue();
}

double QCPvIcpClp(CellMatrix tablaDesarrollo //Tabla de desarrollo de la pata
	, CellMatrix nominalAmortizacion //rango con dos columnas con nominal vigente y amortizacion por periodo
	, double fecha				//fecha de valorizacion
	, double addSpread			//Spread aditivo
	, CellMatrix curva			//Curva de proyeccion
	, string curveInterpolator	//tipo de interpolacion para la curva
	, string curveYf					//fraccion de año de las tasas de la curva
	, string curveWf					//factor de capitalizacion de las tasas de la curva
	)
{
	if (!HelperFunctions::checkAuthKey())
		throw runtime_error(LICENSE_MSG);

	QCInterestRateLeg::QCInterestRatePeriods periods;
	unsigned int filas = tablaDesarrollo.RowsInStructure();
	periods.resize(filas);

	for (size_t i = 0; i < filas; ++i)
	{
		periods.at(i) = make_tuple(tablaDesarrollo(i, 0).NumericValue(),
			(bool)tablaDesarrollo(i, 1).NumericValue(),
			nominalAmortizacion(i, 1).NumericValue(),
			(bool)tablaDesarrollo(i, 3).NumericValue(),
			nominalAmortizacion(i, 0).NumericValue(),
			QCDate{ (long)tablaDesarrollo(i, 5).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 6).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 7).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 8).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 9).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 10).NumericValue() }
		);
	}

	/*QCInterestRate, QCInterestRateLeg, QCDate, QCZeroCouponCurve y QCTimeSeriesShrdPointer*/

	//Construimos un shared pointer de QCInterestRateLeg
	QCInterestRateLeg intLeg{ periods, filas - 1 };
	auto intLegPtr = make_shared<QCInterestRateLeg>(intLeg);

	//Contruimos shared_ptr de QCYearFraction.
	string yf{ "act/360" };
	QCHelperFunctions::lowerCase(yf);
	auto yfShrdPtr = QCFactoryFunctions::yfSharedPtr(yf);

	//Construimos un shared pointer de QCWealthFactor.
	string wf{ "lin" };
	QCHelperFunctions::lowerCase(wf);
	auto wfShrdPtr = QCFactoryFunctions::wfSharedPtr(wf);

	//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
	//Este se usara para instanciar la QCFixedRateLeg
	double valorTasa{ 0.0 };
	QCInterestRate intRate{ valorTasa, yfShrdPtr, wfShrdPtr };
	auto intRatePtr = make_shared<QCInterestRate>(intRate);

	//Vamos a construir el shared_ptr de QCZeroCouponCurve
	size_t puntosCurva = curva.RowsInStructure();
	vector<long> tenors;
	tenors.resize(puntosCurva);
	vector<double> rates;
	rates.resize(puntosCurva);
	for (size_t i = 0; i < puntosCurva; ++i)
	{
		tenors.at(i) = static_cast<long>(curva(i, 0).NumericValue());
		rates.at(i) = curva(i, 1).NumericValue();
	}

	//Definir la curva cero cupon de descuento
	QCHelperFunctions::lowerCase(curveInterpolator);
	QCHelperFunctions::lowerCase(curveYf);
	QCHelperFunctions::lowerCase(curveWf);
	auto zrCrvPtr = QCFactoryFunctions::zrCpnCrvShrdPtr(tenors,
		rates, curveInterpolator, curveYf, curveWf);

	//El shared_ptr de QCTimeSeries lo dejamos nulo
	QCTimeSeriesShrdPtr timeSrsPtr;

	//Construimos el objeto QCDate con la fecha de valorizacion
	QCDate valDate{ (long)fecha }; //constructor que toma un Excel serial

	//Finalmente contruimos el payoff
	double multSpread = 1.0;
	QCIcpClpPayoff icpClpPayoff{ intRatePtr, addSpread, multSpread, intLegPtr,
		zrCrvPtr, zrCrvPtr, valDate, timeSrsPtr };

	return  icpClpPayoff.presentValue();
}

double QCPvFloatingLeg(CellMatrix tablaDesarrollo //Tabla de desarrollo de la pata
	, CellMatrix nominalAmortizacion //Rango con dos columnas con nominal vigente y amortizacion por periodo
	, double fecha			//Fecha de valorizacion
	, CellMatrix curvaProy	//Rango con dos columnas con plazos y tasas curva proyeccion
	, string projCurveInterpolator	//Metodo de interpolacion de la curva de proyeccion
	, string projCurveYf	//Fraccion de ano de las tasas de la curva de proyeccion
	, string projCurveWf	//Factor de capitalizacion de las tasas de la curva de proyeccion
	, CellMatrix curvaDesc	//Rango con dos columnas con plazos y tasas de la curva de descuento
	, string discCurveInterpolator	//Metodo de interpolacion de la curva de descuento
	, string discCurveYf	//Fraccion de ano de las tasas de la curva de descuento
	, string discCurveWf	//Factor de capitalizacion de las tasas de la curva de descuento
	, CellMatrix pastFixings//Rango con dos columnas con fechas y tasas para fixing
	, double addSpread		//Valor del spread sobre tasa flotante
	, double multSpread		//Valor del spread multiplicativo
	, string yf				//Fraccion de ano de la tasa flotante
	, string wf)			//Factor de capitalizacion de la tasa flotante
{
	if (!HelperFunctions::checkAuthKey())
		throw runtime_error(LICENSE_MSG);

	QCInterestRateLeg::QCInterestRatePeriods periods;
	unsigned int filas = tablaDesarrollo.RowsInStructure();

	for (size_t i = 0; i < filas; ++i)
	{
		periods.push_back(
			make_tuple(tablaDesarrollo(i, 0).NumericValue(),
			(bool)tablaDesarrollo(i, 1).NumericValue(),
			nominalAmortizacion(i, 1).NumericValue(),
			(bool)tablaDesarrollo(i, 3).NumericValue(),
			nominalAmortizacion(i, 0).NumericValue(),
			QCDate{ (long)tablaDesarrollo(i, 5).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 6).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 7).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 8).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 9).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 10).NumericValue() }
		));
	}

	/*QCInterestRate, QCInterestRateLeg, QCDate, QCZeroCouponCurve y QCTimeSeriesShrdPointer*/

	//Construimos un shared pointer de QCInterestRateLeg
	QCInterestRateLeg intLeg{ periods, filas - 1 };
	auto intLegPtr = make_shared<QCInterestRateLeg>(intLeg);

	//Contruimos shared_ptr de QCYearFraction.
	QCHelperFunctions::lowerCase(yf);
	auto rateYearFraction = QCFactoryFunctions::yfSharedPtr(yf);

	//Construimos un shared pointer de QCWealthFactor.
	QCHelperFunctions::lowerCase(wf);
	auto rateWealthFactor = QCFactoryFunctions::wfSharedPtr(wf);

	//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
	QCInterestRate intRate{ 0.0, rateYearFraction, rateWealthFactor };
	QCIntrstRtShrdPtr intRatePtr = make_shared<QCInterestRate>(intRate);

	//Vamos a construir el shared_ptr de QCZeroCouponCurve para la curva de descuento
	int puntosCurva = curvaDesc.RowsInStructure();
	vector<long> tenors;
	tenors.resize(puntosCurva);
	vector<double> rates;
	rates.resize(puntosCurva);
	for (int i = 0; i < puntosCurva; ++i)
	{
		tenors.at(i) = static_cast<long>(curvaDesc(i, 0).NumericValue());
		rates.at(i) = curvaDesc(i, 1).NumericValue();
	}

	//Definir la curva cero cupon de descuento
	QCHelperFunctions::lowerCase(discCurveInterpolator);
	QCHelperFunctions::lowerCase(discCurveYf);
	QCHelperFunctions::lowerCase(discCurveWf);
	auto discZrCrvPtr = QCFactoryFunctions::zrCpnCrvShrdPtr(tenors,
		rates, discCurveInterpolator, discCurveYf, discCurveWf);

	//Vamos a construir el shared_ptr de QCZeroCouponCurve para la curva de proyeccion
	int puntosCurva2 = curvaProy.RowsInStructure();
	vector<long> tenors2;
	tenors2.resize(puntosCurva2);
	vector<double> rates2;
	rates2.resize(puntosCurva2);
	for (int i = 0; i < puntosCurva2; ++i)
	{
		tenors2.at(i) = static_cast<long>(curvaProy(i, 0).NumericValue());
		rates2.at(i) = curvaProy(i, 1).NumericValue();
	}

	//Definir la curva cero cupon de descuento
	QCHelperFunctions::lowerCase(projCurveInterpolator);
	QCHelperFunctions::lowerCase(projCurveYf);
	QCHelperFunctions::lowerCase(projCurveWf);
	auto projZrCrvPtr = QCFactoryFunctions::zrCpnCrvShrdPtr(tenors2,
		rates2, projCurveInterpolator, projCurveYf, projCurveWf);

	//El shared_ptr de QCTimeSeries lo dejamos nulo
	QCTimeSeriesShrdPtr timeSrsPtr;

	//Construimos el objeto QCDate con la fecha de valorizacion
	QCDate valDate{ static_cast<long>(fecha) }; //constructor que toma un Excel serial

	//Construimos el objeto con los fixing dates
	//typedef shared_ptr<map<QCDate, double>> QCTimeSeriesShrdPtr;
	size_t puntosFixing = pastFixings.RowsInStructure();
	map<QCDate, double> fixings;
	for (size_t i = 0; i < puntosFixing; ++i)
	{
		QCDate fch = QCDate{ (long)pastFixings(i, 0).NumericValue() };
		fixings.insert(pair<QCDate, double>(fch, pastFixings(i, 1).NumericValue()));
	}
	QCTimeSeriesShrdPtr fixingsShrdPtr = make_shared<map<QCDate, double>>(fixings);

	//Finalmente contruimos el payoff
	QCFloatingRatePayoff fltRtPff{ intRatePtr, addSpread, multSpread, intLegPtr,
		projZrCrvPtr, discZrCrvPtr, valDate, fixingsShrdPtr };

	return  fltRtPff.presentValue();
}

double pvFloat1(CellMatrix tablaDesarrollo,
	CellMatrix curvaProy,
	CellMatrix curvaDesc,
	CellMatrix pastFixings,
	double addSpread, double multSpread,
	int fecha, string yf, string wf)
{
	QCInterestRateLeg::QCInterestRatePeriods periods;
	unsigned int filas = tablaDesarrollo.RowsInStructure();

	for (size_t i = 0; i < filas; ++i)
	{
		periods.push_back(
			make_tuple(tablaDesarrollo(i, 0).NumericValue(),
			(bool)tablaDesarrollo(i, 1).NumericValue(),
			tablaDesarrollo(i, 2).NumericValue(),
			(bool)tablaDesarrollo(i, 3).NumericValue(),
			tablaDesarrollo(i, 4).NumericValue(),
			QCDate{ (long)tablaDesarrollo(i, 5).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 6).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 7).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 8).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 9).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 10).NumericValue() }
		));
	}

	/*QCInterestRate, QCInterestRateLeg, QCDate, QCZeroCouponCurve y QCTimeSeriesShrdPointer*/

	//Construimos un shared pointer de QCInterestRateLeg
	QCInterestRateLeg intLeg{ periods, filas - 1 };
	auto intLegPtr = make_shared<QCInterestRateLeg>(intLeg);

	//Contruimos shared_ptr de QCYearFraction. Por ahora solo QCAct360
	shared_ptr<QCYearFraction> act360Ptr(new QCAct360);

	//Construimos un shared pointer de QCWealthFactor. Por ahora solo lineal.
	shared_ptr<QCWealthFactor> linPtr(new QCLinearWf);

	//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
	QCInterestRate intRate{ 0.0, act360Ptr, linPtr };
	QCIntrstRtShrdPtr intRatePtr = make_shared<QCInterestRate>(intRate);

	//Vamos a construir el shared_ptr de QCZeroCouponCurve para la curva de descuento
	int puntosCurva = curvaDesc.RowsInStructure();
	vector<long> tenors;
	tenors.resize(puntosCurva);
	vector<double> rates;
	rates.resize(puntosCurva);
	for (int i = 0; i < puntosCurva; ++i)
	{
		tenors.at(i) = static_cast<long>(curvaDesc(i, 0).NumericValue());
		rates.at(i) = curvaDesc(i, 1).NumericValue();
	}

	shared_ptr<QCCurve<long>> crvPtr(new QCCurve<long>{ tenors, rates });
	shared_ptr<QCInterpolator> interpol(new QCLinearInterpolator{ crvPtr });
	//definir un interest rate y meterlo al constructor
	QCZrCpnCrvShrdPtr discZrCrvPtr(new QCZeroCouponInterestRateCurve{ interpol, intRate });

	//Vamos a construir el shared_ptr de QCZeroCouponCurve para la curva de proyeccion
	int puntosCurva2 = curvaProy.RowsInStructure();
	vector<long> tenors2;
	tenors2.resize(puntosCurva2);
	vector<double> rates2;
	rates2.resize(puntosCurva2);
	for (int i = 0; i < puntosCurva2; ++i)
	{
		tenors2.at(i) = static_cast<long>(curvaProy(i, 0).NumericValue());
		rates2.at(i) = curvaProy(i, 1).NumericValue();
	}
	
	shared_ptr<QCCurve<long>> crvPtr2(new QCCurve<long>{ tenors2, rates2 });
	shared_ptr<QCInterpolator> interpol2(new QCLinearInterpolator{ crvPtr2 });
	//definir un interest rate y meterlo al constructior
	QCIntRtCrvShrdPtr intRtProjCrvPtr(new QCProjectingInterestRateCurve{ interpol2, intRate });

	//El shared_ptr de QCTimeSeries lo dejamos nulo
	QCTimeSeriesShrdPtr timeSrsPtr;

	//Construimos el objeto QCDate con la fecha de valorizacion
	QCDate valDate{ fecha }; //constructor que toma un Excel serial

	//Construimos el objeto con los fixing dates
	//typedef shared_ptr<map<QCDate, double>> QCTimeSeriesShrdPtr;
	int puntosFixing = pastFixings.RowsInStructure();
	map<QCDate, double> fixings;
	for (int i = 0; i < puntosFixing; ++i)
	{
		QCDate fch = QCDate{ (long)pastFixings(i, 0).NumericValue() };
		fixings.insert(pair<QCDate, double>(fch, pastFixings(i, 1).NumericValue()));
	}
	QCTimeSeriesShrdPtr fixingsShrdPtr = make_shared<map<QCDate, double>>(fixings);
	
	//Finalmente contruimos el payoff
	QCFloatingRatePayoff fltRtPff{ intRatePtr, addSpread, multSpread, intLegPtr,
		intRtProjCrvPtr, discZrCrvPtr, valDate, fixingsShrdPtr };
	QCIntrstRtPffShrdPtr fltRtPffPtr = make_shared<QCFloatingRatePayoff>(fltRtPff);

	return  fltRtPffPtr->presentValue();
}


double pvIcpClp1(CellMatrix tablaDesarrollo //Tabla de desarrollo de la pata
	, CellMatrix curvaProy		//Curva de proyeccion
	, CellMatrix curvaDesc		//Curva de descuento
	, CellMatrix pastFixings	//Fixings pasados de ICP
	, double addSpread			//Spread aditivo
	, double multSpread			//Spread multiplicativo
	, int fecha					//Fecha de valorizacion
	, string yf					//Fraccion de año de la tasa flotante
	, string wf					//Factor de capitalizacion de la tasa flotante
	)
	{
		QCInterestRateLeg::QCInterestRatePeriods periods;
		unsigned int filas = tablaDesarrollo.RowsInStructure();

		for (size_t i = 0; i < filas; ++i)
		{
			periods.push_back(
				make_tuple(tablaDesarrollo(i, 0).NumericValue(),
				(bool)tablaDesarrollo(i, 1).NumericValue(),
				tablaDesarrollo(i, 2).NumericValue(),
				(bool)tablaDesarrollo(i, 3).NumericValue(),
				tablaDesarrollo(i, 4).NumericValue(),
				QCDate{ (long)tablaDesarrollo(i, 5).NumericValue() },
				QCDate{ (long)tablaDesarrollo(i, 6).NumericValue() },
				QCDate{ (long)tablaDesarrollo(i, 7).NumericValue() },
				QCDate{ (long)tablaDesarrollo(i, 8).NumericValue() },
				QCDate{ (long)tablaDesarrollo(i, 9).NumericValue() },
				QCDate{ (long)tablaDesarrollo(i, 10).NumericValue() }
			));
		}

		/*QCInterestRate, QCInterestRateLeg, QCDate, QCZeroCouponCurve y QCTimeSeriesShrdPointer*/

		//Construimos un shared pointer de QCInterestRateLeg
		QCInterestRateLeg intLeg{ periods, filas - 1 };
		auto intLegPtr = make_shared<QCInterestRateLeg>(intLeg);

		//Para las curvas
		//Contruimos shared_ptr de QCYearFraction.
		shared_ptr<QCYearFraction> act365Ptr(new QCAct365);

		//Construimos un shared pointer de QCWealthFactor.
		shared_ptr<QCWealthFactor> comPtr(new QCCompoundWf);

		//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
		QCInterestRate intRate{ 0.0, act365Ptr, comPtr };
		QCIntrstRtShrdPtr intRatePtr = make_shared<QCInterestRate>(intRate);

		//Para el intRateLeg
		//Contruimos shared_ptr de QCYearFraction.
		shared_ptr<QCYearFraction> act360Ptr(new QCAct360);

		//Construimos un shared pointer de QCWealthFactor.
		shared_ptr<QCWealthFactor> linPtr(new QCLinearWf);

		//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
		QCInterestRate intRate2{ 0.0, act360Ptr, linPtr };
		QCIntrstRtShrdPtr intRate2Ptr = make_shared<QCInterestRate>(intRate2);
		//Vamos a construir el shared_ptr de QCZeroCouponCurve para la curva de descuento
		int puntosCurva = curvaDesc.RowsInStructure();
		vector<long> tenors;
		tenors.resize(puntosCurva);
		vector<double> rates;
		rates.resize(puntosCurva);
		for (int i = 0; i < puntosCurva; ++i)
		{
			tenors.at(i) = static_cast<long>(curvaDesc(i, 0).NumericValue());
			rates.at(i) = curvaDesc(i, 1).NumericValue();
		}

		shared_ptr<QCCurve<long>> crvPtr(new QCCurve<long>{ tenors, rates });
		shared_ptr<QCInterpolator> interpol(new QCLinearInterpolator{ crvPtr });
		//definir un interest rate y meterlo al constructior
		QCZrCpnCrvShrdPtr discZrCrvPtr(new QCZeroCouponInterestRateCurve{ interpol, intRate });

		//Vamos a construir el shared_ptr de QCZeroCouponCurve para la curva de proyeccion
		int puntosCurva2 = curvaProy.RowsInStructure();
		vector<long> tenors2;
		tenors2.resize(puntosCurva2);
		vector<double> rates2;
		rates2.resize(puntosCurva2);
		for (int i = 0; i < puntosCurva2; ++i)
		{
			tenors2.at(i) = static_cast<long>(curvaProy(i, 0).NumericValue());
			rates2.at(i) = curvaProy(i, 1).NumericValue();
		}

		shared_ptr<QCCurve<long>> crvPtr2(new QCCurve<long>{ tenors2, rates2 });
		shared_ptr<QCInterpolator> interpol2(new QCLinearInterpolator{ crvPtr2 });
		//definir un interest rate y meterlo al constructior
		QCIntRtCrvShrdPtr intRtProjCrvPtr(new QCZeroCouponInterestRateCurve{ interpol2, intRate });

		//El shared_ptr de QCTimeSeries lo dejamos nulo
		QCTimeSeriesShrdPtr timeSrsPtr;

		//Construimos el objeto QCDate con la fecha de valorizacion
		QCDate valDate{ fecha }; //constructor que toma un Excel serial

		//Construimos el objeto con los fixing dates
		//typedef shared_ptr<map<QCDate, double>> QCTimeSeriesShrdPtr;
		int puntosFixing = pastFixings.RowsInStructure();
		map<QCDate, double> fixings;
		for (int i = 0; i < puntosFixing; ++i)
		{
			QCDate fch = QCDate{ (long)pastFixings(i, 0).NumericValue() };
			fixings.insert(pair<QCDate, double>(fch, pastFixings(i, 1).NumericValue()));
		}
		QCTimeSeriesShrdPtr fixingsShrdPtr = make_shared<map<QCDate, double>>(fixings);

		//Finalmente contruimos el payoff
		QCIcpClpPayoff fltRtPff{ intRate2Ptr, addSpread, multSpread, intLegPtr,
			intRtProjCrvPtr, discZrCrvPtr, valDate, fixingsShrdPtr };
		QCIntrstRtPffShrdPtr fltRtPffPtr = make_shared<QCIcpClpPayoff>(fltRtPff);

		return  fltRtPffPtr->presentValue();
	}


double pvIcpClp(CellMatrix tablaDesarrollo //Tabla de desarrollo de la pata
	, CellMatrix curvaProy		//Curva de proyeccion
	, CellMatrix curvaDesc		//Curva de descuento
	, CellMatrix pastFixings	//Fixings pasados de ICP
	, double addSpread			//Spread aditivo
	, double multSpread			//Spread multiplicativo
	, int fecha					//Fecha de valorizacion
	, string yf					//Fraccion de año de la tasa flotante
	, string wf					//Factor de capitalizacion de la tasa flotante
	, string curveYf			//Fraccion de agno de la curva
	, string curveWf			//Factor de capitalizacion de la curva
	)
{
	QCInterestRateLeg::QCInterestRatePeriods periods;
	unsigned int filas = tablaDesarrollo.RowsInStructure();

	for (size_t i = 0; i < filas; ++i)
	{
		periods.push_back(
			make_tuple(tablaDesarrollo(i, 0).NumericValue(),
			(bool)tablaDesarrollo(i, 1).NumericValue(),
			tablaDesarrollo(i, 2).NumericValue(),
			(bool)tablaDesarrollo(i, 3).NumericValue(),
			tablaDesarrollo(i, 4).NumericValue(),
			QCDate{ (long)tablaDesarrollo(i, 5).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 6).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 7).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 8).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 9).NumericValue() },
			QCDate{ (long)tablaDesarrollo(i, 10).NumericValue() }
		));
	}

		
	/*QCInterestRate, QCInterestRateLeg, QCDate, QCZeroCouponCurve y QCTimeSeriesShrdPointer*/

	//Construimos un shared pointer de QCInterestRateLeg
	QCInterestRateLeg intLeg{ periods, filas - 1 };
	auto intLegPtr = make_shared<QCInterestRateLeg>(intLeg);

	//Para las curvas
	//Contruimos shared_ptr de QCYearFraction.
	shared_ptr<QCYearFraction> yfCurveShrdPtr = QCFactoryFunctions::yfSharedPtr(curveYf);
	//Construimos un shared pointer de QCWealthFactor.
	shared_ptr<QCWealthFactor> wfCurveShrdPtr = QCFactoryFunctions::wfSharedPtr(curveWf);

	//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
	QCInterestRate intRate{ 0.0, yfCurveShrdPtr, wfCurveShrdPtr };
	QCIntrstRtShrdPtr intRatePtr = make_shared<QCInterestRate>(intRate);

	//Para el intRateLeg
	//Contruimos shared_ptr de QCYearFraction.
	shared_ptr<QCYearFraction> yfShrdPtr = QCFactoryFunctions::yfSharedPtr(yf);

	//Construimos un shared pointer de QCWealthFactor.
	shared_ptr<QCWealthFactor> wfShrdPtr = QCFactoryFunctions::wfSharedPtr(wf);

	//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
	QCInterestRate intRate2{ 0.0, yfShrdPtr, wfShrdPtr };
	QCIntrstRtShrdPtr intRate2Ptr = make_shared<QCInterestRate>(intRate2);
	
	//Vamos a construir el shared_ptr de QCZeroCouponCurve para la curva de descuento
	int puntosCurva = curvaDesc.RowsInStructure();
	vector<long> tenors;
	tenors.resize(puntosCurva);
	vector<double> rates;
	rates.resize(puntosCurva);
	for (int i = 0; i < puntosCurva; ++i)
	{
		tenors.at(i) = static_cast<long>(curvaDesc(i, 0).NumericValue());
		rates.at(i) = curvaDesc(i, 1).NumericValue();
	}

	shared_ptr<QCCurve<long>> crvPtr(new QCCurve<long>{ tenors, rates });
	shared_ptr<QCInterpolator> interpol(new QCLinearInterpolator{ crvPtr });
	//definir un interest rate y meterlo al constructor
	QCZrCpnCrvShrdPtr discZrCrvPtr(new QCZeroCouponInterestRateCurve{ interpol, intRate });

	//Vamos a construir el shared_ptr de QCZeroCouponCurve para la curva de proyeccion
	int puntosCurva2 = curvaProy.RowsInStructure();
	vector<long> tenors2;
	tenors2.resize(puntosCurva2);
	vector<double> rates2;
	rates2.resize(puntosCurva2);
	for (int i = 0; i < puntosCurva2; ++i)
	{
		tenors2.at(i) = static_cast<long>(curvaProy(i, 0).NumericValue());
		rates2.at(i) = curvaProy(i, 1).NumericValue();
	}

	shared_ptr<QCCurve<long>> crvPtr2(new QCCurve<long>{ tenors2, rates2 });
	shared_ptr<QCInterpolator> interpol2(new QCLinearInterpolator{ crvPtr2 });
	//definir un interest rate y meterlo al constructior
	QCIntRtCrvShrdPtr intRtProjCrvPtr(new QCZeroCouponInterestRateCurve{ interpol2, intRate });
	//El shared_ptr de QCTimeSeries lo dejamos nulo
	QCTimeSeriesShrdPtr timeSrsPtr;

	//Construimos el objeto QCDate con la fecha de valorizacion
	QCDate valDate{ fecha }; //constructor que toma un Excel serial

	//Construimos el objeto con los fixing dates
	//typedef shared_ptr<map<QCDate, double>> QCTimeSeriesShrdPtr;
	/*int puntosFixing = pastFixings.RowsInStructure();
	map<QCDate, double> fixings;
	for (int i = 0; i < puntosFixing; ++i)
	{
		QCDate fch = QCDate{ (long)pastFixings(i, 0).NumericValue() };
		fixings.insert(pair<QCDate, double>(fch, pastFixings(i, 1).NumericValue()));
	}*/
	QCTimeSeriesShrdPtr fixingsShrdPtr; // = make_shared<map<QCDate, double>>(fixings);

	//Finalmente contruimos el payoff
	QCIcpClpPayoff fltRtPff{ intRate2Ptr, addSpread, multSpread, intLegPtr,
		intRtProjCrvPtr, discZrCrvPtr, valDate, fixingsShrdPtr };
	QCIntrstRtPffShrdPtr fltRtPffPtr = make_shared<QCIcpClpPayoff>(fltRtPff);
	return  fltRtPffPtr->presentValue();
}

double pvIcpClf(CellMatrix tablaDesarrollo //Tabla de desarrollo de la pata
	, CellMatrix curvaProy		//Curva de proyeccion
	, CellMatrix curvaDesc		//Curva de descuento
	, CellMatrix pastFixings	//Fixings pasados de ICP
	, CellMatrix ufFixings		//Fixings pasado de UF
	, double addSpread			//Spread aditivo
	, double multSpread			//Spread multiplicativo
	, int fecha					//Fecha de valorizacion
	, string yf					//Fraccion de año de la tasa flotante
	, string wf					//Factor de capitalizacion de la tasa flotante
	)
	{
		QCInterestRateLeg::QCInterestRatePeriods periods;
		unsigned int filas = tablaDesarrollo.RowsInStructure();

		for (size_t i = 0; i < filas; ++i)
		{
			periods.push_back(
				make_tuple(tablaDesarrollo(i, 0).NumericValue(),
				(bool)tablaDesarrollo(i, 1).NumericValue(),
				tablaDesarrollo(i, 2).NumericValue(),
				(bool)tablaDesarrollo(i, 3).NumericValue(),
				tablaDesarrollo(i, 4).NumericValue(),
				QCDate{ (long)tablaDesarrollo(i, 5).NumericValue() },
				QCDate{ (long)tablaDesarrollo(i, 6).NumericValue() },
				QCDate{ (long)tablaDesarrollo(i, 7).NumericValue() },
				QCDate{ (long)tablaDesarrollo(i, 8).NumericValue() },
				QCDate{ (long)tablaDesarrollo(i, 9).NumericValue() },
				QCDate{ (long)tablaDesarrollo(i, 10).NumericValue() }
			));
		}

		/*QCInterestRate, QCInterestRateLeg, QCDate, QCZeroCouponCurve y QCTimeSeriesShrdPointer*/

		//Construimos un shared pointer de QCInterestRateLeg
		QCInterestRateLeg intLeg{ periods, filas - 1 };
		auto intLegPtr = make_shared<QCInterestRateLeg>(intLeg);

		//Para las curvas
		//Contruimos shared_ptr de QCYearFraction.
		shared_ptr<QCYearFraction> act365Ptr(new QCAct365);

		//Construimos un shared pointer de QCWealthFactor.
		shared_ptr<QCWealthFactor> comPtr(new QCCompoundWf);

		//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
		QCInterestRate intRate{ 0.0, act365Ptr, comPtr };
		QCIntrstRtShrdPtr intRatePtr = make_shared<QCInterestRate>(intRate);

		//Para el intRateLeg
		//Contruimos shared_ptr de QCYearFraction.
		shared_ptr<QCYearFraction> act360Ptr(new QCAct360);

		//Construimos un shared pointer de QCWealthFactor.
		shared_ptr<QCWealthFactor> linPtr(new QCLinearWf);

		//Construimos un shared pointer de QCInterestRate con la yf y wf anteriores
		QCInterestRate intRate2{ 0.0, act360Ptr, linPtr };
		QCIntrstRtShrdPtr intRate2Ptr = make_shared<QCInterestRate>(intRate2);
		//Vamos a construir el shared_ptr de QCZeroCouponCurve para la curva de descuento
		int puntosCurva = curvaDesc.RowsInStructure();
		vector<long> tenors;
		tenors.resize(puntosCurva);
		vector<double> rates;
		rates.resize(puntosCurva);
		for (int i = 0; i < puntosCurva; ++i)
		{
			tenors.at(i) = static_cast<long>(curvaDesc(i, 0).NumericValue());
			rates.at(i) = curvaDesc(i, 1).NumericValue();
		}

		shared_ptr<QCCurve<long>> crvPtr(new QCCurve<long>{ tenors, rates });
		shared_ptr<QCInterpolator> interpol(new QCLinearInterpolator{ crvPtr });
		//definir un interest rate y meterlo al constructior
		QCZrCpnCrvShrdPtr discZrCrvPtr(new QCZeroCouponInterestRateCurve{ interpol, intRate });

		//Vamos a construir el shared_ptr de QCZeroCouponCurve para la curva de proyeccion
		int puntosCurva2 = curvaProy.RowsInStructure();
		vector<long> tenors2;
		tenors2.resize(puntosCurva2);
		vector<double> rates2;
		rates2.resize(puntosCurva2);
		for (int i = 0; i < puntosCurva2; ++i)
		{
			tenors2.at(i) = static_cast<long>(curvaProy(i, 0).NumericValue());
			rates2.at(i) = curvaProy(i, 1).NumericValue();
		}

		shared_ptr<QCCurve<long>> crvPtr2(new QCCurve<long>{ tenors2, rates2 });
		shared_ptr<QCInterpolator> interpol2(new QCLinearInterpolator{ crvPtr2 });
		//definir un interest rate y meterlo al constructior
		QCIntRtCrvShrdPtr intRtProjCrvPtr(new QCZeroCouponInterestRateCurve{ interpol2, intRate });

		//El shared_ptr de QCTimeSeries lo dejamos nulo
		QCTimeSeriesShrdPtr timeSrsPtr;

		//Construimos el objeto QCDate con la fecha de valorizacion
		QCDate valDate{ fecha }; //constructor que toma un Excel serial

		//Construimos el objeto con los fixing de ICP
		//typedef shared_ptr<map<QCDate, double>> QCTimeSeriesShrdPtr;
		int puntosFixing = pastFixings.RowsInStructure();
		map<QCDate, double> fixings;
		for (int i = 0; i < puntosFixing; ++i)
		{
			QCDate fch = QCDate{ (long)pastFixings(i, 0).NumericValue() };
			fixings.insert(pair<QCDate, double>(fch, pastFixings(i, 1).NumericValue()));
		}
		QCTimeSeriesShrdPtr fixingsShrdPtr = make_shared<map<QCDate, double>>(fixings);

		//Construimos el objeto con los fixing de UF
		//typedef shared_ptr<map<QCDate, double>> QCTimeSeriesShrdPtr;
		int puntosFixingUF = ufFixings.RowsInStructure();
		map<QCDate, double> mapFixingsUF;
		for (int i = 0; i < puntosFixingUF; ++i)
		{
			QCDate fch = QCDate{ (long)ufFixings(i, 0).NumericValue() };
			mapFixingsUF.insert(pair<QCDate, double>(fch, ufFixings(i, 1).NumericValue()));
		}
		QCTimeSeriesShrdPtr ufFixingsShrdPtr = make_shared<map<QCDate, double>>(mapFixingsUF);

		//Finalmente contruimos el payoff
		QCIcpClfPayoff fltRtPff{ intRate2Ptr, addSpread, multSpread, intLegPtr,
			intRtProjCrvPtr, discZrCrvPtr, valDate, fixingsShrdPtr, ufFixingsShrdPtr };
		QCIntrstRtPffShrdPtr fltRtPffPtr = make_shared<QCIcpClfPayoff>(fltRtPff);

		return  fltRtPffPtr->presentValue();
	}

CellMatrix pvFixedLegs(double valueDate,
		CellMatrix holidays,
		CellMatrix curveValues,
		CellMatrix curveCharacteristics,
		CellMatrix legCharacteristics,
		CellMatrix customAmort)
{
	//holidays: nombre, fecha. Se construye un map<string, vector<QCDate>> que para cada nombre de calendario
	//tenga todas las fechas
	map<string, vector<QCDate>> mapHolidays;
	HelperFunctions::buildHolidays(holidays, mapHolidays);

	//curveValues: nombre, tenor (en dias), valor (tasa o df)
	//Se construye un map<string, pair<vector<long>, vector<double>>> que contiene las curvas
	map<string, pair<vector<long>, vector<double>>> crvValues;
	HelperFunctions::buildCurveValues(curveValues, crvValues);

	//Toca ahora construir las curvas con su interpolador y sus convenciones de tasas
	//Primero metemos las caracteristicas de las curvas en un
	//map<string, tuple<string, string, string, string>> = HelperFunctions::string4
	map<string, HelperFunctions::string4> crvChars;
	HelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

	//Para cada uno de los elementos de crvValues debo terminar de dar de alta una curva cero
	//Tengo los plazos, las tasas y los nombres del interpolador, yf, wf y tipo de curva.
	//Necesito entonces llamar una funcion factory que con estos argumentos me de de alta un curva.
	//las curvas que vaya dando de alta las meto en un map
	
	map<string, shared_ptr<QCZeroCouponCurve>> allCurves;

	//Loopeo sobre los keys de crvValues
	for (const auto &curva : crvValues)
	{
		vector<long> tmpLng{ curva.second.first };
		vector<double> tmpDbl{ curva.second.second };
		
		string wf = get<1>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(wf);
		
		string yf = get<2>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(yf);
		
		QCZrCpnCrvShrdPtr tmpCrv = QCFactoryFunctions::zrCpnCrvShrdPtr(
			tmpLng,
			tmpDbl,
			get<0>(crvChars.at(curva.first)),
			wf, yf, get<3>(crvChars.at(curva.first)));
			allCurves.insert(pair <string, shared_ptr<QCZeroCouponCurve>>(curva.first, tmpCrv));
	}

	//Guardaremos el CellMatrix customAmort con los datos de nominal vigente y amortizacion en
	//esta estructura.
	map<unsigned long, vector<tuple<QCDate, double, double>>> dateNotionalAndAmortByIdLeg;
	HelperFunctions::buildCustomAmortization(customAmort, dateNotionalAndAmortByIdLeg);

	//Ahora hay que construir QCInterestRatePayoff para cada operacion
	//Se requiere:
	//	shared_ptr<QCInterestRate>
	//	shared_ptr<QCInterestRateLeg>
	//	QCDate (valueDate)
	//	shared_ptr<QCZeroCouponCurve> (la curva de descuento)
	//	shared_ptr<QCTimeSeries> (los fixings)

	//La info esta en la CellMatriz legCharacteristics:
	//	0:	id_leg
	//	1:	rec_pay
	//	2:	start_date
	//	3:	end_date
	//	4:	settlement_calendar
	//	5:	settlement_lag
	//	6:	stub_period
	//	7:	periodicity
	//	8:	end_date_adjustment
	//	9:	amortization
	//	10: fixed_rate
	//	11: notional
	//	12: wealth_factor
	//	13: year_fraction
	//	14: discount_curve

	QCDate allValueDate{ (long)valueDate };
	map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
	for (unsigned long i = 0; i < legCharacteristics.RowsInStructure(); ++i)
	{
		string wf = legCharacteristics(i, 12).StringValue();
		QCHelperFunctions::lowerCase(wf);
		string yf = legCharacteristics(i, 13).StringValue();
		QCHelperFunctions::lowerCase(yf);
		shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
			(double)legCharacteristics(i, 10).NumericValue(), yf, wf);

		long numOp = (long)legCharacteristics(i, 0).NumericValue();
		double x = 0;
		vector<tuple<QCDate, double, double>> amortIfCustom;
		if (legCharacteristics(i, 9).StringValue() == "CUSTOM")
		{
			amortIfCustom = dateNotionalAndAmortByIdLeg.at(
				(long)legCharacteristics(i, 0).NumericValue());
		}
		QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildFixedRateLeg(
			legCharacteristics(i, 1).StringValue(),					 //receive or pay
			QCDate{ (long)legCharacteristics(i, 2).NumericValue() }, //start date
			QCDate{ (long)legCharacteristics(i, 3).NumericValue() }, //end date
			mapHolidays.at(legCharacteristics(i, 4).StringValue()),  //settlement calendar
			(int)legCharacteristics(i, 5).NumericValue(),			 //settlement lag
			QCHelperFunctions::stringToQCStubPeriod(
			legCharacteristics(i, 6).StringValue()),				 //stub period
			legCharacteristics(i, 7).StringValue(),					 //periodicity
			QCHelperFunctions::stringToQCBusDayAdjRule(
			legCharacteristics(i, 8).StringValue()),				 //end date adjustment
			QCHelperFunctions::stringToQCAmortization(
			legCharacteristics(i, 9).StringValue()),					 //amortization
			amortIfCustom,										//amortization and notional by end date
			(double)legCharacteristics(i, 11).NumericValue()		 //notional
			);
		shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
			new QCFixedRatePayoff{tmpIntRate, make_shared<QCInterestRateLeg>(tmpIntRateLeg),
		allCurves.at(legCharacteristics(i, 14).StringValue()), allValueDate, nullptr});
		payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(
			(long)legCharacteristics(i, 0).NumericValue(),
			tmpIntRatePayoff));
	}

	//Calcular los valores presentes
	vector<tuple<long, double, vector<double>>> result;
	result.resize(payoffs.size());
	double m2m;
	vector<double> der;
	unsigned long counter = 0;
	unsigned int longestCurve = 0;
	for (const auto& payoff : payoffs)
	{
		m2m = payoff.second->presentValue();
		unsigned int vertices = payoff.second->discountCurveLength();
		if (vertices > longestCurve)
			longestCurve = vertices;
		der.resize(vertices);
		for (unsigned long i = 0; i < vertices; ++i)
		{
			der.at(i) = payoff.second->getPvRateDerivativeAt(i);
		}
		result.at(counter) = make_tuple(payoff.first, m2m, der);
		der.clear();
		++counter;
	}

	long cuantosM2M = legCharacteristics.RowsInStructure();
	CellMatrix legM2M(cuantosM2M, longestCurve + 2);
	for (long i = 0; i < cuantosM2M; ++i)
	{
		legM2M(i, 0) = get<0>(result.at(i));
		legM2M(i, 1) = get<1>(result.at(i));
		unsigned int vertices = get<2>(result.at(i)).size();
		for (unsigned int j = 0; j < vertices; ++j)
		{
			legM2M(i, j + 2) = get<2>(result.at(i)).at(j) * BASIS_POINT;
		}
	}
	return legM2M;
}

CellMatrix pvFixedLegs2(double valueDate,
	CellMatrix holidays,
	CellMatrix curveValues,
	CellMatrix curveCharacteristics,
	CellMatrix legCharacteristics,
	CellMatrix customAmort)
{
	//holidays: nombre, fecha. Se construye un map<string, vector<QCDate>> que para cada nombre de calendario
	//tenga todas las fechas
	map<string, vector<QCDate>> mapHolidays;
	HelperFunctions::buildHolidays(holidays, mapHolidays);

	//curveValues: nombre, tenor (en dias), valor (tasa o df)
	//Se construye un map<string, pair<vector<long>, vector<double>>> que contiene las curvas
	map<string, pair<vector<long>, vector<double>>> crvValues;
	HelperFunctions::buildCurveValues(curveValues, crvValues);

	//Toca ahora construir las curvas con su interpolador y sus convenciones de tasas
	//Primero metemos las caracteristicas de las curvas en un
	//map<string, tuple<string, string, string, string>> = HelperFunctions::string4
	map<string, HelperFunctions::string4> crvChars;
	HelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

	//Para cada uno de los elementos de crvValues debo terminar de dar de alta una curva cero
	//Tengo los plazos, las tasas y los nombres del interpolador, yf, wf y tipo de curva.
	//Necesito entonces llamar una funcion factory que con estos argumentos me de de alta un curva.
	//las curvas que vaya dando de alta las meto en un map

	map<string, shared_ptr<QCZeroCouponCurve>> allCurves;

	//Loopeo sobre los keys de crvValues
	for (const auto &curva : crvValues)
	{
		vector<long> tmpLng{ curva.second.first };
		vector<double> tmpDbl{ curva.second.second };

		string wf = get<1>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(wf);

		string yf = get<2>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(yf);

		QCZrCpnCrvShrdPtr tmpCrv = QCFactoryFunctions::zrCpnCrvShrdPtr(
			tmpLng,
			tmpDbl,
			get<0>(crvChars.at(curva.first)),
			wf, yf, get<3>(crvChars.at(curva.first)));
		allCurves.insert(pair <string, shared_ptr<QCZeroCouponCurve>>(curva.first, tmpCrv));
	}

	//Guardaremos el CellMatrix customAmort con los datos de nominal vigente y amortizacion en
	//esta estructura.
	map<unsigned long, vector<tuple<QCDate, double, double>>> dateNotionalAndAmortByIdLeg;
	HelperFunctions::buildCustomAmortization(customAmort, dateNotionalAndAmortByIdLeg);

	//Ahora hay que construir QCInterestRatePayoff para cada operacion
	//Se requiere:
	//	shared_ptr<QCInterestRate>
	//	shared_ptr<QCInterestRateLeg>
	//	QCDate (valueDate)
	//	shared_ptr<QCZeroCouponCurve> (la curva de descuento)
	//	shared_ptr<QCTimeSeries> (los fixings)

	//La info esta en la CellMatriz legCharacteristics:
	//	0:	id_leg
	//	1:	rec_pay
	//	2:	start_date
	//	3:	end_date
	//	4:	settlement_calendar
	//	5:	settlement_lag
	//	6:	stub_period
	//	7:	periodicity
	//	8:	end_date_adjustment
	//	9:	amortization
	//	10: fixed_rate
	//	11: notional
	//	12: wealth_factor
	//	13: year_fraction
	//	14: discount_curve

	QCDate allValueDate{ (long)valueDate };
	map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
	for (unsigned long i = 0; i < legCharacteristics.RowsInStructure(); ++i)
	{
		string wf = legCharacteristics(i, 12).StringValue();
		QCHelperFunctions::lowerCase(wf);
		string yf = legCharacteristics(i, 13).StringValue();
		QCHelperFunctions::lowerCase(yf);
		shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
			(double)legCharacteristics(i, 10).NumericValue(), yf, wf);

		long numOp = (long)legCharacteristics(i, 0).NumericValue();
		double x = 0;
		vector<tuple<QCDate, double, double>> amortIfCustom;
		if (legCharacteristics(i, 9).StringValue() != "BULLET")
		{
			amortIfCustom = dateNotionalAndAmortByIdLeg.at(
				(long)legCharacteristics(i, 0).NumericValue());
		}
		QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildFixedRateLeg2(
			legCharacteristics(i, 1).StringValue(),					 //receive or pay
			QCDate{ (long)legCharacteristics(i, 2).NumericValue() }, //start date
			QCDate{ (long)legCharacteristics(i, 3).NumericValue() }, //end date
			mapHolidays.at(legCharacteristics(i, 4).StringValue()),  //settlement calendar
			(int)legCharacteristics(i, 5).NumericValue(),			 //settlement lag
			QCHelperFunctions::stringToQCStubPeriod(
			legCharacteristics(i, 6).StringValue()),				 //stub period
			legCharacteristics(i, 7).StringValue(),					 //periodicity
			QCHelperFunctions::stringToQCBusDayAdjRule(
			legCharacteristics(i, 8).StringValue()),				 //end date adjustment
			QCHelperFunctions::stringToQCAmortization(
			legCharacteristics(i, 9).StringValue()),					 //amortization
			amortIfCustom,										//amortization and notional by end date
			(double)legCharacteristics(i, 11).NumericValue()		 //notional
			);
		shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
			new QCFixedRatePayoff{ tmpIntRate, make_shared<QCInterestRateLeg>(tmpIntRateLeg),
			allCurves.at(legCharacteristics(i, 14).StringValue()), allValueDate, nullptr });
		payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(
			(long)legCharacteristics(i, 0).NumericValue(),
			tmpIntRatePayoff));
	}

	//Calcular los valores presentes
	vector<tuple<long, double, vector<double>>> result;
	result.resize(payoffs.size());
	double m2m;
	vector<double> der;
	unsigned long counter = 0;
	unsigned int longestCurve = 0;
	for (const auto& payoff : payoffs)
	{
		m2m = payoff.second->presentValue();
		unsigned int vertices = payoff.second->discountCurveLength();
		if (vertices > longestCurve)
			longestCurve = vertices;
		der.resize(vertices);
		for (unsigned long i = 0; i < vertices; ++i)
		{
			der.at(i) = payoff.second->getPvRateDerivativeAt(i);
		}
		result.at(counter) = make_tuple(payoff.first, m2m, der);
		der.clear();
		++counter;
	}

	long cuantosM2M = legCharacteristics.RowsInStructure();
	CellMatrix legM2M(cuantosM2M, longestCurve + 2);
	for (long i = 0; i < cuantosM2M; ++i)
	{
		legM2M(i, 0) = get<0>(result.at(i));
		legM2M(i, 1) = get<1>(result.at(i));
		unsigned int vertices = get<2>(result.at(i)).size();
		for (unsigned int j = 0; j < vertices; ++j)
		{
			legM2M(i, j + 2) = get<2>(result.at(i)).at(j) * BASIS_POINT;
		}
	}
	return legM2M;
}

CellMatrix pvIcpClpLegs(double valueDate,
	CellMatrix holidays,
	CellMatrix curveValues,
	CellMatrix curveCharacteristics,
	CellMatrix legCharacteristics,
	CellMatrix customAmort,
	CellMatrix fixings)
{
	//holidays: nombre, fecha. Se construye un map<string, vector<QCDate>> que para cada nombre de calendario
	//tenga todas las fechas
	map<string, vector<QCDate>> mapHolidays;
	HelperFunctions::buildHolidays(holidays, mapHolidays);

	//curveValues: nombre, tenor (en dias), valor (tasa o df)
	//Se construye un map<string, pair<vector<long>, vector<double>>> que contiene las curvas
	map<string, pair<vector<long>, vector<double>>> crvValues;
	HelperFunctions::buildCurveValues(curveValues, crvValues);

	//Toca ahora construir las curvas con su interpolador y sus convenciones de tasas
	//Primero metemos las caracteristicas de las curvas en un
	//map<string, tuple<string, string, string, string>> = HelperFunctions::string4
	map<string, HelperFunctions::string4> crvChars;
	HelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

	//Para cada uno de los elementos de crvValues debo terminar de dar de alta una curva cero
	//Tengo los plazos, las tasas y los nombres del interpolador, yf, wf y tipo de curva.
	//Necesito entonces llamar una funcion factory que con estos argumentos me de de alta un curva.
	//las curvas que vaya dando de alta las meto en un map

	map<string, shared_ptr<QCInterestRateCurve>> allCurves;

	//Loopeo sobre los keys de crvValues
	for (const auto &curva : crvValues)
	{
		vector<long> tmpLng{ curva.second.first };
		vector<double> tmpDbl{ curva.second.second };

		string wf = get<1>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(wf);

		string yf = get<2>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(yf);

		//Tengo que mirar esta funcion
		shared_ptr<QCInterestRateCurve> tmpCrv = QCFactoryFunctions::intRtCrvShrdPtr(
			tmpLng,
			tmpDbl,
			get<0>(crvChars.at(curva.first)),
			wf, yf, QCHelperFunctions::stringToQCIntRateCurve(get<3>(crvChars.at(curva.first))));
		allCurves.insert(pair<string, shared_ptr<QCInterestRateCurve>>(curva.first, tmpCrv));
	}

	//Guardaremos el CellMatrix customAmort con los datos de nominal vigente y amortizacion en
	//esta estructura.
	map<unsigned long, vector<tuple<QCDate, double, double>>> dateNotionalAndAmortByIdLeg;
	HelperFunctions::buildCustomAmortization(customAmort, dateNotionalAndAmortByIdLeg);

	//Metemos los fixings de ICP en esta estructura
	map<QCDate, double> mapFixings;
	HelperFunctions::buildFixings(fixings, mapFixings);

	//Ahora hay que construir QCInterestRatePayoff para cada operacion
	//Se requiere:
	//	shared_ptr<QCInterestRate>
	//	shared_ptr<QCInterestRateLeg>
	//	QCDate (valueDate)
	//	shared_ptr<QCZeroCouponCurve> (la curva de proyeccion)
	//	shared_ptr<QCZeroCouponCurve> (la curva de descuento)
	//	shared_ptr<QCTimeSeries> (los fixings)

	//La info esta en la CellMatrix legCharacteristics:
	//0:	id_leg					long
	//1:	rec_pay					string
	//2:	start_date				QCDate
	//3:	end_date				QCDate
	//4:	settlement_calendar		string
	//5:	settlement_lag			int
	//6:	stub_period				string
	//7:	periodicity				string
	//8:	end_date_adjustment		string
	//9:	amortization			string
	//10:	interest_rate_index		string
	//11:	rate_value				double
	//12:	spread					double
	//13:	notional_currency		string
	//14:	notional				double
	//15:	wealth_factor			string
	//16:	year_fraction			string
	//17:	projecting_curve		string
	//18:	discount_curve			string
	//19:	fixing_calendar			string

	QCDate allValueDate{ (long)valueDate };
	map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
	for (unsigned long i = 0; i < legCharacteristics.RowsInStructure(); ++i)
	{
		string wf = legCharacteristics(i, 15).StringValue();
		QCHelperFunctions::lowerCase(wf);
		string yf = legCharacteristics(i, 16).StringValue();
		QCHelperFunctions::lowerCase(yf);
		shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
			(double)legCharacteristics(i, 11).NumericValue(), yf, wf);

		long numOp = (long)legCharacteristics(i, 0).NumericValue();
		vector<tuple<QCDate, double, double>> amortIfCustom;
		if (legCharacteristics(i, 9).StringValue() == "CUSTOM")
		{
			amortIfCustom = dateNotionalAndAmortByIdLeg.at(
				(long)legCharacteristics(i, 0).NumericValue());
		}
		QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildIcpLeg(
			legCharacteristics(i, 1).StringValue(),					 //receive or pay
			QCDate{ (long)legCharacteristics(i, 2).NumericValue() }, //start date
			QCDate{ (long)legCharacteristics(i, 3).NumericValue() }, //end date
			mapHolidays.at(legCharacteristics(i, 4).StringValue()),  //settlement calendar
			(int)legCharacteristics(i, 5).NumericValue(),			 //settlement lag
			QCHelperFunctions::stringToQCStubPeriod(
			legCharacteristics(i, 6).StringValue()),					 //stub period
			legCharacteristics(i, 7).StringValue(),					 //periodicity
			QCHelperFunctions::stringToQCBusDayAdjRule(
			legCharacteristics(i, 8).StringValue()),				 //end date adjustment
			QCHelperFunctions::stringToQCAmortization(
			legCharacteristics(i, 9).StringValue()),					 //amortization
			amortIfCustom,											 //amortization and notional by end date
			(double)legCharacteristics(i, 14).NumericValue()		 //notional
			);
		shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
			new QCIcpClpPayoff{ tmpIntRate, legCharacteristics(i, 12).NumericValue(), 1.0,
			make_shared<QCInterestRateLeg>(tmpIntRateLeg), allCurves.at(legCharacteristics(i, 17).StringValue()),
			allCurves.at(legCharacteristics(i, 18).StringValue()), allValueDate,
			make_shared<map<QCDate, double>>(mapFixings) });
		payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(
			(long)legCharacteristics(i, 0).NumericValue(),
			tmpIntRatePayoff));
	}

	//Calcular los valores presentes
	vector<tuple<long, double, vector<double>, vector<double>>> result;
	result.resize(payoffs.size());
	double m2m;
	vector<double> discDer;
	vector<double> projDer;
	unsigned long counter = 0;
	unsigned int longestDiscCurve = 0;
	unsigned int longestProjCurve = 0;
	for (const auto& payoff : payoffs)
	{
		m2m = payoff.second->presentValue();
		unsigned int discVertices = payoff.second->discountCurveLength();
		if (discVertices > longestDiscCurve)
			longestDiscCurve = discVertices;
		discDer.resize(discVertices);
		for (unsigned long i = 0; i < discVertices; ++i)
		{
			discDer.at(i) = payoff.second->getPvRateDerivativeAt(i);
		}

		unsigned int projVertices = payoff.second->projectingCurveLength();
		if (projVertices > longestProjCurve)
			longestProjCurve = projVertices;
		projDer.resize(projVertices);
		for (unsigned long i = 0; i < projVertices; ++i)
		{
			projDer.at(i) = payoff.second->getPvProjRateDerivativeAt(i);
		}
		result.at(counter) = make_tuple(payoff.first, m2m, discDer, projDer);
		++counter;
	}

	long cuantosResultados = legCharacteristics.RowsInStructure();
	CellMatrix legM2MAndDelta(cuantosResultados, longestDiscCurve + longestProjCurve + 2);
	for (long i = 0; i < cuantosResultados; ++i)
	{
		legM2MAndDelta(i, 0) = get<0>(result.at(i));
		legM2MAndDelta(i, 1) = get<1>(result.at(i));
		unsigned int discVertices = get<2>(result.at(i)).size();
		for (unsigned int j = 0; j < discVertices; ++j)
		{
			legM2MAndDelta(i, j + 2) = get<2>(result.at(i)).at(j) * BASIS_POINT;
		}
		unsigned int projVertices = get<3>(result.at(i)).size();
		for (unsigned int j = 0; j < projVertices; ++j)
		{
			legM2MAndDelta(i, j + discVertices + 2) = get<3>(result.at(i)).at(j) * BASIS_POINT;
		}
	}
	return legM2MAndDelta;
}

CellMatrix pvIcpClpLegs2(double valueDate,
	CellMatrix holidays,
	CellMatrix curveValues,
	CellMatrix curveCharacteristics,
	CellMatrix legCharacteristics,
	CellMatrix customAmort,
	CellMatrix fixings)
{
	//holidays: nombre, fecha. Se construye un map<string, vector<QCDate>> que para cada nombre de calendario
	//tenga todas las fechas
	map<string, vector<QCDate>> mapHolidays;
	HelperFunctions::buildHolidays(holidays, mapHolidays);

	//curveValues: nombre, tenor (en dias), valor (tasa o df)
	//Se construye un map<string, pair<vector<long>, vector<double>>> que contiene las curvas
	map<string, pair<vector<long>, vector<double>>> crvValues;
	HelperFunctions::buildCurveValues(curveValues, crvValues);

	//Toca ahora construir las curvas con su interpolador y sus convenciones de tasas
	//Primero metemos las caracteristicas de las curvas en un
	//map<string, tuple<string, string, string, string>> = HelperFunctions::string4
	map<string, HelperFunctions::string4> crvChars;
	HelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

	//Para cada uno de los elementos de crvValues debo terminar de dar de alta una curva cero
	//Tengo los plazos, las tasas y los nombres del interpolador, yf, wf y tipo de curva.
	//Necesito entonces llamar una funcion factory que con estos argumentos me de de alta un curva.
	//las curvas que vaya dando de alta las meto en un map

	map<string, shared_ptr<QCInterestRateCurve>> allCurves;

	//Loopeo sobre los keys de crvValues
	for (const auto &curva : crvValues)
	{
		vector<long> tmpLng{ curva.second.first };
		vector<double> tmpDbl{ curva.second.second };

		string wf = get<1>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(wf);

		string yf = get<2>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(yf);

		//Tengo que mirar esta funcion
		shared_ptr<QCInterestRateCurve> tmpCrv = QCFactoryFunctions::intRtCrvShrdPtr(
			tmpLng,
			tmpDbl,
			get<0>(crvChars.at(curva.first)),
			wf, yf, QCHelperFunctions::stringToQCIntRateCurve(get<3>(crvChars.at(curva.first))));
		allCurves.insert(pair<string, shared_ptr<QCInterestRateCurve>>(curva.first, tmpCrv));
	}

	//Guardaremos el CellMatrix customAmort con los datos de nominal vigente y amortizacion en
	//esta estructura.
	map<unsigned long, vector<tuple<QCDate, double, double>>> dateNotionalAndAmortByIdLeg;
	HelperFunctions::buildCustomAmortization(customAmort, dateNotionalAndAmortByIdLeg);

	//Metemos los fixings de ICP en esta estructura
	map<QCDate, double> mapFixings;
	HelperFunctions::buildFixings(fixings, mapFixings);

	//Ahora hay que construir QCInterestRatePayoff para cada operacion
	//Se requiere:
	//	shared_ptr<QCInterestRate>
	//	shared_ptr<QCInterestRateLeg>
	//	QCDate (valueDate)
	//	shared_ptr<QCZeroCouponCurve> (la curva de proyeccion)
	//	shared_ptr<QCZeroCouponCurve> (la curva de descuento)
	//	shared_ptr<QCTimeSeries> (los fixings)

	//La info esta en la CellMatrix legCharacteristics:
	//0:	id_leg					long
	//1:	rec_pay					string
	//2:	start_date				QCDate
	//3:	end_date				QCDate
	//4:	settlement_calendar		string
	//5:	settlement_lag			int
	//6:	stub_period				string
	//7:	periodicity				string
	//8:	end_date_adjustment		string
	//9:	amortization			string
	//10:	interest_rate_index		string
	//11:	rate_value				double
	//12:	spread					double
	//13:	notional_currency		string
	//14:	notional				double
	//15:	wealth_factor			string
	//16:	year_fraction			string
	//17:	projecting_curve		string
	//18:	discount_curve			string
	//19:	fixing_calendar			string

	QCDate allValueDate{ (long)valueDate };
	map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
	for (unsigned long i = 0; i < legCharacteristics.RowsInStructure(); ++i)
	{
		string wf = legCharacteristics(i, 15).StringValue();
		QCHelperFunctions::lowerCase(wf);
		string yf = legCharacteristics(i, 16).StringValue();
		QCHelperFunctions::lowerCase(yf);
		shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
			(double)legCharacteristics(i, 11).NumericValue(), yf, wf);

		long numOp = (long)legCharacteristics(i, 0).NumericValue();
		vector<tuple<QCDate, double, double>> amortIfCustom;
		if (legCharacteristics(i, 9).StringValue() != "BULLET")
		{
			amortIfCustom = dateNotionalAndAmortByIdLeg.at(
				(long)legCharacteristics(i, 0).NumericValue());
		}
		QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildIcpLeg2(
			legCharacteristics(i, 1).StringValue(),					 //receive or pay
			QCDate{ (long)legCharacteristics(i, 2).NumericValue() }, //start date
			QCDate{ (long)legCharacteristics(i, 3).NumericValue() }, //end date
			mapHolidays.at(legCharacteristics(i, 4).StringValue()),  //settlement calendar
			(int)legCharacteristics(i, 5).NumericValue(),			 //settlement lag
			QCHelperFunctions::stringToQCStubPeriod(
			legCharacteristics(i, 6).StringValue()),				 //stub period
			legCharacteristics(i, 7).StringValue(),					 //periodicity
			QCHelperFunctions::stringToQCBusDayAdjRule(
			legCharacteristics(i, 8).StringValue()),				 //end date adjustment
			QCHelperFunctions::stringToQCAmortization(
			legCharacteristics(i, 9).StringValue()),				 //amortization
			amortIfCustom,											 //amortization and notional by end date
			(double)legCharacteristics(i, 14).NumericValue()		 //notional
			);
		shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
			new QCIcpClpPayoff{ tmpIntRate, legCharacteristics(i, 12).NumericValue(), 1.0,
			make_shared<QCInterestRateLeg>(tmpIntRateLeg), allCurves.at(legCharacteristics(i, 17).StringValue()),
			allCurves.at(legCharacteristics(i, 18).StringValue()), allValueDate,
			make_shared<map<QCDate, double>>(mapFixings) });
		payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(
			(long)legCharacteristics(i, 0).NumericValue(),
			tmpIntRatePayoff));
	}

	//Calcular los valores presentes
	vector<tuple<long, double, vector<double>, vector<double>>> result;
	result.resize(payoffs.size());
	double m2m;
	vector<double> discDer;
	vector<double> projDer;
	unsigned long counter = 0;
	unsigned int longestDiscCurve = 0;
	unsigned int longestProjCurve = 0;
	for (const auto& payoff : payoffs)
	{
		m2m = payoff.second->presentValue();
		unsigned int discVertices = payoff.second->discountCurveLength();
		if (discVertices > longestDiscCurve)
			longestDiscCurve = discVertices;
		discDer.resize(discVertices);
		for (unsigned long i = 0; i < discVertices; ++i)
		{
			discDer.at(i) = payoff.second->getPvRateDerivativeAt(i);
		}

		unsigned int projVertices = payoff.second->projectingCurveLength();
		if (projVertices > longestProjCurve)
			longestProjCurve = projVertices;
		projDer.resize(projVertices);
		for (unsigned long i = 0; i < projVertices; ++i)
		{
			projDer.at(i) = payoff.second->getPvProjRateDerivativeAt(i);
		}
		result.at(counter) = make_tuple(payoff.first, m2m, discDer, projDer);
		++counter;
	}

	long cuantosResultados = legCharacteristics.RowsInStructure();
	CellMatrix legM2MAndDelta(cuantosResultados, longestDiscCurve + longestProjCurve + 2);
	for (long i = 0; i < cuantosResultados; ++i)
	{
		legM2MAndDelta(i, 0) = get<0>(result.at(i));
		legM2MAndDelta(i, 1) = get<1>(result.at(i));
		unsigned int discVertices = get<2>(result.at(i)).size();
		for (unsigned int j = 0; j < discVertices; ++j)
		{
			legM2MAndDelta(i, j + 2) = get<2>(result.at(i)).at(j) * BASIS_POINT;
		}
		unsigned int projVertices = get<3>(result.at(i)).size();
		for (unsigned int j = 0; j < projVertices; ++j)
		{
			legM2MAndDelta(i, j + discVertices + 2) = get<3>(result.at(i)).at(j) * BASIS_POINT;
		}
	}
	
	return legM2MAndDelta;
}

CellMatrix pvIcpClfLegs(double valueDate,
	CellMatrix holidays,
	CellMatrix curveValues,
	CellMatrix curveCharacteristics,
	CellMatrix legCharacteristics,
	CellMatrix customAmort,
	CellMatrix icpFixings,
	CellMatrix ufFixings)
{
	//holidays: nombre, fecha. Se construye un map<string, vector<QCDate>> que para cada nombre de calendario
	//tenga todas las fechas
	map<string, vector<QCDate>> mapHolidays;
	HelperFunctions::buildHolidays(holidays, mapHolidays);

	//curveValues: nombre, tenor (en dias), valor (tasa o df)
	//Se construye un map<string, pair<vector<long>, vector<double>>> que contiene las curvas
	map<string, pair<vector<long>, vector<double>>> crvValues;
	HelperFunctions::buildCurveValues(curveValues, crvValues);

	//Toca ahora construir las curvas con su interpolador y sus convenciones de tasas
	//Primero metemos las caracteristicas de las curvas en un
	//map<string, tuple<string, string, string, string>> = HelperFunctions::string4
	map<string, HelperFunctions::string4> crvChars;
	HelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

	//Para cada uno de los elementos de crvValues debo terminar de dar de alta una curva cero
	//Tengo los plazos, las tasas y los nombres del interpolador, yf, wf y tipo de curva.
	//Necesito entonces llamar una funcion factory que con estos argumentos me de de alta un curva.
	//las curvas que vaya dando de alta las meto en un map

	map<string, shared_ptr<QCInterestRateCurve>> allCurves;

	//Loopeo sobre los keys de crvValues
	for (const auto &curva : crvValues)
	{
		vector<long> tmpLng{ curva.second.first };
		vector<double> tmpDbl{ curva.second.second };

		string wf = get<1>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(wf);

		string yf = get<2>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(yf);

		//Tengo que mirar esta funcion
		shared_ptr<QCInterestRateCurve> tmpCrv = QCFactoryFunctions::intRtCrvShrdPtr(
			tmpLng,
			tmpDbl,
			get<0>(crvChars.at(curva.first)),
			wf, yf, QCHelperFunctions::stringToQCIntRateCurve(get<3>(crvChars.at(curva.first))));
		allCurves.insert(pair<string, shared_ptr<QCInterestRateCurve>>(curva.first, tmpCrv));
	}

	//Guardaremos el CellMatrix customAmort con los datos de nominal vigente y amortizacion en
	//esta estructura.
	map<unsigned long, vector<tuple<QCDate, double, double>>> dateNotionalAndAmortByIdLeg;
	HelperFunctions::buildCustomAmortization(customAmort, dateNotionalAndAmortByIdLeg);

	//Metemos los fixings de ICP en esta estructura
	map<QCDate, double> mapIcpFixings;
	HelperFunctions::buildFixings(icpFixings, mapIcpFixings);

	//Metemos los fixings de UF en esta estructura
	map<QCDate, double> mapUfFixings;
	HelperFunctions::buildFixings(ufFixings, mapUfFixings);

	//Ahora hay que construir QCInterestRatePayoff para cada operacion
	//Se requiere:
	//	shared_ptr<QCInterestRate>
	//	shared_ptr<QCInterestRateLeg>
	//	QCDate (valueDate)
	//	shared_ptr<QCZeroCouponCurve> (la curva de proyeccion)
	//	shared_ptr<QCZeroCouponCurve> (la curva de descuento)
	//	shared_ptr<QCTimeSeries> (los fixings)

	//La info esta en la CellMatrix legCharacteristics:
	//0:	id_leg					long
	//1:	rec_pay					string
	//2:	start_date				QCDate
	//3:	end_date				QCDate
	//4:	settlement_calendar		string
	//5:	settlement_lag			int
	//6:	stub_period				string
	//7:	periodicity				string
	//8:	end_date_adjustment		string
	//9:	amortization			string
	//10:	interest_rate_index		string
	//11:	rate_value				double
	//12:	spread					double
	//13:	notional_currency		string
	//14:	notional				double
	//15:	wealth_factor			string
	//16:	year_fraction			string
	//17:	projecting_curve		string
	//18:	discount_curve			string
	//19:	fixing_calendar			string

	QCDate allValueDate{ (long)valueDate };
	map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
	for (unsigned long i = 0; i < legCharacteristics.RowsInStructure(); ++i)
	{
		string wf = legCharacteristics(i, 15).StringValue();
		QCHelperFunctions::lowerCase(wf);
		string yf = legCharacteristics(i, 16).StringValue();
		QCHelperFunctions::lowerCase(yf);
		shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
			(double)legCharacteristics(i, 11).NumericValue(), yf, wf);

		long numOp = (long)legCharacteristics(i, 0).NumericValue();
		vector<tuple<QCDate, double, double>> amortIfCustom;
		if (legCharacteristics(i, 9).StringValue() == "CUSTOM")
		{
			amortIfCustom = dateNotionalAndAmortByIdLeg.at(
				(long)legCharacteristics(i, 0).NumericValue());
		}
		QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildIcpLeg(
			legCharacteristics(i, 1).StringValue(),						//receive or pay
			QCDate{ (long)legCharacteristics(i, 2).NumericValue() },	//start date
			QCDate{ (long)legCharacteristics(i, 3).NumericValue() },	//end date
			mapHolidays.at(legCharacteristics(i, 4).StringValue()),		//settlement calendar
			(int)legCharacteristics(i, 5).NumericValue(),		//settlement lag
			QCHelperFunctions::stringToQCStubPeriod(
			legCharacteristics(i, 6).StringValue()),			//stub period
			legCharacteristics(i, 7).StringValue(),				//periodicity
			QCHelperFunctions::stringToQCBusDayAdjRule(
			legCharacteristics(i, 8).StringValue()),			//end date adjustment
			QCHelperFunctions::stringToQCAmortization(
			legCharacteristics(i, 9).StringValue()),			//amortization
			amortIfCustom,										//amortization and notional by end date
			(double)legCharacteristics(i, 14).NumericValue()	//notional
			);
		shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
			new QCIcpClfPayoff{ tmpIntRate, legCharacteristics(i, 12).NumericValue(), 1.0,
			make_shared<QCInterestRateLeg>(tmpIntRateLeg), allCurves.at(legCharacteristics(i, 17).StringValue()),
			allCurves.at(legCharacteristics(i, 18).StringValue()), allValueDate,
			make_shared<map<QCDate, double>>(mapIcpFixings),
			make_shared<map<QCDate, double>>(mapUfFixings) });
		payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(
			(long)legCharacteristics(i, 0).NumericValue(),
			tmpIntRatePayoff));
	}

	//Calcular los valores presentes
	vector<tuple<long, double, vector<double>, vector<double>>> result;
	result.resize(payoffs.size());
	double m2m;
	vector<double> discDer;
	vector<double> projDer;
	unsigned long counter = 0;
	unsigned int longestDiscCurve = 0;
	unsigned int longestProjCurve = 0;
	for (const auto& payoff : payoffs)
	{
		m2m = payoff.second->presentValue();
		unsigned int discVertices = payoff.second->discountCurveLength();
		if (discVertices > longestDiscCurve)
			longestDiscCurve = discVertices;
		discDer.resize(discVertices);
		for (unsigned long i = 0; i < discVertices; ++i)
		{
			discDer.at(i) = payoff.second->getPvRateDerivativeAt(i);
		}

		unsigned int projVertices = payoff.second->projectingCurveLength();
		if (projVertices > longestProjCurve)
			longestProjCurve = projVertices;
		projDer.resize(projVertices);
		for (unsigned long i = 0; i < projVertices; ++i)
		{
			projDer.at(i) = payoff.second->getPvProjRateDerivativeAt(i);
		}
		result.at(counter) = make_tuple(payoff.first, m2m, discDer, projDer);
		++counter;
	}

	long cuantosResultados = legCharacteristics.RowsInStructure();
	CellMatrix legM2MAndDelta(cuantosResultados, longestDiscCurve + longestProjCurve + 2);
	for (long i = 0; i < cuantosResultados; ++i)
	{
		legM2MAndDelta(i, 0) = get<0>(result.at(i));
		legM2MAndDelta(i, 1) = get<1>(result.at(i));
		unsigned int discVertices = get<2>(result.at(i)).size();
		for (unsigned int j = 0; j < discVertices; ++j)
		{
			legM2MAndDelta(i, j + 2) = get<2>(result.at(i)).at(j) * BASIS_POINT;
		}
		unsigned int projVertices = get<3>(result.at(i)).size();
		for (unsigned int j = 0; j < projVertices; ++j)
		{
			legM2MAndDelta(i, j + discVertices + 2) = get<3>(result.at(i)).at(j) * BASIS_POINT;
		}
	}
	return legM2MAndDelta;
}

CellMatrix pvIcpClfLegs2(double valueDate,
	CellMatrix holidays,
	CellMatrix curveValues,
	CellMatrix curveCharacteristics,
	CellMatrix legCharacteristics,
	CellMatrix customAmort,
	CellMatrix icpFixings,
	CellMatrix ufFixings)
{
	//holidays: nombre, fecha. Se construye un map<string, vector<QCDate>> que para cada nombre de calendario
	//tenga todas las fechas
	map<string, vector<QCDate>> mapHolidays;
	HelperFunctions::buildHolidays(holidays, mapHolidays);

	//curveValues: nombre, tenor (en dias), valor (tasa o df)
	//Se construye un map<string, pair<vector<long>, vector<double>>> que contiene las curvas
	map<string, pair<vector<long>, vector<double>>> crvValues;
	HelperFunctions::buildCurveValues(curveValues, crvValues);

	//Toca ahora construir las curvas con su interpolador y sus convenciones de tasas
	//Primero metemos las caracteristicas de las curvas en un
	//map<string, tuple<string, string, string, string>> = HelperFunctions::string4
	map<string, HelperFunctions::string4> crvChars;
	HelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

	//Para cada uno de los elementos de crvValues debo terminar de dar de alta una curva cero
	//Tengo los plazos, las tasas y los nombres del interpolador, yf, wf y tipo de curva.
	//Necesito entonces llamar una funcion factory que con estos argumentos me de de alta un curva.
	//las curvas que vaya dando de alta las meto en un map

	map<string, shared_ptr<QCInterestRateCurve>> allCurves;

	//Loopeo sobre los keys de crvValues
	for (const auto &curva : crvValues)
	{
		vector<long> tmpLng{ curva.second.first };
		vector<double> tmpDbl{ curva.second.second };

		string wf = get<1>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(wf);

		string yf = get<2>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(yf);

		//Tengo que mirar esta funcion
		shared_ptr<QCInterestRateCurve> tmpCrv = QCFactoryFunctions::intRtCrvShrdPtr(
			tmpLng,
			tmpDbl,
			get<0>(crvChars.at(curva.first)),
			wf, yf, QCHelperFunctions::stringToQCIntRateCurve(get<3>(crvChars.at(curva.first))));
		allCurves.insert(pair<string, shared_ptr<QCInterestRateCurve>>(curva.first, tmpCrv));
	}

	//Guardaremos el CellMatrix customAmort con los datos de nominal vigente y amortizacion en
	//esta estructura.
	map<unsigned long, vector<tuple<QCDate, double, double>>> dateNotionalAndAmortByIdLeg;
	HelperFunctions::buildCustomAmortization(customAmort, dateNotionalAndAmortByIdLeg);

	//Metemos los fixings de ICP en esta estructura
	map<QCDate, double> mapIcpFixings;
	HelperFunctions::buildFixings(icpFixings, mapIcpFixings);

	//Metemos los fixings de UF en esta estructura
	map<QCDate, double> mapUfFixings;
	HelperFunctions::buildFixings(ufFixings, mapUfFixings);

	//Ahora hay que construir QCInterestRatePayoff para cada operacion
	//Se requiere:
	//	shared_ptr<QCInterestRate>
	//	shared_ptr<QCInterestRateLeg>
	//	QCDate (valueDate)
	//	shared_ptr<QCZeroCouponCurve> (la curva de proyeccion)
	//	shared_ptr<QCZeroCouponCurve> (la curva de descuento)
	//	shared_ptr<QCTimeSeries> (los fixings)

	//La info esta en la CellMatrix legCharacteristics:
	//0:	id_leg					long
	//1:	rec_pay					string
	//2:	start_date				QCDate
	//3:	end_date				QCDate
	//4:	settlement_calendar		string
	//5:	settlement_lag			int
	//6:	stub_period				string
	//7:	periodicity				string
	//8:	end_date_adjustment		string
	//9:	amortization			string
	//10:	interest_rate_index		string
	//11:	rate_value				double
	//12:	spread					double
	//13:	notional_currency		string
	//14:	notional				double
	//15:	wealth_factor			string
	//16:	year_fraction			string
	//17:	projecting_curve		string
	//18:	discount_curve			string
	//19:	fixing_calendar			string

	QCDate allValueDate{ (long)valueDate };
	map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
	for (unsigned long i = 0; i < legCharacteristics.RowsInStructure(); ++i)
	{
		string wf = legCharacteristics(i, 15).StringValue();
		QCHelperFunctions::lowerCase(wf);
		string yf = legCharacteristics(i, 16).StringValue();
		QCHelperFunctions::lowerCase(yf);
		shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
			(double)legCharacteristics(i, 11).NumericValue(), yf, wf);

		long numOp = (long)legCharacteristics(i, 0).NumericValue();
		vector<tuple<QCDate, double, double>> amortIfCustom;
		if (legCharacteristics(i, 9).StringValue() != "BULLET")
		{
			amortIfCustom = dateNotionalAndAmortByIdLeg.at(
				(long)legCharacteristics(i, 0).NumericValue());
		}
		QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildIcpLeg2(
			legCharacteristics(i, 1).StringValue(),						//receive or pay
			QCDate{ (long)legCharacteristics(i, 2).NumericValue() },	//start date
			QCDate{ (long)legCharacteristics(i, 3).NumericValue() },	//end date
			mapHolidays.at(legCharacteristics(i, 4).StringValue()),		//settlement calendar
			(int)legCharacteristics(i, 5).NumericValue(),		//settlement lag
			QCHelperFunctions::stringToQCStubPeriod(
			legCharacteristics(i, 6).StringValue()),			//stub period
			legCharacteristics(i, 7).StringValue(),				//periodicity
			QCHelperFunctions::stringToQCBusDayAdjRule(
			legCharacteristics(i, 8).StringValue()),			//end date adjustment
			QCHelperFunctions::stringToQCAmortization(
			legCharacteristics(i, 9).StringValue()),			//amortization
			amortIfCustom,										//amortization and notional by end date
			(double)legCharacteristics(i, 14).NumericValue()	//notional
			);
		shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
			new QCIcpClfPayoff{ tmpIntRate, legCharacteristics(i, 12).NumericValue(), 1.0,
			make_shared<QCInterestRateLeg>(tmpIntRateLeg), allCurves.at(legCharacteristics(i, 17).StringValue()),
			allCurves.at(legCharacteristics(i, 18).StringValue()), allValueDate,
			make_shared<map<QCDate, double>>(mapIcpFixings),
			make_shared<map<QCDate, double>>(mapUfFixings) });
		payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(
			(long)legCharacteristics(i, 0).NumericValue(),
			tmpIntRatePayoff));
	}

	//Calcular los valores presentes
	vector<tuple<long, double, vector<double>, vector<double>>> result;
	result.resize(payoffs.size());
	double m2m;
	vector<double> discDer;
	vector<double> projDer;
	unsigned long counter = 0;
	unsigned int longestDiscCurve = 0;
	unsigned int longestProjCurve = 0;
	for (const auto& payoff : payoffs)
	{
		m2m = payoff.second->presentValue();
		unsigned int discVertices = payoff.second->discountCurveLength();
		if (discVertices > longestDiscCurve)
			longestDiscCurve = discVertices;
		discDer.resize(discVertices);
		for (unsigned long i = 0; i < discVertices; ++i)
		{
			discDer.at(i) = payoff.second->getPvRateDerivativeAt(i);
		}

		unsigned int projVertices = payoff.second->projectingCurveLength();
		if (projVertices > longestProjCurve)
			longestProjCurve = projVertices;
		projDer.resize(projVertices);
		for (unsigned long i = 0; i < projVertices; ++i)
		{
			projDer.at(i) = payoff.second->getPvProjRateDerivativeAt(i);
		}
		result.at(counter) = make_tuple(payoff.first, m2m, discDer, projDer);
		++counter;
	}

	long cuantosResultados = legCharacteristics.RowsInStructure();
	CellMatrix legM2MAndDelta(cuantosResultados, longestDiscCurve + longestProjCurve + 2);
	for (long i = 0; i < cuantosResultados; ++i)
	{
		legM2MAndDelta(i, 0) = get<0>(result.at(i));
		legM2MAndDelta(i, 1) = get<1>(result.at(i));
		unsigned int discVertices = get<2>(result.at(i)).size();
		for (unsigned int j = 0; j < discVertices; ++j)
		{
			legM2MAndDelta(i, j + 2) = get<2>(result.at(i)).at(j) * BASIS_POINT;
		}
		unsigned int projVertices = get<3>(result.at(i)).size();
		for (unsigned int j = 0; j < projVertices; ++j)
		{
			legM2MAndDelta(i, j + discVertices + 2) = get<3>(result.at(i)).at(j) * BASIS_POINT;
		}
	}
	return legM2MAndDelta;
}

CellMatrix pvFloatingRateLegs(double valueDate,
	CellMatrix holidays,
	CellMatrix curveValues,
	CellMatrix curveCharacteristics,
	CellMatrix legCharacteristics,
	CellMatrix customAmort,
	CellMatrix fixings,
	CellMatrix intRateIndexChars)
{
	//holidays: nombre, fecha. Se construye un map<string, vector<QCDate>> que para cada nombre de calendario
	//tenga todas las fechas
	map<string, vector<QCDate>> mapHolidays;
	HelperFunctions::buildHolidays(holidays, mapHolidays);

	//curveValues: nombre, tenor (en dias), valor (tasa o df)
	//Se construye un map<string, pair<vector<long>, vector<double>>> que contiene las curvas
	map<string, pair<vector<long>, vector<double>>> crvValues;
	HelperFunctions::buildCurveValues(curveValues, crvValues);

	//Toca ahora construir las curvas con su interpolador y sus convenciones de tasas
	//Primero metemos las caracteristicas de las curvas en un
	//map<string, tuple<string, string, string, string>> = HelperFunctions::string4
	map<string, HelperFunctions::string4> crvChars;
	HelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

	//Para cada uno de los elementos de crvValues debo terminar de dar de alta una curva cero
	//Tengo los plazos, las tasas y los nombres del interpolador, yf, wf y tipo de curva.
	//Necesito entonces llamar una funcion factory que con estos argumentos me de de alta un curva.
	//las curvas que vaya dando de alta las meto en un map

	map<string, shared_ptr<QCInterestRateCurve>> allCurves;

	//Loopeo sobre los keys de crvValues
	for (const auto &curva : crvValues)
	{
		vector<long> tmpLng{ curva.second.first };
		vector<double> tmpDbl{ curva.second.second };

		string wf = get<1>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(wf);

		string yf = get<2>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(yf);

		//Tengo que mirar esta funcion
		shared_ptr<QCInterestRateCurve> tmpCrv = QCFactoryFunctions::intRtCrvShrdPtr(
			tmpLng,
			tmpDbl,
			get<0>(crvChars.at(curva.first)),
			wf, yf, QCHelperFunctions::stringToQCIntRateCurve(get<3>(crvChars.at(curva.first))));
		allCurves.insert(pair<string, shared_ptr<QCInterestRateCurve>>(curva.first, tmpCrv));
	}

	//Guardaremos el CellMatrix customAmort con los datos de nominal vigente y amortizacion en
	//esta estructura.
	map<unsigned long, vector<tuple<QCDate, double, double>>> dateNotionalAndAmortByIdLeg;
	HelperFunctions::buildCustomAmortization(customAmort, dateNotionalAndAmortByIdLeg);

	//Metemos los fixings de los indices de tasa flotante en esta estructura
	map<string, map<QCDate, double>> mapManyFixings;
	HelperFunctions::buildManyFixings(fixings, mapManyFixings);

	//Metemos las caracteristicas de los indices en esta estructura
	map<string, pair<string, string>> indexChars; //en el pair viene el tenor y el start date rule
	HelperFunctions::buildStringPairStringMap(intRateIndexChars, indexChars);

	//Ahora hay que construir QCInterestRatePayoff para cada operacion
	//Se requiere:
	//	shared_ptr<QCInterestRate>
	//	shared_ptr<QCInterestRateLeg>
	//	QCDate (valueDate)
	//	shared_ptr<QCInterestRateCurve> (la curva de proyeccion)
	//	shared_ptr<QCInterestCurve> (la curva de descuento)
	//	shared_ptr<QCTimeSeries> (los fixings)

	//La info esta en la CellMatrix legCharacteristics:
	//0:	id_leg
	//1:	rec_pay
	//2:	start_date
	//3:	end_date
	//4:	settlement_calendar
	//5:	settlement_lag
	//6:	stub_period
	//7:	periodicity
	//8:	end_date_adjustment
	//9:	amortization
	//10:	interest_rate_index
	//11:	rate_value
	//12:	spread
	//13:	fixing_stub_period
	//14:	fixing_periodicity
	//15:	fixing_calendar
	//16:	fixing_lag
	//17:	notional_currency
	//18:	notional
	//19:	wealth_factor
	//20:	year_fraction
	//21:	projecting_curve
	//22:	discount_curve

	QCDate allValueDate{ (long)valueDate };
	map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
	for (unsigned long i = 0; i < legCharacteristics.RowsInStructure(); ++i)
	{
		string wf = legCharacteristics(i, 19).StringValue();
		QCHelperFunctions::lowerCase(wf);
		string yf = legCharacteristics(i, 20).StringValue();
		QCHelperFunctions::lowerCase(yf);
		shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
			(double)legCharacteristics(i, 11).NumericValue(), yf, wf);

		long numOp = (long)legCharacteristics(i, 0).NumericValue();
		vector<tuple<QCDate, double, double>> amortIfCustom;
		if (legCharacteristics(i, 9).StringValue() == "CUSTOM")
		{
			amortIfCustom = dateNotionalAndAmortByIdLeg.at(
				(long)legCharacteristics(i, 0).NumericValue());
		}
		QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildFloatingRateLeg(
			legCharacteristics(i, 1).StringValue(),					 //receive or pay
			QCDate{ (long)legCharacteristics(i, 2).NumericValue() }, //start date
			QCDate{ (long)legCharacteristics(i, 3).NumericValue() }, //end date
			mapHolidays.at(legCharacteristics(i, 4).StringValue()),  //settlement calendar
			(int)legCharacteristics(i, 5).NumericValue(),			 //settlement lag
			QCHelperFunctions::stringToQCStubPeriod(
			legCharacteristics(i, 6).StringValue()),				 //stub period
			legCharacteristics(i, 7).StringValue(),					 //periodicity
			QCHelperFunctions::stringToQCBusDayAdjRule(
			legCharacteristics(i, 8).StringValue()),				 //end date adjustment
			QCHelperFunctions::stringToQCAmortization(
			legCharacteristics(i, 9).StringValue()),				 //amortization
			amortIfCustom,											 //amortization and notional by end date
			(int)legCharacteristics(i, 16).NumericValue(),			 //fixing lag
			QCHelperFunctions::stringToQCStubPeriod(
			legCharacteristics(i, 13).StringValue()),				 //fixing stub period
			legCharacteristics(i, 14).StringValue(),				 //fixing periodicity
			mapHolidays.at(legCharacteristics(i, 15).StringValue()), //fixing calendar
			indexChars.at(legCharacteristics(i, 10).StringValue()),  //interest rate index tenor
			(double)legCharacteristics(i, 18).NumericValue()		 //notional
			);
		shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
			new QCFloatingRatePayoff{tmpIntRate,
			legCharacteristics(i, 12).NumericValue(), 1.0,
			make_shared<QCInterestRateLeg>(tmpIntRateLeg),
			allCurves.at(legCharacteristics(i, 21).StringValue()),
			allCurves.at(legCharacteristics(i, 22).StringValue()),
			allValueDate,
			make_shared<map<QCDate, double>>(mapManyFixings.at(legCharacteristics(i, 10).StringValue()))});
		
		payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(
			(long)legCharacteristics(i, 0).NumericValue(),
			tmpIntRatePayoff));
	}

	//Calcular los valores presentes
	vector<tuple<long, double, vector<double>, vector<double>>> result;
	result.resize(payoffs.size());
	double m2m;
	vector<double> discDer;
	vector<double> projDer;
	unsigned long counter = 0;
	unsigned int longestDiscCurve = 0;
	unsigned int longestProjCurve = 0;
	for (const auto& payoff : payoffs)
	{
		m2m = payoff.second->presentValue();
		unsigned int discVertices = payoff.second->discountCurveLength();
		if (discVertices > longestDiscCurve)
			longestDiscCurve = discVertices;
		discDer.resize(discVertices);
		for (unsigned long i = 0; i < discVertices; ++i)
		{
			discDer.at(i) = payoff.second->getPvRateDerivativeAt(i);
		}

		unsigned int projVertices = payoff.second->projectingCurveLength();
		if (projVertices > longestProjCurve)
			longestProjCurve = projVertices;
		projDer.resize(projVertices);
		for (unsigned long i = 0; i < projVertices; ++i)
		{
			projDer.at(i) = payoff.second->getPvProjRateDerivativeAt(i);
		}
		result.at(counter) = make_tuple(payoff.first, m2m, discDer, projDer);
		++counter;
	}

	long cuantosResultados = legCharacteristics.RowsInStructure();
	CellMatrix legM2MAndDelta(cuantosResultados, longestDiscCurve + longestProjCurve + 2);
	for (long i = 0; i < cuantosResultados; ++i)
	{
		legM2MAndDelta(i, 0) = get<0>(result.at(i));
		legM2MAndDelta(i, 1) = get<1>(result.at(i));
		unsigned int discVertices = get<2>(result.at(i)).size();
		for (unsigned int j = 0; j < discVertices; ++j)
		{
			legM2MAndDelta(i, j + 2) = get<2>(result.at(i)).at(j) * BASIS_POINT;
		}
		unsigned int projVertices = get<3>(result.at(i)).size();
		for (unsigned int j = 0; j < projVertices; ++j)
		{
			legM2MAndDelta(i, j + discVertices + 2) = get<3>(result.at(i)).at(j) * BASIS_POINT;
		}
	}
	return legM2MAndDelta;

}

CellMatrix pvFloatingRateLegs2(double valueDate,
	CellMatrix holidays,
	CellMatrix curveValues,
	CellMatrix curveCharacteristics,
	CellMatrix legCharacteristics,
	CellMatrix customAmort,
	CellMatrix fixings,
	CellMatrix intRateIndexChars)
{
	//holidays: nombre, fecha. Se construye un map<string, vector<QCDate>> que para cada nombre de calendario
	//tenga todas las fechas
	map<string, vector<QCDate>> mapHolidays;
	HelperFunctions::buildHolidays(holidays, mapHolidays);

	//curveValues: nombre, tenor (en dias), valor (tasa o df)
	//Se construye un map<string, pair<vector<long>, vector<double>>> que contiene las curvas
	map<string, pair<vector<long>, vector<double>>> crvValues;
	HelperFunctions::buildCurveValues(curveValues, crvValues);

	//Toca ahora construir las curvas con su interpolador y sus convenciones de tasas
	//Primero metemos las caracteristicas de las curvas en un
	//map<string, tuple<string, string, string, string>> = HelperFunctions::string4
	map<string, HelperFunctions::string4> crvChars;
	HelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

	//Para cada uno de los elementos de crvValues debo terminar de dar de alta una curva cero
	//Tengo los plazos, las tasas y los nombres del interpolador, yf, wf y tipo de curva.
	//Necesito entonces llamar una funcion factory que con estos argumentos me de de alta un curva.
	//las curvas que vaya dando de alta las meto en un map

	map<string, shared_ptr<QCInterestRateCurve>> allCurves;

	//Loopeo sobre los keys de crvValues
	for (const auto &curva : crvValues)
	{
		vector<long> tmpLng{ curva.second.first };
		vector<double> tmpDbl{ curva.second.second };

		string wf = get<1>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(wf);

		string yf = get<2>(crvChars.at(curva.first));
		QCHelperFunctions::lowerCase(yf);

		//Tengo que mirar esta funcion
		shared_ptr<QCInterestRateCurve> tmpCrv = QCFactoryFunctions::intRtCrvShrdPtr(
			tmpLng,
			tmpDbl,
			get<0>(crvChars.at(curva.first)),
			wf, yf, QCHelperFunctions::stringToQCIntRateCurve(get<3>(crvChars.at(curva.first))));
		allCurves.insert(pair<string, shared_ptr<QCInterestRateCurve>>(curva.first, tmpCrv));
	}

	//Guardaremos el CellMatrix customAmort con los datos de nominal vigente y amortizacion en
	//esta estructura.
	map<unsigned long, vector<tuple<QCDate, double, double>>> dateNotionalAndAmortByIdLeg;
	HelperFunctions::buildCustomAmortization(customAmort, dateNotionalAndAmortByIdLeg);

	//Metemos los fixings de los indices de tasa flotante en esta estructura
	map<string, map<QCDate, double>> mapManyFixings;
	HelperFunctions::buildManyFixings(fixings, mapManyFixings);

	//Metemos las caracteristicas de los indices en esta estructura
	map<string, pair<string, string>> indexChars; //en el pair viene el tenor y el start date rule
	HelperFunctions::buildStringPairStringMap(intRateIndexChars, indexChars);

	//Ahora hay que construir QCInterestRatePayoff para cada operacion
	//Se requiere:
	//	shared_ptr<QCInterestRate>
	//	shared_ptr<QCInterestRateLeg>
	//	QCDate (valueDate)
	//	shared_ptr<QCInterestRateCurve> (la curva de proyeccion)
	//	shared_ptr<QCInterestCurve> (la curva de descuento)
	//	shared_ptr<QCTimeSeries> (los fixings)

	//La info esta en la CellMatrix legCharacteristics:
	//0:	id_leg
	//1:	rec_pay
	//2:	start_date
	//3:	end_date
	//4:	settlement_calendar
	//5:	settlement_lag
	//6:	stub_period
	//7:	periodicity
	//8:	end_date_adjustment
	//9:	amortization
	//10:	interest_rate_index
	//11:	rate_value
	//12:	spread
	//13:	fixing_stub_period
	//14:	fixing_periodicity
	//15:	fixing_calendar
	//16:	fixing_lag
	//17:	notional_currency
	//18:	notional
	//19:	wealth_factor
	//20:	year_fraction
	//21:	projecting_curve
	//22:	discount_curve

	QCDate allValueDate{ (long)valueDate };
	map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
	for (unsigned long i = 0; i < legCharacteristics.RowsInStructure(); ++i)
	{
		string wf = legCharacteristics(i, 19).StringValue();
		QCHelperFunctions::lowerCase(wf);
		string yf = legCharacteristics(i, 20).StringValue();
		QCHelperFunctions::lowerCase(yf);
		shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
			(double)legCharacteristics(i, 11).NumericValue(), yf, wf);

		long numOp = (long)legCharacteristics(i, 0).NumericValue();
		vector<tuple<QCDate, double, double>> amortIfCustom;
		if (legCharacteristics(i, 9).StringValue() != "BULLET")
		{
			amortIfCustom = dateNotionalAndAmortByIdLeg.at(
				(long)legCharacteristics(i, 0).NumericValue());
		}
		QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildFloatingRateLeg2(
			legCharacteristics(i, 1).StringValue(),					 //receive or pay
			QCDate{ (long)legCharacteristics(i, 2).NumericValue() }, //start date
			QCDate{ (long)legCharacteristics(i, 3).NumericValue() }, //end date
			mapHolidays.at(legCharacteristics(i, 4).StringValue()),  //settlement calendar
			(int)legCharacteristics(i, 5).NumericValue(),			 //settlement lag
			QCHelperFunctions::stringToQCStubPeriod(
			legCharacteristics(i, 6).StringValue()),				 //stub period
			legCharacteristics(i, 7).StringValue(),					 //periodicity
			QCHelperFunctions::stringToQCBusDayAdjRule(
			legCharacteristics(i, 8).StringValue()),				 //end date adjustment
			QCHelperFunctions::stringToQCAmortization(
			legCharacteristics(i, 9).StringValue()),				 //amortization
			amortIfCustom,											 //amortization and notional by end date
			(int)legCharacteristics(i, 16).NumericValue(),			 //fixing lag
			QCHelperFunctions::stringToQCStubPeriod(
			legCharacteristics(i, 13).StringValue()),				 //fixing stub period
			legCharacteristics(i, 14).StringValue(),				 //fixing periodicity
			mapHolidays.at(legCharacteristics(i, 15).StringValue()), //fixing calendar
			indexChars.at(legCharacteristics(i, 10).StringValue()),  //interest rate index tenor
			(double)legCharacteristics(i, 18).NumericValue()		 //notional
			);
		shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
			new QCFloatingRatePayoff{ tmpIntRate,
			legCharacteristics(i, 12).NumericValue(), 1.0,
			make_shared<QCInterestRateLeg>(tmpIntRateLeg),
			allCurves.at(legCharacteristics(i, 21).StringValue()),
			allCurves.at(legCharacteristics(i, 22).StringValue()),
			allValueDate,
			make_shared<map<QCDate, double>>(mapManyFixings.at(legCharacteristics(i, 10).StringValue())) });

		payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(
			(long)legCharacteristics(i, 0).NumericValue(),
			tmpIntRatePayoff));
	}

	//Calcular los valores presentes
	vector<tuple<long, double, vector<double>, vector<double>>> result;
	result.resize(payoffs.size());
	double m2m;
	vector<double> discDer;
	vector<double> projDer;
	unsigned long counter = 0;
	unsigned int longestDiscCurve = 0;
	unsigned int longestProjCurve = 0;
	for (const auto& payoff : payoffs)
	{
		m2m = payoff.second->presentValue();
		unsigned int discVertices = payoff.second->discountCurveLength();
		if (discVertices > longestDiscCurve)
			longestDiscCurve = discVertices;
		discDer.resize(discVertices);
		for (unsigned long i = 0; i < discVertices; ++i)
		{
			discDer.at(i) = payoff.second->getPvRateDerivativeAt(i);
		}

		unsigned int projVertices = payoff.second->projectingCurveLength();
		if (projVertices > longestProjCurve)
			longestProjCurve = projVertices;
		projDer.resize(projVertices);
		for (unsigned long i = 0; i < projVertices; ++i)
		{
			projDer.at(i) = payoff.second->getPvProjRateDerivativeAt(i);
		}
		result.at(counter) = make_tuple(payoff.first, m2m, discDer, projDer);
		++counter;
	}

	long cuantosResultados = legCharacteristics.RowsInStructure();
	CellMatrix legM2MAndDelta(cuantosResultados, longestDiscCurve + longestProjCurve + 2);
	for (long i = 0; i < cuantosResultados; ++i)
	{
		legM2MAndDelta(i, 0) = get<0>(result.at(i));
		legM2MAndDelta(i, 1) = get<1>(result.at(i));
		unsigned int discVertices = get<2>(result.at(i)).size();
		for (unsigned int j = 0; j < discVertices; ++j)
		{
			legM2MAndDelta(i, j + 2) = get<2>(result.at(i)).at(j) * BASIS_POINT;
		}
		unsigned int projVertices = get<3>(result.at(i)).size();
		for (unsigned int j = 0; j < projVertices; ++j)
		{
			legM2MAndDelta(i, j + discVertices + 2) = get<3>(result.at(i)).at(j) * BASIS_POINT;
		}
	}
	return legM2MAndDelta;

}

CellMatrix buildInterestRateLeg(double startDate,
	double endDate,
	CellMatrix calendars,
	string settlementStubPeriod,
	string settlementPeriodicity,
	string endDateAdjustment,
	string settlementCalendar,
	int settlementLag,
	string fixingStubPeriod,
	string fixingPeriodicity,
	int fixingLag,
	string fixingCalendar,
	int fixingStartDateRule,
	string fixingTenor)
{
	//holidays: nombre, fecha. Se construye un map<string, vector<QCDate>> que para cada nombre de calendario
	//tenga todas las fechas
	map<string, vector<QCDate>> mapHolidays;
	HelperFunctions::buildHolidays(calendars, mapHolidays);

	QCInterestRatePeriodsFactory factory{ QCDate{ (long)startDate }, QCDate{ (long)endDate },
		QCHelperFunctions::stringToQCBusDayAdjRule(endDateAdjustment), settlementPeriodicity,
		QCHelperFunctions::stringToQCStubPeriod(settlementStubPeriod),
		make_shared<vector<QCDate>>(mapHolidays.at(settlementCalendar)),
		(unsigned int)settlementLag, fixingPeriodicity, QCHelperFunctions::stringToQCStubPeriod(fixingStubPeriod),
		make_shared<vector<QCDate>>(mapHolidays.at(fixingCalendar)), (unsigned int)fixingLag,
		(unsigned int)fixingStartDateRule, fixingTenor };

	auto periods = factory.getPeriods();

	//Hay que transformar periods a CellMatrix y retornar
	CellMatrix result(periods.size(), 11);

	for (unsigned int i = 0; i < periods.size(); ++i)
	{
		result(i, 0) = get<0>(periods.at(i));
		result(i, 1) = QCHelperFunctions::boolToZeroOne(get<1>(periods.at(i)));
		result(i, 2) = get<2>(periods.at(i));
		result(i, 3) = QCHelperFunctions::boolToZeroOne(get<3>(periods.at(i)));
		result(i, 4) = get<4>(periods.at(i));
		result(i, 5) = get<5>(periods.at(i)).excelSerial();
		result(i, 6) = get<6>(periods.at(i)).excelSerial();
		result(i, 7) = get<7>(periods.at(i)).excelSerial();
		result(i, 8) = get<8>(periods.at(i)).excelSerial();
		result(i, 9) = get<9>(periods.at(i)).excelSerial();
		result(i, 10) = get<10>(periods.at(i)).excelSerial();
	}

	return result;
}

string QCGetMacAddress()
{
	if (!HelperFunctions::checkAuthKey())
		throw runtime_error(LICENSE_MSG);

	char* result = HelperFunctions::getMAC();
	return string(result);
}

string QCSha256(string input)
{	
	char* cstr = &input[0u];;
	return Sha256::SHA256(cstr);
}

string QCGetAuthKey()
{
	return HelperFunctions::getAuthKey();
}

string QCGenerateKey(string password)
{
	if (password != "3141-YYZ-217-APDV")
		return "Password invalida.";

	string paddedMac = "rockandroll" + string(HelperFunctions::getMAC()) + "heavymetal";
	char* cstr = &paddedMac[0u];
	string key = Sha256::SHA256(cstr);
	ofstream keyFile("C:\\Creasys\\FrontDesk\\XLL\\auth_key.txt");
	if (keyFile.is_open())
	{
		keyFile << key << endl;
		keyFile.close();
		return "Archivo de licencia creado.";
	}
	else
	{
		return "No se pudo crear el archivo de licencia.";
	}
}
