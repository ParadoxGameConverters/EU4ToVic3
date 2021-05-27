#include "EU4Province.h"
#include "CommonRegexes.h"
#include "Configuration.h"
#include "Log.h"
#include "ParserHelpers.h"

EU4::Province::Province(const std::string& numString, std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();

	provID = 0 - stoi(numString);
}

void EU4::Province::registerKeys()
{
	registerSetter("name", name);
	registerSetter("culture", culture);
	registerSetter("religion", religion);
	registerSetter("base_tax", baseTax);
	registerSetter("base_production", baseProduction);
	registerSetter("base_manpower", baseManpower);
	registerSetter("owner", ownerTag);
	registerSetter("controller", controllerTag);
	registerKeyword("cores", [this](std::istream& theStream) {
		const auto& coreList = commonItems::getStrings(theStream);
		cores.insert(coreList.begin(), coreList.end());
	});
	registerKeyword("territorial_core", [this](std::istream& theStream) {
		commonItems::ignoreItem("unused", theStream);
		territorialCore = true;
	});
	registerKeyword("hre", [this](std::istream& theStream) {
		inHRE = commonItems::getString(theStream) == "yes";
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
