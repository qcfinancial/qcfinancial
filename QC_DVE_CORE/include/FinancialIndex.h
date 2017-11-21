#ifndef FINANCIALINDEX_H
#define FINANCIALINDEX_H

#include<string>

namespace QCode
{
	namespace Financial
	{
		class FinancialIndex
		{
		public:
			enum AssetClass
			{
				InterestRate,
				InterestRateVolatility,
				Fx,
				FxVolatility,
				Equity,
				EquityVolatility,
				Commodity,
				CommodityVolatility,
				Credit,
				CreditVolatility
			};

			FinancialIndex(AssetClass assetClass,
						   std::string code) :
						   _assetClass(assetClass),
						   _code(code)
			{
			}

			virtual AssetClass getAssetClass()
			{
				return _assetClass;
			}

			virtual std::string getCode()
			{
				return _code;
			}

			virtual ~FinancialIndex()
			{
			}

		protected:
			AssetClass _assetClass;
			std::string _code;
		};
	}
}
#endif //FINANCIALINDEX_H