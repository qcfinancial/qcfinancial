#ifndef TENOR_H
#define TENOR_H

#include<regex>
#include<string>

namespace QCode
{
	namespace Financial
	{
		class Tenor
		{
		public:
			Tenor(std::string tenor);
			void setTenor(std::string tenor);
			unsigned int getYears();
			unsigned int getMonths();
			unsigned int getDays();
			std::string getString();
			~Tenor();

		private:
			std::string _tenor;
			bool _validate(std::string tenor);
		};
	}
}
#endif //TENOR_H