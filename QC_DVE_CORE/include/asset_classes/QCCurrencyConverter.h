#ifndef QCCURRENCYCONVERTER_H
#define QCCURRENCYCONVERTER_H

#include <string>
#include <map>

#include "asset_classes/FXRateIndex.h"
#include "QCCurrency.h"

using namespace std;

/*!
* @brief Esta es una clase auxiliar que se utiliza para transformar un monto en una
* moneda a un monto en otra moneda, dado el fx_rate de conversión.
*/
class QCCurrencyConverter
{
public:
	/*!
	Enumera todas las divisas.
	*/
	enum QCCurrencyEnum
	{
        qcUSD,
		qcCLF,
		qcCLP,
		qcGBP,
		qcEUR,
		qcJPY,
		qcCHF,
		qcCOP,
        qcCNY,
        qcSEK,
        qcPEN,
        qcCAD,
        qcMXN
	};

    /*!
	Enumera todos los tipos de cambio.
	*/
    enum QCFxRateEnum
    {
        qcUSDUSD,

        qcCLFCLF,
        qcUSDCLF,
        qcCLFCLP,

        qcCLPCLP,
        qcUSDCLP,

        qcGBPGBP,
        qcGBPUSD,
        qcGBPCLP,

        qcEUREUR,
        qcEURUSD,
        qcEURCLP,

        qcJPYJPY,
        qcUSDJPY,
        qcJPYCLP,

        qcCHFCHF,
        qcUSDCHF,
        qcEURCHF,
        qcCHFCLP,

        qcCOPCOP,
        qcUSDCOP,
        qcCOPCLP,

        qcCNYCNY,
        qcUSDCNY,
        qcCNYCLP,

        qcSEKSEK,
        qcUSDSEK,
        qcSEKCLP,

        qcPENPEN,
        qcUSDPEN,
        qcPENCLP,

        qcCADCAD,
        qcUSDCAD,
        qcCADCLP,

        qcMXNMXN,
        qcUSDMXN,
        qcMXNCLP
    };

	

