#include "EU4Province.h"
#include "Configuration.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::Province::Province(const std::string& numString, std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();

	num = 0 - stoi(numString);

	if (!provinceHistory.hasInitializedHistory() && !culture.empty() && !religion.empty())
	{
		// recover from broken save data.
		provinceHistory.setStartingCulture(culture);
		provinceHistory.setStartingReligion(religion);
	} // Else it's probably a blank province anyway.

}

void EU4::Province::registerKeys()
{
	registerSetter("name", name);
	registerSetter("culture", culture);
	registerSetter("religion", religion);
	registerSetter("base_tax", baseTax);
	registerSetter("base_production", baseProduction);
	registerSetter("base_manpower", baseManpower);
	registerSetter("owner", ownerString);
	registerSetter("controller", controllerString);
	registerKeyword("cores", [this](std::istream& theStream) {
		const auto& coreList = commonItems::getStrings(theStream);
		cores.insert(coreList.begin(), coreList.end());
	});
	registerKeyword("core", [this](std::istream& theStream) {
		cores.insert(commonItems::getString(theStream));
	});
	registerKeyword("territorial_core", [this](std::istream& theStream) {
		commonItems::ignoreItem("unused", theStream);
		territorialCore = true;
	});
	registerKeyword("hre", [this](std::istream& theStream) {
		inHRE = commonItems::getString(theStream) == "yes";
	});
	registerKeyword("is_city", [this](std::istream& theStream) {
		city = commonItems::getString(theStream) == "yes";
	});
	registerKeyword("colonysize", [this](std::istream& theStream) {
		commonItems::ignoreItem("unused", theStream);
		colony = true;
	});
	registerKeyword("original_coloniser", [this]( std::istream& theStream) {
		commonItems::ignoreItem("unused", theStream);
		hadOriginalColonizer = true;
	});
	registerKeyword("history", [this](std::istream& theStream) {
		const ProvinceHistory theHistory(theStream);
		provinceHistory = theHistory;
	});
	registerKeyword("buildings", [this](std::istream& theStream) {
		const ProvinceBuildings theBuildings(theStream);
		buildings = theBuildings.getBuildings();
	});
	registerKeyword("great_projects", [this](std::istream& theStream) {
		const auto& projectList = commonItems::getStrings(theStream);
		greatProjects.insert(projectList.begin(), projectList.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

