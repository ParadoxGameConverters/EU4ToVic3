#include "BuildingCostLoader.h"
#include "BuildingCost.h"
#include "CommonRegexes.h"
#include "OSCompatibilityLayer.h"
#include <ranges>

void EU4::BuildingCostLoader::loadBuildingCosts(const std::string& EU4Path, const Mods& mods)
{
	registerKeys();

	for (const auto& filename: commonItems::GetAllFilesInFolder(EU4Path + "/common/buildings/"))
		parseFile(EU4Path + "/common/buildings/" + filename);

	for (const auto& mod: mods)
		for (const auto& filename: commonItems::GetAllFilesInFolder(mod.path + "/common/buildings/"))
			parseFile(mod.path + "/common/buildings/" + filename);

	clearRegisteredKeywords();
}

void EU4::BuildingCostLoader::loadBuildingCosts(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::BuildingCostLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& buildingName, std::istream& theStream) {
		const BuildingCost building(theStream);
		buildingCosts.emplace(buildingName, building.getCost());
	});
}

std::optional<double> EU4::BuildingCostLoader::getBuildingCost(const std::string& buildingName) const
{
	const auto& building = buildingCosts.find(buildingName);
	if (building != buildingCosts.end())
		return building->second;
	else
		return std::nullopt;
}
