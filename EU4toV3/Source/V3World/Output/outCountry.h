#ifndef OUT_COUNTRY_H
#define OUT_COUNTRY_H
#include "PoliticalManager/CountryDefinitionLoader/Country.h"

namespace V3
{
std::ostream& operator<<(std::ostream& output, const Country& country);

} // namespace V3

#endif // OUT_COUNTRY_H