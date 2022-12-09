#ifndef OUT_LOCALIZATIONS_H
#define OUT_LOCALIZATIONS_H
#include "CultureMapper/CultureDefinitionLoader/CultureDef.h"
#include "PoliticalManager/Country/Country.h"
#include "ReligionMapper/ReligionDefinitionLoader/ReligionDef.h"

namespace OUT
{
void exportCountryNamesAndAdjectives(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries);
void exportReligionLocs(const std::string& outputName, const std::map<std::string, mappers::ReligionDef>& religions);
void exportCultureLocs(const std::string& outputName, const std::map<std::string, mappers::CultureDef>& cultures);

} // namespace OUT

#endif // OUT_LOCALIZATIONS_H