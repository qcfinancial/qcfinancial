//
// BusinessCalendarId.h
//
// Identifiers for the market calendars that CalendarFactory can build, using
// FpML business-center codes as the enum symbols.
//

#ifndef QC_BUSINESSCALENDARID_H
#define QC_BUSINESSCALENDARID_H

#include <string>

/*!
 * @brief Identificadores de calendarios de mercado, usando los códigos de centro
 * de negocios de FpML.
 *
 * - CLSA: Santiago, Chile (feriados públicos)
 * - CLBA: Chile, calendario bancario (CLSA + 31 de diciembre, feriado bancario)
 * - USNY: Estados Unidos, banca de Nueva York (settlement SIFMA)
 * - USGS: Estados Unidos, government securities (mercado de bonos SIFMA)
 * - EUTA: Eurozona, sistema de pagos TARGET
 */
enum class BusinessCalendarId
{
    CLSA,
    CLBA,
    USNY,
    USGS,
    EUTA
};

/*!
 * Retorna el código FpML de un BusinessCalendarId (ej. "CLSA").
 */
std::string fpmlCode(BusinessCalendarId id);

/*!
 * Retorna una descripción legible del calendario.
 */
std::string description(BusinessCalendarId id);

/*!
 * Inversa de fpmlCode: retorna el BusinessCalendarId asociado a un código FpML.
 * Lanza std::invalid_argument si el código no corresponde a ningún calendario.
 */
BusinessCalendarId fromFpmlCode(const std::string& code);

#endif //QC_BUSINESSCALENDARID_H
