#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include "xlw/MyContainers.h"
#include <xlw/CellMatrix.h>
#include <xlw/DoubleOrNothing.h>
#include <xlw/ArgList.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <stdio.h>
#include <Windows.h>
#include <Iphlpapi.h>
#include <Assert.h>
#pragma comment(lib, "iphlpapi.lib")

#include "QCDate.h"
#include "Sha256.h"

using namespace xlw;
using namespace std;

namespace HelperFunctions
{
	typedef tuple<string, string, string, string> string4;

	//start_date, end_date, value, yf, wf
	typedef tuple<
		QCDate,
		QCDate,
		double,
		string,
		string
	> ZeroRate;

	enum QCZeroRate
	{
		qcZeroRateStartDate,
		qcZeroRateEndDate,
		qcZeroRateValue,
		qcZeroRateYf,
		qcZeroRateWf
	};

	typedef tuple<
		double,								    //0 notional1 (strong currency)
		double,								    //1 notional2 (weak currency)
		QCDate,								    //2 end_date
		QCCurrencyConverter::QCCurrencyEnum,	//3 present value currency
		QCCurrencyConverter::QCCurrencyEnum,	//4 notional1 currency (strong)
		QCCurrencyConverter::QCCurrencyEnum,	//5 notional2 currency (weak)
		QCCurrencyConverter::QCFxRateEnum,		//6 fxRate1 (strong a present value)
		QCCurrencyConverter::QCFxRateEnum		//7 fxRate2 (weak a presente value)
	> FwdIndex;

	enum QCFwdIndex
	{
		qcFwdNotionalStrong,
		qcFwdNotionalWeak,
		qcFwdEndDate,
		qcFwdPresentValueCurr,
		qcFwdStrongCurr,
		qcFwdWeakCurr,
		qcFwdFxRateStrong2PV,
		qcFwdFxRateWeak2PV
	};

	typedef tuple<
		string,								//0		receive or pay
		QCDate,								//1		start_date
		QCDate,								//2		end_date
		unsigned int,						//3		settlement_lag
		QCInterestRateLeg::QCStubPeriod,	//4		stub period
		string,								//5		periodicity
		QCDate::QCBusDayAdjRules,			//6		end_date_adjustment
		QCInterestRateLeg::QCAmortization,	//7		amortization
		double,								//8		rate
		double,								//9		notional
		string,								//10	yf
		string								//11	wf
	> SwapIndex;

	typedef tuple<
		string,								//0		receive or pay
		QCDate,								//1		start_date
		QCDate,								//2		end_date
		string,								//3		settlement calendar
		unsigned int,						//4		settlement_lag
		QCInterestRateLeg::QCStubPeriod,	//5		stub period
		string,								//6		periodicity
		QCDate::QCBusDayAdjRules,			//7		end_date_adjustment
		string,								//8		fixing periodicity
		string,								//9		fixing calendar
		QCInterestRateLeg::QCStubPeriod,	//10	fixing stub period
		unsigned int,						//11	fixing lag
		QCInterestRateLeg::QCAmortization,	//12	amortization
		string,								//12	interest rate index
		double,								//14	rate
		double,								//15	spread
		double,								//16	notional
		string,								//17	yf
		string								//18	wf
	> FloatIndex;

	enum QCFloatIndex
	{
		qcReceivePay,
		qcStartDate,
		qcEndDate,
		qcSettlementCalendar,
		qcSettlementLag,
		qcStubPeriod,
		qcPeriodicity,
		qcEndDateAdjustment,
		qcFixingPeriodicity,
		qcFixingCalendar,
		qcFixingStubPeriod,
		qcFixingLag,
		qcAmortization,
		qcInterestRateIndex,
		qcRate,
		qcSpread,
		qcNotional,
		qcYearFraction,
		qcWealthFactor
	};