	/*!
	* Constructor. Inicializa además los mapas que contienen los string que permiten identificar
	* textualmente las divisas y tipos de cambio.
	*/
	QCCurrencyConverter()
	{
        // QCCurrencyEnum ---> ISO Code
        _currencyCode.insert(pair<QCCurrencyEnum, string>(qcUSD, "USD"));
        _currencyCode.insert(pair<QCCurrencyEnum, string>(qcCLF, "CLF"));
        _currencyCode.insert(pair<QCCurrencyEnum, string>(qcCLP, "CLP"));
        _currencyCode.insert(pair<QCCurrencyEnum, string>(qcGBP, "GBP"));
        _currencyCode.insert(pair<QCCurrencyEnum, string>(qcEUR, "EUR"));
        _currencyCode.insert(pair<QCCurrencyEnum, string>(qcJPY, "JPY"));
        _currencyCode.insert(pair<QCCurrencyEnum, string>(qcCHF, "CHF"));
        _currencyCode.insert(pair<QCCurrencyEnum, string>(qcCOP, "COP"));
        _currencyCode.insert(pair<QCCurrencyEnum, string>(qcCNY, "CNY"));
        _currencyCode.insert(pair<QCCurrencyEnum, string>(qcSEK, "SEK"));
        _currencyCode.insert(pair<QCCurrencyEnum, string>(qcPEN, "PEN"));
        _currencyCode.insert(pair<QCCurrencyEnum, string>(qcCAD, "CAD"));
        _currencyCode.insert(pair<QCCurrencyEnum, string>(qcMXN, "MXN"));

        // QCFxRateEnum ---> Code
        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcUSDUSD, "USDUSD"));

        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcCLFCLF, "CLFCLF"));
        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcUSDCLF, "USDCLF"));
        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcCLFCLP, "CLFCLP"));

		_fxRateCode.insert(pair<QCFxRateEnum, string>(qcCLPCLP, "CLPCLP"));
        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcUSDCLP, "USDCLP"));

        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcGBPGBP, "GBPGBP"));
        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcGBPUSD, "GBPUSD"));
        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcGBPCLP, "GBPCLP"));

        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcEUREUR, "EUREUR"));
        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcEURUSD, "EURUSD"));
		_fxRateCode.insert(pair<QCFxRateEnum, string>(qcEURCLP, "EURCLP"));

        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcJPYJPY, "JPYJPY"));
        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcUSDJPY, "USDJPY"));
		_fxRateCode.insert(pair<QCFxRateEnum, string>(qcJPYCLP, "JPYCLP"));

		_fxRateCode.insert(pair<QCFxRateEnum, string>(qcCHFCHF, "CHFCHF"));
		_fxRateCode.insert(pair<QCFxRateEnum, string>(qcEURCHF, "EURCHF"));
        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcUSDCHF, "USDCHF"));
        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcCHFCLP, "CHFCLP"));

        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcCOPCOP, "COPCOP"));
        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcUSDCOP, "USDCOP"));
        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcCOPCLP, "COPCLP"));

        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcCNYCNY, "CNYCNY"));
        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcUSDCNY, "USDCNY"));
        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcCNYCLP, "CNYCLP"));

        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcSEKSEK, "SEKSEK"));
        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcUSDSEK, "USDSEK"));
        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcSEKCLP, "SEKCLP"));

        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcPENPEN, "PENPEN"));
        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcUSDPEN, "USDPEN"));
        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcPENCLP, "PENCLP"));

        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcCADCAD, "CADCAD"));
        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcUSDCAD, "USDCAD"));
        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcCADCLP, "CADCLP"));

        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcMXNMXN, "MXNMXN"));
        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcUSDMXN, "USDMXN"));
        _fxRateCode.insert(pair<QCFxRateEnum, string>(qcMXNCLP, "MXNCLP"));

        // Code (incluido al revés) ---> QCFxRateEnum
        _standardFxRate.insert(pair<string, QCFxRateEnum>("USDUSD", qcUSDUSD));

        _standardFxRate.insert(pair<string, QCFxRateEnum>("CLFCLF", qcCLFCLF));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("CLFCLP", qcCLFCLP));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("CLPCLF", qcCLFCLP));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("USDCLF", qcUSDCLF));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("CLFUSD", qcUSDCLF));

		_standardFxRate.insert(pair<string, QCFxRateEnum>("CLPCLP", qcCLPCLP));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("USDCLP", qcUSDCLP));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("CLPUSD", qcUSDCLP));

        _standardFxRate.insert(pair<string, QCFxRateEnum>("GBPGBP", qcGBPGBP));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("GBPUSD", qcGBPUSD));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("USDGBP", qcGBPUSD));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("GBPCLP", qcGBPCLP));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("CLPGBP", qcGBPCLP));

        _standardFxRate.insert(pair<string, QCFxRateEnum>("EUREUR", qcEUREUR));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("EURUSD", qcEURUSD));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("USDEUR", qcEURUSD));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("EURCLP", qcEURCLP));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("CLPEUR", qcEURCLP));

        _standardFxRate.insert(pair<string, QCFxRateEnum>("JPYJPY", qcJPYJPY));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("USDJPY", qcUSDJPY));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("JPYUSD", qcUSDJPY));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("JPYCLP", qcJPYCLP));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("CLPJPY", qcJPYCLP));

		_standardFxRate.insert(pair<string, QCFxRateEnum>("CHFCHF", qcCHFCHF));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("EURCHF", qcEURCHF));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("CHFEUR", qcEURCHF));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("USDCHF", qcUSDCHF));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("CHFUSD", qcUSDCHF));

        _standardFxRate.insert(pair<string, QCFxRateEnum>("COPCOP", qcCOPCOP));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("USDCOP", qcUSDCOP));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("COPUSD", qcUSDCOP));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("COPCLP", qcCOPCLP));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("CLPCOP", qcCOPCLP));

        _standardFxRate.insert(pair<string, QCFxRateEnum>("CNYCNY", qcCNYCNY));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("USDCNY", qcUSDCNY));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("CNYUSD", qcUSDCNY));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("CNYCLP", qcCNYCLP));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("CLPCNY", qcCNYCLP));

        _standardFxRate.insert(pair<string, QCFxRateEnum>("SEKSEK", qcSEKSEK));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("USDSEK", qcUSDSEK));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("SEKUSD", qcUSDSEK));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("SEKCLP", qcSEKCLP));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("CLPSEK", qcSEKCLP));

        _standardFxRate.insert(pair<string, QCFxRateEnum>("PENPEN", qcCNYCNY));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("USDCNY", qcUSDCNY));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("CNYUSD", qcUSDCNY));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("CNYCLP", qcCNYCLP));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("CLPCNY", qcCNYCLP));

        _standardFxRate.insert(pair<string, QCFxRateEnum>("CADCAD", qcCADCAD));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("USDCAD", qcUSDCAD));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("CADUSD", qcUSDCAD));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("CADCLP", qcCADCLP));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("CLPCAD", qcCADCLP));

        _standardFxRate.insert(pair<string, QCFxRateEnum>("MXNMXN", qcMXNMXN));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("USDMXN", qcUSDMXN));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("MXNUSD", qcUSDMXN));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("MXNCLP", qcMXNCLP));
        _standardFxRate.insert(pair<string, QCFxRateEnum>("CLPMXN", qcMXNCLP));


        // Code (incluido cuando está al revés) ---> Code (el correcto)
        _fxRateMktCode.insert(pair<std::string, std::string>("USDUSD", "USDUSD"));

        _fxRateMktCode.insert(pair<std::string, std::string>("CLFCLF", "CLFCLF"));
        _fxRateMktCode.insert(pair<std::string, std::string>("CLFCLP", "CLFCLP"));
        _fxRateMktCode.insert(pair<std::string, std::string>("CLPCLF", "CLFCLP"));
        _fxRateMktCode.insert(pair<std::string, std::string>("USDCLF", "USDCLF"));
        _fxRateMktCode.insert(pair<std::string, std::string>("CLFUSD", "USDCLF"));

        _fxRateMktCode.insert(pair<std::string, std::string>("CLPCLP", "CLPCLP"));
        _fxRateMktCode.insert(pair<std::string, std::string>("USDCLP", "USDCLP"));
        _fxRateMktCode.insert(pair<std::string, std::string>("CLPUSD", "USDCLP"));

        _fxRateMktCode.insert(pair<std::string, std::string>("GBPGBP", "GBPGBP"));
        _fxRateMktCode.insert(pair<std::string, std::string>("GBPUSD", "GBPUSD"));
        _fxRateMktCode.insert(pair<std::string, std::string>("USDGBP", "GBPUSD"));
        _fxRateMktCode.insert(pair<std::string, std::string>("GBPCLP", "GBPCLP"));
        _fxRateMktCode.insert(pair<std::string, std::string>("CLPGBP", "GBPCLP"));

		_fxRateMktCode.insert(pair<std::string, std::string>("EUREUR", "EUREUR"));
        _fxRateMktCode.insert(pair<std::string, std::string>("EURUSD", "EURUSD"));
        _fxRateMktCode.insert(pair<std::string, std::string>("USDEUR", "EURUSD"));
        _fxRateMktCode.insert(pair<std::string, std::string>("EURCLP", "EURCLP"));
        _fxRateMktCode.insert(pair<std::string, std::string>("CLPEUR", "EURCLP"));

        _fxRateMktCode.insert(pair<std::string, std::string>("JPYJPY", "JPYJPY"));
        _fxRateMktCode.insert(pair<std::string, std::string>("USDJPY", "USDJPY"));
        _fxRateMktCode.insert(pair<std::string, std::string>("JPYUSD", "USDJPY"));
        _fxRateMktCode.insert(pair<std::string, std::string>("JPYCLP", "JPYCLP"));
        _fxRateMktCode.insert(pair<std::string, std::string>("CLPJPY", "JPYCLP"));

		_fxRateMktCode.insert(pair<std::string, std::string>("CHFCHF", "CHFCHF"));
        _fxRateMktCode.insert(pair<std::string, std::string>("CHFUSD", "CHFUSD"));
        _fxRateMktCode.insert(pair<std::string, std::string>("USDCHF", "CHFUSD"));
        _fxRateMktCode.insert(pair<std::string, std::string>("CHFCLP", "CHFCLP"));
        _fxRateMktCode.insert(pair<std::string, std::string>("CLPCHF", "CHFCLP"));
        _fxRateMktCode.insert(pair<std::string, std::string>("EURCHF", "EURCHF"));
        _fxRateMktCode.insert(pair<std::string, std::string>("CHFEUR", "EURCHF"));

        _fxRateMktCode.insert(pair<std::string, std::string>("COPCOP", "COPCOP"));
        _fxRateMktCode.insert(pair<std::string, std::string>("USDCOP", "USDCOP"));
        _fxRateMktCode.insert(pair<std::string, std::string>("COPUSD", "USDCOP"));
        _fxRateMktCode.insert(pair<std::string, std::string>("COPCLP", "COPCLP"));
        _fxRateMktCode.insert(pair<std::string, std::string>("CLPCOP", "COPCLP"));

        _fxRateMktCode.insert(pair<std::string, std::string>("CNYCNY", "CNYCNY"));
        _fxRateMktCode.insert(pair<std::string, std::string>("USDCNY", "USDCNY"));
        _fxRateMktCode.insert(pair<std::string, std::string>("CNYUSD", "USDCNY"));
        _fxRateMktCode.insert(pair<std::string, std::string>("CNYCLP", "CNYCLP"));
        _fxRateMktCode.insert(pair<std::string, std::string>("CLPCNY", "CNYCLP"));

        _fxRateMktCode.insert(pair<std::string, std::string>("SEKSEK", "SEKSEK"));
        _fxRateMktCode.insert(pair<std::string, std::string>("USDSEK", "USDSEK"));
        _fxRateMktCode.insert(pair<std::string, std::string>("SEKUSD", "USDSEK"));
        _fxRateMktCode.insert(pair<std::string, std::string>("SEKCLP", "SEKCLP"));
        _fxRateMktCode.insert(pair<std::string, std::string>("CLPSEK", "SEKCLP"));

        _fxRateMktCode.insert(pair<std::string, std::string>("PENPEN", "PENPEN"));
        _fxRateMktCode.insert(pair<std::string, std::string>("USDPEN", "USDPEN"));
        _fxRateMktCode.insert(pair<std::string, std::string>("PENUSD", "USDPEN"));
        _fxRateMktCode.insert(pair<std::string, std::string>("PENCLP", "PENCLP"));
        _fxRateMktCode.insert(pair<std::string, std::string>("CLPPEN", "PENCLP"));

        _fxRateMktCode.insert(pair<std::string, std::string>("CADCAD", "CADCAD"));
        _fxRateMktCode.insert(pair<std::string, std::string>("USDCAD", "USDCAD"));
        _fxRateMktCode.insert(pair<std::string, std::string>("CADUSD", "USDCAD"));
        _fxRateMktCode.insert(pair<std::string, std::string>("CADCLP", "CADCLP"));
        _fxRateMktCode.insert(pair<std::string, std::string>("CLPCAD", "CADCLP"));

        _fxRateMktCode.insert(pair<std::string, std::string>("MXNMXN", "MXNMXN"));
        _fxRateMktCode.insert(pair<std::string, std::string>("USDMXN", "USDMXN"));
        _fxRateMktCode.insert(pair<std::string, std::string>("MXNUSD", "USDMXN"));
        _fxRateMktCode.insert(pair<std::string, std::string>("MXNCLP", "MXNCLP"));
        _fxRateMktCode.insert(pair<std::string, std::string>("CLPMXN", "MXNCLP"));
	}

	/*!
	* Convierte un monto en un moneda a otra.
	* @param amount, monto de la moneda a convertir.
	* @param currency, moneda del monto a convertir.
	* @fxRateValue, valor del índice a utilizar en la conversión.
	* @fxRate, qué tipo de cambio representa el valor anterior.
	*/
	double convert(double amount, QCCurrencyEnum currency, double fxRateValue, QCFxRateEnum fxRate)
	{
		if (_getStrongCurrency(fxRate) == _currencyCode[currency])
		{
			return amount * fxRateValue;
		}
		else if (_getWeakCurrency(fxRate) == _currencyCode[currency])
		{
			return amount / fxRateValue;
		}
		else
		{
			throw invalid_argument("Fx Rate for conversion does not apply to this currency.");
		}
	}

	/**
	 * @fn	double QCCurrencyConverter::convert(double amount, QCCurrency& currency, double fxRateValue, QCode::Financial::FXRateIndex& fxRateIndex)
	 *
	 * @brief	Converts an amount in currency to another currency using the fx rate given.
	 *
	 * @author	Alvaro Díaz V.
	 * @date	19/07/2018
	 *
	 * @exception	invalid_argument	Thrown when an invalid argument error condition occurs.
	 *
	 * @param 		  	amount	   	The amount to convert.
	 * @param [in,out]	currency   	The currency of the amount.
	 * @param 		  	fxRateValue	The fx rate value.
	 * @param [in,out]	fxRateIndex	The fx rate index.
	 *
	 * @return	A double.
	 */
