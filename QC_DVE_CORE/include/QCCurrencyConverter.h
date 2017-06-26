#ifndef QCCURRENCYCONVERTER_H
#define QCCURRENCYCONVERTER_H

#include <string>
#include <map>

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
	enum QCFxRate
	{
		qcCLFCLP,
		qcCLPCLP,
		qcEURCLP,
		qcJPYCLP,
		qcUSDCLP,
		qcEURUSD,
		qcGBPUSD,
		qcUSDCLF,
		qcUSDUSD
	};

	/*!
	Enumera todas las divisas.
	*/
	enum QCCurrency
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
		_fxRateCode.insert(pair<QCFxRate, string>(qcCLFCLP, "CLFCLP"));
		_fxRateCode.insert(pair<QCFxRate, string>(qcCLPCLP, "CLPCLP"));
		_fxRateCode.insert(pair<QCFxRate, string>(qcEURCLP, "EURCLP"));
		_fxRateCode.insert(pair<QCFxRate, string>(qcJPYCLP, "JPYCLP"));
		_fxRateCode.insert(pair<QCFxRate, string>(qcUSDCLP, "USDCLP"));
		_fxRateCode.insert(pair<QCFxRate, string>(qcEURUSD, "EURUSD"));
		_fxRateCode.insert(pair<QCFxRate, string>(qcGBPUSD, "GBPUSD"));
		_fxRateCode.insert(pair<QCFxRate, string>(qcUSDCLF, "USDCLF"));
		_fxRateCode.insert(pair<QCFxRate, string>(qcUSDUSD, "USDUSD"));

		_currencyCode.insert(pair<QCCurrency, string>(qcCLF, "CLF"));
		_currencyCode.insert(pair<QCCurrency, string>(qcCLP, "CLP"));
		_currencyCode.insert(pair<QCCurrency, string>(qcGBP, "GBP"));
		_currencyCode.insert(pair<QCCurrency, string>(qcEUR, "EUR"));
		_currencyCode.insert(pair<QCCurrency, string>(qcJPY, "JPY"));
		_currencyCode.insert(pair<QCCurrency, string>(qcUSD, "USD"));

		_standardFxRate.insert(pair<string, QCFxRate>("CLFCLP", qcCLFCLP));
		_standardFxRate.insert(pair<string, QCFxRate>("CLPCLF", qcCLFCLP));
		_standardFxRate.insert(pair<string, QCFxRate>("CLPCLP", qcCLPCLP));
		_standardFxRate.insert(pair<string, QCFxRate>("EURCLP", qcEURCLP));
		_standardFxRate.insert(pair<string, QCFxRate>("CLPEUR", qcEURCLP));
		_standardFxRate.insert(pair<string, QCFxRate>("JPYCLP", qcJPYCLP));
		_standardFxRate.insert(pair<string, QCFxRate>("CLPJPY", qcJPYCLP));
		_standardFxRate.insert(pair<string, QCFxRate>("USDCLP", qcUSDCLP));
		_standardFxRate.insert(pair<string, QCFxRate>("CLPUSD", qcUSDCLP));
		_standardFxRate.insert(pair<string, QCFxRate>("EURUSD", qcEURUSD));
		_standardFxRate.insert(pair<string, QCFxRate>("USDEUR", qcEURUSD));
		_standardFxRate.insert(pair<string, QCFxRate>("USDCLF", qcUSDCLF));
		_standardFxRate.insert(pair<string, QCFxRate>("CLFUSD", qcUSDCLF));
		_standardFxRate.insert(pair<string, QCFxRate>("USDUSD", qcUSDUSD));
		_standardFxRate.insert(pair<string, QCFxRate>("GBPUSD", qcGBPUSD));
		_standardFxRate.insert(pair<string, QCFxRate>("USDGBP", qcGBPUSD));
	}

	/*!
	* Convierte un monto en un moneda a otra.
	* @param amount, monto de la moneda a convertir.
	* @param currency, moneda del monto a convertir.
	* @fxRateValue, valor del índice a utilizar en la conversión.
	* @fxRate, qué tipo de cambio representa el valor anterior.
	*/
	double convert(double amount, QCCurrency currency, double fxRateValue, QCFxRate fxRate)
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

	/*!
	* Retorna el enum de una divisa si se da el código en texto.
	*/
	QCCurrency getCurrencyEnum(string code)
	{
		for (auto& x : _currencyCode)
		{
			if (x.second == code)
				return x.first;
		}
		throw invalid_argument("No corresponding currency enum for this currency code.");
	}

	/*!
	* Retorna el enum de un tipo de cambio si se da el código en texto.
	*/
	QCFxRate getFxRateEnum(string code)
	{
		for (auto& x : _fxRateCode)
		{
			if (x.second == code)
				return x.first;
		}
		throw invalid_argument("No corresponding fx rate enum for this fx rate code.");
	}

	/*!
	* Retorna el enum de la divisa fuerte de un tipo de cambio si se da el código en texto.
	*/
	QCCurrency getStrongCurrencyEnum(string code)
	{
		auto fx = getFxRateEnum(code);
		auto strong = _getStrongCurrency(fx);
		return getCurrencyEnum(strong);
	}

	/*!
	* Retorna el enum de la divisa debil de un tipo de cambio si se da el código en texto.
	*/
	QCCurrency getWeakCurrencyEnum(string code)
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
	QCFxRate getStandardFxRate(QCCurrency currency1, QCCurrency currency2)
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
	map<QCFxRate, string> _fxRateCode;

	/*!
	* Variables donde se almacenan los códigos textuales de las divisas.
	*/
	map<QCCurrency, string> _currencyCode;

	/*!
	* Estandariza un tipo de cambio representado por la concatenación de los strings que representan sus divisas.
	* Por ejemplo "USDCLP" ---> qcUSDCLP y "CLPUSD" ---> qcUSDCLP
	*/
	map<string, QCFxRate> _standardFxRate;

	/*!
	* Método que devuelve la divisa fuerte en un tipo de cambio.
	*/
	string _getStrongCurrency(QCFxRate fxRateCode)
	{
		return _fxRateCode[fxRateCode].substr(0, 3);
	}

	/*!
	* Método que devuelve la divisa débil en un tipo de cambio.
	*/
	string _getWeakCurrency(QCFxRate fxRateCode)
	{
		return _fxRateCode[fxRateCode].substr(3, 3);
	}
};

#endif //QCCURRENCYCONVERTER_H