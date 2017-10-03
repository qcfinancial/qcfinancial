#ifndef INTERESTRATEINDEX_H
#define INTERESTRATEINDEX_H

#include<string>
#include<map>
#include<utility>

namespace QCode
{
	namespace Financial
	{
		enum InterestRateIndex
		{
			
			///< An enum constant representing Libor USD 1M
			US0001M,

			///< An enum constant representing Libor USD 3M
			US0003M,

			///< An enum constant representing Libor USD 6M
			US0006M,

			///< An enum constant representing Libor USD 1Y (12M)
			US0012M,

			///< An enum constant representing TAB 30 CLP
			TAB30CLP,

			///< An enum constant representing TAB 90 CLP
			TAB90CLP,

			///< An enum constant representing TAB 180 CLP
			TAB180CLP,

			///< An enum constant representing TAB 360 CLP
			TAB360CLP,

			///< An enum constant representing TAB 90 UF
			TAB90UF,

			///< An enum constant representing TAB 180 UF
			TAB180UF,

			///< An enum constant representing TAB 360 UF
			TAB360UF,

			///< An enum constant representing Effective Fed Funds
			FDFD,

			///< An enum constant representing Tasa Politica Monetaria in Chile
			TPM,

			///< An enum constant representing 1D Expected Tasa Politica Monetaria in Chile
			ETPM,

			///< An enum constant representing the 1D rate in USD in Chile
			CLD1
		};

		inline std::string irIndexCode(InterestRateIndex irIndex)
		{
			std::map<InterestRateIndex, std::string> codes;
			codes.insert(std::pair<InterestRateIndex, std::string>(US0001M, "LIBORUSD1M"));
			codes.insert(std::pair<InterestRateIndex, std::string>(US0003M, "LIBORUSD3M"));
			codes.insert(std::pair<InterestRateIndex, std::string>(US0006M, "LIBORUSD6M"));
			codes.insert(std::pair<InterestRateIndex, std::string>(US0012M, "LIBORUSD12M"));
			codes.insert(std::pair<InterestRateIndex, std::string>(TAB30CLP, "TABCLP30D"));
			codes.insert(std::pair<InterestRateIndex, std::string>(TAB90CLP, "TABCLP90D"));
			codes.insert(std::pair<InterestRateIndex, std::string>(TAB180CLP, "TABCLP180D"));
			codes.insert(std::pair<InterestRateIndex, std::string>(TAB360CLP, "TABCLP360D"));
			codes.insert(std::pair<InterestRateIndex, std::string>(TAB90UF, "TABCLF90D"));
			codes.insert(std::pair<InterestRateIndex, std::string>(TAB180UF, "TABCLF180D"));
			codes.insert(std::pair<InterestRateIndex, std::string>(TAB360UF, "TABCLF360D"));
			codes.insert(std::pair<InterestRateIndex, std::string>(FDFD, "FDFD"));
			codes.insert(std::pair<InterestRateIndex, std::string>(TPM, "TPM"));
			codes.insert(std::pair<InterestRateIndex, std::string>(ETPM, "ETPM"));
			codes.insert(std::pair<InterestRateIndex, std::string>(CLD1, "CLD1D"));

			return codes.at(irIndex);
		}
	
		inline InterestRateIndex test(InterestRateIndex ir)
		{
			return ir;
		}
	}
}
#endif