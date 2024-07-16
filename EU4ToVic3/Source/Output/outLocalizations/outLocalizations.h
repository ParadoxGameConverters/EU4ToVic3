#ifndef OUT_LOCALIZATIONS_H
#define OUT_LOCALIZATIONS_H
#include "CultureMapper/CultureDefinitionLoader/CultureDef.h"
#include "PoliticalManager/Country/Country.h"
#include "PoliticalManager/Diplomacy/PowerBloc.h"
#include "ReligionMapper/ReligionDefinitionLoader/ReligionDef.h"

namespace OUT
{
void exportCountryNamesAndAdjectives(const std::string& outputName,
	 const std::map<std::string, std::shared_ptr<V3::Country>>& countries,
	 const V3::LocalizationLoader& knownLocs);
void exportReligionLocs(const std::string& outputName, const std::map<std::string, mappers::ReligionDef>& religions, const V3::LocalizationLoader& knownLocs);
void exportCultureLocs(const std::string& outputName, const std::map<std::string, mappers::CultureDef>& cultures, const V3::LocalizationLoader& knownLocs);
void exportCharacterLocs(const std::string& outputName,
	 const std::map<std::string, std::shared_ptr<V3::Country>>& countries,
	 const V3::LocalizationLoader& knownLocs);
void exportPowerBlocLocs(const std::string& outputName, const std::vector<V3::PowerBloc>& powerBlocs);

} // namespace OUT

#endif // OUT_LOCALIZATIONS_H