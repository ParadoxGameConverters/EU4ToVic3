#ifndef OUT_LOCALIZATIONS_H
#define OUT_LOCALIZATIONS_H
#include "PoliticalManager/Country/Country.h"
#include "ReligionMapper/ReligionDefinitionLoader/ReligionDef.h"

namespace OUT
{
void exportCountryNamesAndAdjectives(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries);
void exportReligionLocs(const std::string& outputName, const std::map<std::string, mappers::ReligionDef>& religions);

} // namespace OUT

#endif // OUT_LOCALIZATIONS_H