	void buildFloatingRateIndexVector(CellMatrix& xlInputFloatingLegs,
									  double fixing,
									  vector<FloatIndex>& floatIndexVector)
	{
		size_t numSwaps = xlInputFloatingLegs.RowsInStructure();
		floatIndexVector.resize(numSwaps);
		for (size_t i = 0; i < numSwaps; ++i)
		{
			FloatIndex temp;
			cout << endl;

			//Receive or pay
			get<qcReceivePay>(temp) = xlInputFloatingLegs(i, 0).StringValue();

			//Build start date
			get<qcStartDate>(temp) = QCDate{ static_cast<long>(xlInputFloatingLegs(i, 1).NumericValue()) };

			//Build end date
			get<qcEndDate>(temp) = QCDate{ static_cast<long>(xlInputFloatingLegs(i, 2).NumericValue()) };

			//Build setllement calendar
			get<qcSettlementCalendar>(temp) = xlInputFloatingLegs(i, 3).StringValue();

			//Build settlement lag
			get<qcSettlementLag>(temp) = static_cast<long>(xlInputFloatingLegs(i, 4).NumericValue());

			//Build enum stub
			string stub = xlInputFloatingLegs(i, 5).StringValue();
			get<qcStubPeriod>(temp) = QCHelperFunctions::stringToQCStubPeriod(stub);

			//Build periodicity
			get<qcPeriodicity>(temp) = xlInputFloatingLegs(i, 6).StringValue();

			//Build enum end date adjustment
			string endDatAdj = xlInputFloatingLegs(i, 7).StringValue();
			get<qcEndDateAdjustment>(temp) = QCHelperFunctions::stringToQCBusDayAdjRule(endDatAdj);

			//Build fixing periodicity
			get<qcFixingPeriodicity>(temp) = xlInputFloatingLegs(i, 13).StringValue();

			//Build fixing calendar
			get<qcFixingCalendar>(temp) = xlInputFloatingLegs(i, 14).StringValue();

			//Build fixing stub period
			string fixingStub = xlInputFloatingLegs(i, 12).StringValue();
			get<qcFixingStubPeriod>(temp) = QCHelperFunctions::stringToQCStubPeriod(fixingStub);

			//Build fixing lag
			get<qcFixingLag>(temp) = static_cast<unsigned int>(xlInputFloatingLegs(i, 15).NumericValue());

			//Build enum amort
			string amort = xlInputFloatingLegs(i, 8).StringValue();
			get<qcAmortization>(temp) = QCHelperFunctions::stringToQCAmortization(amort);

			//Build interest rate index
			get<qcInterestRateIndex>(temp) = xlInputFloatingLegs(i, 9).StringValue();

			//Build rate
			get<qcRate>(temp) = fixing;

			//Build spread
			get<qcSpread>(temp) = xlInputFloatingLegs(i, 11).NumericValue();

			//Build notional
			get<qcNotional>(temp) = xlInputFloatingLegs(i, 17).NumericValue();

			//Build enum wf
			string wf = xlInputFloatingLegs(i, 18).StringValue();
			QCHelperFunctions::lowerCase(wf);
			get<qcWealthFactor>(temp) = wf;

			//Build enum yf (20)
			string yf = xlInputFloatingLegs(i, 19).StringValue();
			QCHelperFunctions::lowerCase(yf);
			get<qcYearFraction>(temp) = yf;

			floatIndexVector.at(i) = temp;
		}
	}

	void buildFixedRateIndexVector(CellMatrix& xlInputFixedLegs, QCDate valueDate,
		vector<QCDate>& dateVector, vector<SwapIndex>& swapIndexVector)
	{
		//Esto es un ejemplo de lo que entra:
		//('SwapCamCLP9M' 0, False 1, 2 (start_date_lag) 2, '9M' 3, None (start_date) 4, None (end_date) 5,
		//'SCL' 6, 0L 7, 'CORTO INICIO' 8, '2Y' 9, 'FOLLOW' 10, 'BULLET' 11, 1.0 12, 'LIN' 13, 
		//'ACT/360' 14, 'CAMARACLP' 15, 0.0291 16)

		//Se quiere obtener
		//string (R) 0, QCDate (start) 1, QCDate (end) 2, unsigned int (sett lag) 3, enum stub 4,
		//string (periodicity) 5, enum follow 6, enum amort 7, double (rate) 8, double (notional) 9,
		//enum wf 10, enum yf 11
		size_t numSwaps = xlInputFixedLegs.RowsInStructure();
		swapIndexVector.resize(numSwaps);
		for (size_t i = 0; i < numSwaps; ++i)
		{
			SwapIndex temp;

			//Receive or pay
			get<0>(temp) = "R";

			//Build start date (1)
			get<1>(temp) = QCDate{ static_cast<long>(xlInputFixedLegs(i, 1).NumericValue()) };

			//Build end date (2)
			get<2>(temp) = QCDate{ static_cast<long>(xlInputFixedLegs(i, 2).NumericValue()) };

			//Build settlement lag (4)
			get<3>(temp) = static_cast<int>(xlInputFixedLegs(i, 4).NumericValue());

			//Build enum stub (5)
			string stub = xlInputFixedLegs(i, 5).StringValue();
			get<4>(temp) = QCHelperFunctions::stringToQCStubPeriod(stub);

			//Build periodicity (6)
			string periodicity = xlInputFixedLegs(i, 6).StringValue();
			get<5>(temp) = periodicity;

			//Build enum end date adjustment (7)
			string endDatAdj = xlInputFixedLegs(i, 7).StringValue();
			get<6>(temp) = QCHelperFunctions::stringToQCBusDayAdjRule(endDatAdj);

			//Build enum amort (8)
			string amort = xlInputFixedLegs(i, 8).StringValue();
			get<7>(temp) = QCHelperFunctions::stringToQCAmortization(amort);

			//Build rate (9)
			double rate = xlInputFixedLegs(i, 9).NumericValue();
			get<8>(temp) = rate;

			//Build notional (10)
			double notional = xlInputFixedLegs(i, 10).NumericValue();
			get<9>(temp) = notional;

			//Build enum yf (12)
			string yf = xlInputFixedLegs(i, 12).StringValue();
			QCHelperFunctions::lowerCase(yf);
			get<10>(temp) = yf;

			//Build enum wf (11)
			string wf = xlInputFixedLegs(i, 11).StringValue();
			QCHelperFunctions::lowerCase(wf);
			get<11>(temp) = wf;

			swapIndexVector.at(i) = temp;
		}

	}

