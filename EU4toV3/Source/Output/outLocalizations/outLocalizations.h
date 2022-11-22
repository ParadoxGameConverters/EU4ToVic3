#ifndef OUT_LOCALIZATIONS_H
#define OUT_LOCALIZATIONS_H
#include "PoliticalManager/CountryDefinitionLoader/Country.h"

namespace OUT
{
void exportCountryNamesAndAdjectives(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries);

} // namespace OUT

#endif // OUT_LOCALIZATIONS_H