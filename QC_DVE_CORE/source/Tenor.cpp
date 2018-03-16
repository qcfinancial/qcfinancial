#include"Tenor.h"

#include<iostream>

namespace QCode
{
	namespace Financial
	{
		const std::regex tenorYears{ "[0-9]+[yY]" };
		const std::regex tenorMonths{ "[0-9]+[mM]" };
		const std::regex tenorDays{ "[0-9]+[dD]" };

		Tenor::Tenor(std::string tenor)
		{
			setTenor(tenor);
		}

		void Tenor::setTenor(std::string tenor)
		{
			if (_validate(tenor))
			{
				_tenor = tenor;
			}
			else
			{
				throw std::invalid_argument("String tenor does not contain a valid Tenor");
			}

		}

		unsigned int Tenor::getYears()
		{
			if (std::regex_search(_tenor.c_str(), tenorYears))
			{
				std::smatch sm;
				std::regex_search(_tenor, sm, tenorYears);
				std::string temp{ std::string(sm[0]) };
				temp.pop_back();
				return std::stoi(temp);
			}
			else
			{
				return 0;
			}
		}

		unsigned int Tenor::getMonths()
		{
			if (std::regex_search(_tenor.c_str(), tenorMonths))
			{
				std::smatch sm;
				std::regex_search(_tenor, sm, tenorMonths);
				std::string temp{ std::string(sm[0]) };
				temp.pop_back();
				return std::stoi(temp);
			}
			else
			{
				return 0;
			}
		}

		unsigned int Tenor::getDays()
		{
			if (std::regex_search(_tenor.c_str(), tenorDays))
			{
				std::smatch sm;
				std::regex_search(_tenor, sm, tenorDays);
				std::string temp{ std::string(sm[0]) };
				temp.pop_back();
				return std::stoi(temp);
			}
			else
			{
				return 0;
			}
		}

		std::string Tenor::getString()
		{
			unsigned int years = getYears();
			unsigned int months = getMonths();
			unsigned int days = getDays();

			std::string strYears;
			std::string strMonths;
			std::string strDays;

			(years == 0) ? strYears = "" : (strYears = std::to_string(years) + "Y");
			(months == 0) ? strMonths = "" : (strYears = std::to_string(months) + "M");
			(days == 0) ? strDays = "" : (strYears = std::to_string(days) + "D");

			std::string result;
			(strYears + strMonths + strDays == "") ? (result = "0D") : (result = strYears + strMonths + strDays);
			return result;
		}

		bool Tenor::_validate(std::string tenor)
		{
			bool result{ false };
			result = result || std::regex_search(tenor.c_str(), tenorYears);
			result = result || std::regex_search(tenor.c_str(), tenorMonths);
			result = result || std::regex_search(tenor.c_str(), tenorDays);
			return result;
		}

		Tenor::~Tenor()
		{
		}
	}
}
