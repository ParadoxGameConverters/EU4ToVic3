#include "EU4Province.h"
#include "Configuration.h"
#include "Country/EU4Country.h"
#include "Log.h"
#include "ParserHelpers.h"
#include <cmath>
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

	// for old versions of EU4 (< 1.12), copy tax to production if necessary
	if (baseProduction == 0.0 && baseTax > 0)
		baseProduction = baseTax;
}

void EU4::Province::registerKeys()
{
	registerSetter("name", name);
	registerSetter("culture", culture);
	registerSetter("religion", religion);
	registerSetter("base_tax", baseTax);
	registerSetter("base_production", baseProduction);
	// One of these is obsolete but required for old versions.
	registerSetter("base_manpower", baseManpower);
	// No idea which one.
	registerSetter("manpower", baseManpower);
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
		buildings = theBuildings;
	});
	registerKeyword("great_projects", [this](std::istream& theStream) {
		const auto& projectList = commonItems::getStrings(theStream);
		greatProjects.insert(projectList.begin(), projectList.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void EU4::Province::buildPopRatio(const mappers::SuperGroupMapper& superGroupMapper, const Regions& regions)
{
	double assimilationFactor;
	const auto& superRegionName = regions.getParentSuperRegionName(num);
	if (superRegionName)
		assimilationFactor = superGroupMapper.getAssimilationFactor(*superRegionName);
	else
		assimilationFactor = 0.0025;
	provinceHistory.buildPopRatios(assimilationFactor);
}

double EU4::Province::getCulturePercent(const std::string& theCulture) const
{
	auto culturePercent = 0.0;
	for (const auto& pop: provinceHistory.getPopRatios())
		if (pop.getCulture() == theCulture)
			culturePercent += pop.getLowerRatio();

	return culturePercent;
}

void EU4::Province::determineProvinceWeight(const mappers::Buildings& buildingTypes)
{
	auto buildingWeight = 0.0;
	for (const auto& buildingName: buildings.getBuildings())
	{
		const auto& theBuilding = buildingTypes.getBuilding(buildingName);
		if (theBuilding)
			buildingWeight += theBuilding->getCost();
	}
	// This is the cost of all built buildings, scaled to 1% (every 100 gold in buildings = 1 dev).
	buildingWeight /= 100;

	// This is how much dev there is and how much was invested in the province.
	const auto currentDevelopment = baseTax + baseProduction + baseManpower;
	const auto developmentDelta = currentDevelopment - provinceHistory.getOriginalDevelopment();

	// Province weight is absolute dev + buildings. It is used in extreme popShaping.
	provinceWeight = currentDevelopment + buildingWeight;

	// Investment weight is invested dev + buildings. Its FACTOR is used in devPush popShaping.
	const auto investedWeight = developmentDelta + buildingWeight;

	if (investedWeight > 0)
	{
		// provinces with modifier weights under 10 (under-invested with no buildings) get a penalty for popShaping, (realistically) up to -10.
		investmentFactor = (std::log10(investedWeight) - 1) * 10;
	}
	else
	{
		investmentFactor = -10 + investedWeight / 10; // For negatives, go linearly into debt.
	}

	// both reshapings only work on owned provinces.
	if (ownerString.empty())
	{
		provinceWeight = 0;
		investmentFactor = 0;
	}
}