	void buildFwdVector(CellMatrix xlInputForwards, vector<FwdIndex>& fwdIndexVector, double spot = 0)
	{
		size_t numFwds = xlInputForwards.RowsInStructure();
		fwdIndexVector.resize(numFwds);
		QCCurrencyConverter conv;
		for (size_t i = 0; i < numFwds; ++i)
		{
			FwdIndex temp;
			get<0>(temp) = 1.0;
			get<1>(temp) = xlInputForwards(i, 2).NumericValue() + spot;
			get<2>(temp) = QCDate{ static_cast<long>(xlInputForwards(i, 1).NumericValue()) };
			get<3>(temp) = conv.getWeakCurrencyEnum(xlInputForwards(i, 3).StringValue());
			get<4>(temp) = conv.getStrongCurrencyEnum(xlInputForwards(i, 3).StringValue());
			get<5>(temp) = get<3>(temp);
			get<6>(temp) = conv.getStandardFxRate(get<4>(temp), get<3>(temp));
			get<7>(temp) = conv.getStandardFxRate(get<3>(temp), get<3>(temp));
			fwdIndexVector.at(i) = temp;
		}
	}

	void buildZeroRateVector(CellMatrix& xlInputRates, vector<ZeroRate>& zeroRateVector)
	{
		size_t cuantas = xlInputRates.RowsInStructure();
		zeroRateVector.resize(cuantas);
		for (size_t i = 0; i < cuantas; ++i)
		{
			auto temp = make_tuple(QCDate{ static_cast<long>(xlInputRates(i, 0).NumericValue()) },
				QCDate{ static_cast<long>(xlInputRates(i, 1).NumericValue()) },
				xlInputRates(i, 2).NumericValue(),
				xlInputRates(i, 3).StringValue(),
				xlInputRates(i, 4).StringValue()
				);
			QCHelperFunctions::lowerCase(get<3>(temp));
			QCHelperFunctions::lowerCase(get<4>(temp));
			zeroRateVector.at(i) = temp;
		}
	}

	void buildVectorHolidays(CellMatrix& holidays, vector<QCDate>& vectorHolidays)
	{
		size_t cuantas = holidays.RowsInStructure();
		vectorHolidays.resize(cuantas);
		for (size_t i = 0; i < cuantas; ++i)
		{
			vectorHolidays.at(i) = QCDate{ static_cast<long>(holidays(i, 0).NumericValue()) };
		}

		return;
	}

	void buildHolidays(CellMatrix& holidays, map<string, vector<QCDate>>& mapHolidays)
	{
		string lastName, nextName;
		lastName = holidays(0, 0).StringValue();
		vector<QCDate> temp;
		for (size_t i = 0; i < holidays.RowsInStructure(); ++i)
		{
			nextName = holidays(i, 0).StringValue();
			if (nextName == lastName)
			{
				temp.push_back(QCDate{ (long)holidays(i, 1).NumericValue() });
				lastName = nextName;
			}
			else
			{
				mapHolidays.insert(pair<string, vector<QCDate>>{lastName, temp});
				temp.clear();
				temp.push_back(QCDate{ (long)holidays(i, 1).NumericValue() });
				lastName = nextName;
			}
		}
		mapHolidays.insert(pair<string, vector<QCDate>>{lastName, temp});
		return;
	}