//	double convert(double amount, shared_ptr<QCCurrency> currency, double fxRateValue,
//	        QCode::Financial::FXRateIndex& fxRateIndex)
//	{
//		if (fxRateIndex.strongCcyCode() == currency->getIsoCode() )
//		{
//			return amount * fxRateValue;
//		}
//		else if (fxRateIndex.weakCcyCode() == currency->getIsoCode())
//		{
//			return amount / fxRateValue;
//		}
//		else
//		{
//			throw invalid_argument("Fx Rate for conversion does not apply to this currency.");
//		}
//	}

	/**
	 * @fn	double QCCurrencyConverter::convert(double amount, shared_ptr<QCCurrency> currency, double fxRateValue, const QCode::Financial::FXRateIndex& fxRateIndex)
	 *
	 * @brief	Converts an amount in currency to another currency using the fx rate given.
	 *
	 * @author	A Diaz V
	 * @date	21-03-2019
	 *
	 * @exception	invalid_argument	Thrown when an invalid argument error condition occurs.
	 *
	 * @param	amount	   	The amount to convert.
	 * @param	currency   	The currency of the amount.
	 * @param	fxRateValue	The fx rate value.
	 * @param	fxRateIndex	The fx rate index.
	 *
	 * @returns	A double.
	 */
	double convert(double amount, shared_ptr<QCCurrency> currency, double fxRateValue,
	        QCode::Financial::FXRateIndex fxRateIndex)
	{
		if (fxRateIndex.strongCcyCode() == currency->getIsoCode())
		{
			return amount * fxRateValue;
		}
		else if (fxRateIndex.weakCcyCode() == currency->getIsoCode())
		{
			return amount / fxRateValue;
		}
		else
		{
			throw invalid_argument("Fx Rate for conversion does not apply to this currency.");
		}
	}

	/*!
	* Retorna el enum de una divisa si se da el código en texto.
	*/
	QCCurrencyEnum getCurrencyEnum(string code)
	{
		for (auto& x : _currencyCode)
		{
			if (x.second == code)
				return x.first;
		}
		throw invalid_argument("No corresponding currency enum for this currency code (" + code +").");
	}

	/*!
	* Retorna el enum de un tipo de cambio si se da el código en texto.
	*/
	QCFxRateEnum getFxRateEnum(string code)
	{
		for (auto& x : _fxRateCode)
		{
			if (x.second == code)
				return x.first;
		}
		throw invalid_argument("No corresponding fx rate enum for this fx rate code (" + code + ").");
	}

	/*!
	* Retorna el enum de la divisa fuerte de un tipo de cambio si se da el código en texto.
	*/
	QCCurrencyEnum getStrongCurrencyEnum(string code)
	{
		auto fx = getFxRateEnum(code);
		auto strong = _getStrongCurrency(fx);
		return getCurrencyEnum(strong);
	}

	/*!
	* Retorna el enum de la divisa débil de un tipo de cambio si se da el código en texto.
	*/
	QCCurrencyEnum getWeakCurrencyEnum(string code)
	{
		auto fx = getFxRateEnum(code);
		auto weak = _getWeakCurrency(fx);
		return getCurrencyEnum(weak);
	}

	/*!
	* Retorna el enum del tipo de cambio estandarizado para dos strings de divisas
	* @param currency1 string que representa la primera divisa.
	* @param currency2 string que representa la segunda divisa.
	* @return enum con el tipo de cambio estándar.
	*/
	QCFxRateEnum getStandardFxRate(QCCurrencyEnum currency1, QCCurrencyEnum currency2)
	{
		string cc1, cc2;
		if (_currencyCode.find(currency1) != _currencyCode.end())
		{
			cc1 = _currencyCode.find(currency1)->second;
		}
		else
		{
			throw invalid_argument("Cannot find the first currency in QCCurrencyConverter.");
		}
		
		if (_currencyCode.find(currency2) != _currencyCode.end())
		{
			cc2 = _currencyCode.find(currency2)->second;
		}
		else
		{
			throw invalid_argument("Cannot find the second currency in QCCurrencyConverter.");
		}

		auto fxRate = _standardFxRate.find(cc1 + cc2);
		if (fxRate != _standardFxRate.end())
			return fxRate->second;
		throw invalid_argument("No corresponding Fx Rate for these currencies in QCCurrencyConverter.");
	}

	/**
	 * @fn	std::string QCCurrencyConverter::getFxRateMktCode(std::string code)
	 *
	 * @brief	Gets effects rate mkt code
	 *
	 * @author	A Diaz V
	 * @date	26-04-2019
	 *
	 * @exception	invalid_argument	Thrown when an invalid argument error condition occurs.
	 *
	 * @param	code	The code.
	 *
	 * @returns	The FX rate mkt code.
	 */
	std::string getFxRateMktCode(std::string code)
	{
		if (_fxRateMktCode.find(code) != _fxRateMktCode.end())
		{
			return _fxRateMktCode.at(code);
		}
		else
		{
			throw invalid_argument("Cannot find the market convention for " + code + ".");
		}

	}

