//
// BusinessCalendarId.cpp
//

#include <stdexcept>

#include "time/BusinessCalendarId.h"

std::string fpmlCode(BusinessCalendarId id)
{
    switch (id)
    {
        case BusinessCalendarId::CLSA: return "CLSA";
        case BusinessCalendarId::USNY: return "USNY";
        case BusinessCalendarId::USGS: return "USGS";
        case BusinessCalendarId::EUTA: return "EUTA";
    }
    throw std::invalid_argument("Unknown BusinessCalendarId");
}

std::string description(BusinessCalendarId id)
{
    switch (id)
    {
        case BusinessCalendarId::CLSA: return "Santiago, Chile - bank holidays";
        case BusinessCalendarId::USNY: return "United States - New York banking (SIFMA settlement)";
        case BusinessCalendarId::USGS: return "United States - government securities (SIFMA bond market)";
        case BusinessCalendarId::EUTA: return "Eurozone - TARGET payment system";
    }
    throw std::invalid_argument("Unknown BusinessCalendarId");
}

BusinessCalendarId fromFpmlCode(const std::string& code)
{
    if (code == "CLSA") return BusinessCalendarId::CLSA;
    if (code == "USNY") return BusinessCalendarId::USNY;
    if (code == "USGS") return BusinessCalendarId::USGS;
    if (code == "EUTA") return BusinessCalendarId::EUTA;
    throw std::invalid_argument("Unknown FpML calendar code: " + code);
}
