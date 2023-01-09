#include "BuildingLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "EconomyManager/Building/Building.h"
#include "Loaders/DefinesLoader/BuildingScriptValuesLoader.h"
#include "ParserHelpers.h"

void V3::BuildingLoader::loadBuildings(const commonItems::ModFilesystem& modFS)
{
	BuildingScriptValuesLoader bsvl;
	bsvl.loadBuildingScriptValues(modFS);

	registerKeys(bsvl.getBuildingCostConstants());
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/buildings/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(fileName);
	}
	clearRegisteredKeywords();
}

void V3::BuildingLoader::registerKeys(const std::map<std::string, int>& costTiers)
{
	registerRegex(commonItems::catchallRegex, [this, costTiers](const std::string& buildingName, std::istream& theStream) {
		auto newBuilding = std::make_unique<Building>();
		newBuilding->loadBuilding(theStream, costTiers);
		newBuilding->setName(buildingName);
		buildings.emplace(buildingName, std::move(newBuilding));
	});
}