private:
	/*!
	* Variable donde se almacenan los códigos textuales de los tipos de cambio.
	*/
	map<QCFxRateEnum, string> _fxRateCode;

	/** @brief	std::map withh key a concatenation 2 ccy ISO codes and value the market standard for the represented fx rate.
	For example: CLPUSD ---> USDCLP */
	std::map<std::string, std::string> _fxRateMktCode;


	/*!
	* Variables donde se almacenan los códigos textuales de las divisas.
	*/
	map<QCCurrencyEnum, string> _currencyCode;

	/*!
	* Estandariza un tipo de cambio representado por la concatenación de los strings que representan sus divisas.
	* Por ejemplo "USDCLP" ---> qcUSDCLP y "CLPUSD" ---> qcUSDCLP
	*/
	map<string, QCFxRateEnum> _standardFxRate;

	/*!
	* Método que devuelve la divisa fuerte en un tipo de cambio.
	*/
	string _getStrongCurrency(QCFxRateEnum fxRateCode)
	{
		return _fxRateCode[fxRateCode].substr(0, 3);
	}

	/*!
	* Método que devuelve la divisa débil en un tipo de cambio.
	*/
	string _getWeakCurrency(QCFxRateEnum fxRateCode)
	{
		return _fxRateCode[fxRateCode].substr(3, 3);
	}
};

#endif //QCCURRENCYCONVERTER_H