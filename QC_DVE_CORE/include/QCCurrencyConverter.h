#ifndef QCCURRENCYCONVERTER_H
#define QCCURRENCYCONVERTER_H

#include <string>
#include <map>

#include "FXRateIndex.h"
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
	Enumera todos los tipos de cambio.
	*/
	enum QCFxRateEnum
	{
		qcCLFCLP,
		qcCLPCLP,
		qcEURCLP,
		qcJPYCLP,
		qcUSDCLP,
		qcEURUSD,
		qcGBPGBP,
		qcGBPUSD,
		qcGBPCLP,
		qcUSDCLF,
		qcUSDUSD,
		qcEUREUR,
		qcCHFCHF,
		qcEURCHF,
		qcJPYJPY
	};

	/*!
	Enumera todas las divisas.
	*/
	enum QCCurrencyEnum
	{
		qcCLF,
		qcCLP,
		qcGBP,
		qcEUR,
		qcJPY,
		qcUSD,
		qcCHF
	};

	/*!
	* Constructor. Inicializa además los mapas que contienen los string que permiten identificar
	* textualmente las divisas y tipos de cambio.
	*/
	QCCurrencyConverter()
	{
		_fxRateCode.insert(pair<QCFxRateEnum, string>(qcCLFCLP, "CLFCLP"));
		_fxRateCode.insert(pair<QCFxRateEnum, string>(qcCLPCLP, "CLPCLP"));
		_fxRateCode.insert(pair<QCFxRateEnum, string>(qcEURCLP, "EURCLP"));
		_fxRateCode.insert(pair<QCFxRateEnum, string>(qcJPYCLP, "JPYCLP"));
		_fxRateCode.insert(pair<QCFxRateEnum, string>(qcUSDCLP, "USDCLP"));
		_fxRateCode.insert(pair<QCFxRateEnum, string>(qcEURUSD, "EURUSD"));
		_fxRateCode.insert(pair<QCFxRateEnum, string>(qcGBPUSD, "GBPUSD"));
		_fxRateCode.insert(pair<QCFxRateEnum, string>(qcUSDCLF, "USDCLF"));
		_fxRateCode.insert(pair<QCFxRateEnum, string>(qcUSDUSD, "USDUSD"));
		_fxRateCode.insert(pair<QCFxRateEnum, string>(qcEUREUR, "EUREUR"));
		_fxRateCode.insert(pair<QCFxRateEnum, string>(qcCHFCHF, "CHFCHF"));
		_fxRateCode.insert(pair<QCFxRateEnum, string>(qcEURCHF, "EURCHF"));
		_fxRateCode.insert(pair<QCFxRateEnum, string>(qcGBPCLP, "GBPCLP"));
		_fxRateCode.insert(pair<QCFxRateEnum, string>(qcGBPGBP, "GBPGBP"));
		_fxRateCode.insert(pair<QCFxRateEnum, string>(qcJPYJPY, "JPYJPY"));

		_currencyCode.insert(pair<QCCurrencyEnum, string>(qcCLF, "CLF"));
		_currencyCode.insert(pair<QCCurrencyEnum, string>(qcCLP, "CLP"));
		_currencyCode.insert(pair<QCCurrencyEnum, string>(qcGBP, "GBP"));
		_currencyCode.insert(pair<QCCurrencyEnum, string>(qcEUR, "EUR"));
		_currencyCode.insert(pair<QCCurrencyEnum, string>(qcJPY, "JPY"));
		_currencyCode.insert(pair<QCCurrencyEnum, string>(qcUSD, "USD"));
		_currencyCode.insert(pair<QCCurrencyEnum, string>(qcCHF, "CHF"));

		_standardFxRate.insert(pair<string, QCFxRateEnum>("CLFCLP", qcCLFCLP));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("CLPCLF", qcCLFCLP));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("CLPCLP", qcCLPCLP));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("EURCLP", qcEURCLP));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("CLPEUR", qcEURCLP));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("JPYCLP", qcJPYCLP));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("CLPJPY", qcJPYCLP));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("USDCLP", qcUSDCLP));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("CLPUSD", qcUSDCLP));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("EURUSD", qcEURUSD));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("USDEUR", qcEURUSD));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("USDCLF", qcUSDCLF));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("CLFUSD", qcUSDCLF));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("USDUSD", qcUSDUSD));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("GBPGBP", qcGBPGBP));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("GBPUSD", qcGBPUSD));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("USDGBP", qcGBPUSD));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("GBPCLP", qcGBPCLP));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("CLPGBP", qcGBPCLP));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("EUREUR", qcEUREUR));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("CHFCHF", qcCHFCHF));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("EURCHF", qcEURCHF));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("CHFEUR", qcEURCHF));
		_standardFxRate.insert(pair<string, QCFxRateEnum>("JPYJPY", qcJPYJPY));
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
	double convert(double amount, shared_ptr<QCCurrency> currency, double fxRateValue, QCode::Financial::FXRateIndex& fxRateIndex)
	{
		if (fxRateIndex.strongCcyCode() == currency->getIsoCode() )
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

private:
	/*!
	* Variable donde se almacenan los códigos textuales de los tipos de cambio.
	*/
	map<QCFxRateEnum, string> _fxRateCode;

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