	void buildCurveValues(CellMatrix& curveValues, map<string, pair<vector<long>,
		vector<double>>>& crvValues)
	{
		string lastName, nextName;
		lastName = curveValues(0, 0).StringValue();
		vector<long> tenors;
		vector<double> values;
		for (size_t i = 0; i < curveValues.RowsInStructure(); ++i)
		{
			nextName = curveValues(i, 0).StringValue();
			if (nextName == lastName)
			{
				double aux = curveValues(i, 1).NumericValue();
				tenors.push_back((long)curveValues(i, 1).NumericValue());
				values.push_back((double)curveValues(i, 2).NumericValue());
				lastName = nextName;
			}
			else
			{
				crvValues.insert(pair<string, pair<vector<long>, vector<double>>>{lastName, { tenors, values }});
				tenors.clear();
				values.clear();
				tenors.push_back((long)curveValues(i, 1).NumericValue());
				values.push_back((double)curveValues(i, 2).NumericValue());
				lastName = nextName;
			}
		}
		crvValues.insert(pair<string, pair<vector<long>, vector<double>>>{lastName, { tenors, values }});

	}

	void buildCurveCharacteristics(CellMatrix& curveChars, map<string, string4>& crvChars)
	{
		for (size_t i = 0; i < curveChars.RowsInStructure(); ++i)
		{
			string4 temp = make_tuple(curveChars(i, 1).StringValue(),
				curveChars(i, 2).StringValue(),
				curveChars(i, 3).StringValue(),
				curveChars(i, 4).StringValue());
			crvChars.insert(pair<string, string4>(curveChars(i, 0).StringValue(),
				temp));
		}
	}

	void buildCustomAmortization(CellMatrix& customAmort, map<unsigned long,
		vector<tuple<QCDate, double, double>>>& dateNotionalAndAmortByIdLeg)
	{
		unsigned long lastIdLeg = (long)customAmort(0, 0).NumericValue();
		QCDate tempDate;
		double tempAmort;
		double tempNotional;
		tuple<QCDate, double, double> tempValue;
		vector<tuple<QCDate, double, double>> tempVector;
		for (size_t i = 0; i < customAmort.RowsInStructure(); ++i)
		{
			unsigned long idLeg = static_cast<unsigned long>(customAmort(i, 0).NumericValue());
			tempDate = QCDate{ (long)customAmort(i, 1).NumericValue() };
			tempAmort = (double)customAmort(i, 2).NumericValue();
			tempNotional = (double)customAmort(i, 3).NumericValue();
			tempValue = make_tuple(tempDate, tempAmort, tempNotional);
			if (idLeg == lastIdLeg)
			{
				tempVector.push_back(tempValue);
				lastIdLeg = idLeg;
			}
			else
			{
				dateNotionalAndAmortByIdLeg.insert(pair<unsigned long, vector<tuple<QCDate, double, double>>>
					(lastIdLeg, tempVector));
				tempVector.clear();
				tempVector.push_back(tempValue);
				lastIdLeg = idLeg;
			}
		}

		//Insertamos en el ultimo vector en el mapa 
		dateNotionalAndAmortByIdLeg.insert(pair<unsigned long, vector<tuple<QCDate, double, double>>>
			(lastIdLeg, tempVector));
		tempVector.clear();

	}

	void buildFixings(CellMatrix& fixings, map<QCDate, double>& mapFixings)
	{
		for (unsigned long i = 0; i < fixings.RowsInStructure(); ++i)
		{
			mapFixings.insert(pair<QCDate, double>(QCDate{ (long)fixings(i, 0).NumericValue() },
				fixings(i, 1).NumericValue()));
		}
	}

	void buildManyFixings(CellMatrix& fixings, map<string, map<QCDate, double>>& mapManyFixings)
	{
		//La CellMatrix fixings debe venir ordenada por nombre
		string lastName, nextName;
		lastName = fixings(0, 0).StringValue();
		map<QCDate, double> tempMap;
		unsigned long numRows = fixings.RowsInStructure();
		for (unsigned long i = 0; i < numRows; ++i)
		{
			nextName = fixings(i, 0).StringValue();
			if (nextName == lastName)
			{
				tempMap.insert(pair<QCDate, double>(QCDate{ (long)fixings(i, 1).NumericValue() },
					(double)fixings(i, 2).NumericValue()));
				lastName = nextName;
			}
			else
			{
				mapManyFixings.insert(pair<string, map<QCDate, double>>(lastName, tempMap));
				tempMap.clear();
				tempMap.insert(pair<QCDate, double>(QCDate{ (long)fixings(i, 1).NumericValue() },
					(double)fixings(i, 2).NumericValue()));
				lastName = nextName;
			}
		}
		//Un insert final
		mapManyFixings.insert(pair<string, map<QCDate, double>>(lastName, tempMap));

	}

	void buildStringStringMap(CellMatrix& stringMatrix, map<string, string>& mapStringString)
	{
		for (unsigned long i = 0; i < stringMatrix.RowsInStructure(); ++i)
		{
			mapStringString.insert(pair<string, string>{stringMatrix(i, 0).StringValue(),
				stringMatrix(i, 1).StringValue()});
		}
	}

	void buildStringPairStringMap(CellMatrix& stringMatrix,
		map<string, pair<string, string>>& mapStringPairString)
	{
		for (size_t i = 0; i < stringMatrix.RowsInStructure(); ++i)
		{
			mapStringPairString.insert(pair<string, pair<string, string>>{stringMatrix(i, 0).StringValue(),
				make_pair(stringMatrix(i, 1).StringValue(),
				stringMatrix(i, 2).StringValue())});
		}
	}

	shared_ptr<QCZeroCouponCurve> buildZeroCouponCurve(CellMatrix tenorsRates,
		string curveInterpolator,
		string curveYf,
		string curveWf)
	{
		//Vamos a construir el shared_ptr de QCZeroCouponCurve
		size_t puntosCurva = tenorsRates.RowsInStructure();
		vector<long> tenors;
		tenors.resize(puntosCurva);
		vector<double> rates;
		rates.resize(puntosCurva);
		for (size_t i = 0; i < puntosCurva; ++i)
		{
			tenors.at(i) = static_cast<long>(tenorsRates(i, 0).NumericValue());
			rates.at(i) = tenorsRates(i, 1).NumericValue();
		}

		//Definir la curva cero cupon de descuento
		QCHelperFunctions::lowerCase(curveInterpolator);
		QCHelperFunctions::lowerCase(curveYf);
		QCHelperFunctions::lowerCase(curveWf);
		return QCFactoryFunctions::zrCpnCrvShrdPtr(tenors,
			rates, curveInterpolator, curveYf, curveWf);
	}

	char* getMAC()
	{
		PIP_ADAPTER_INFO AdapterInfo;
		DWORD dwBufLen = sizeof(AdapterInfo);
		char* mac_addr = (char*)malloc(17);

		AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
		if (AdapterInfo == NULL) {
			printf("Error allocating memory needed to call GetAdaptersinfo\n");

		}

		// Make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen variable
		if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) {

			AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
			if (AdapterInfo == NULL) {
				printf("Error allocating memory needed to call GetAdaptersinfo\n");
			}
		}

		if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) {
			PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;// Contains pointer to current adapter info
			do {
				sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
					pAdapterInfo->Address[0], pAdapterInfo->Address[1],
					pAdapterInfo->Address[2], pAdapterInfo->Address[3],
					pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
				printf("Address: %s, mac: %s\n", pAdapterInfo->IpAddressList.IpAddress.String, mac_addr);
				return mac_addr;

				printf("\n");
				pAdapterInfo = pAdapterInfo->Next;
			} while (pAdapterInfo);
		}
		free(AdapterInfo);
	}

	string getAuthKey() 
	{
		string line;
		ifstream authKeyFile("C:\\Creasys\\FrontDesk\\XLL\\auth_key.txt");
		if (authKeyFile.is_open())
		{
			getline(authKeyFile, line);
			authKeyFile.close();
		}
		else
		{
			throw runtime_error("No se puede abrir el archivo de licencia.");
		}
		return line;
	}

	bool checkAuthKey()
	{
		string key = getAuthKey();
		string paddedMac = "rockandroll" + string(getMAC()) + "heavymetal";
		char* cstr = &paddedMac[0u];
		string calcKey = Sha256::SHA256(cstr);
		if (calcKey == key)
			return true;
		else
			return false;
	}

	vector<QCDate> getHolidays(map<string, vector<QCDate>>& hm, string name)
	{
		try
		{
			return hm.at(name);
		}
		catch (exception& e)
		{
			string msg = string(e.what()) + "No se encuentran la lista de feriados del calendario " + name;
			throw runtime_error(msg);
		}
	};

	pair<string, string> getIndexChars(map<string, pair<string, string>>& ic, string name)
	{
		try
		{
			return ic.at(name);
		}
		catch (exception& e)
		{
			string msg = string(e.what()) + "No se encuentran las caracteristicas del indice flotante " + name;
			throw runtime_error(msg);
		}
	};

}


#endif