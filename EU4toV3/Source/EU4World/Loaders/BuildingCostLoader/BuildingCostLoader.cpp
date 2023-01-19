#include "BuildingCostLoader.h"
#include "BuildingCost.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "OSCompatibilityLayer.h"
#include <ranges>

void EU4::BuildingCostLoader::loadBuildingCosts(const commonItems::ModFilesystem& modFS)
{
	registerKeys();

	for (const auto& file: modFS.GetAllFilesInFolder("/common/buildings/"))
	{
		if (getExtension(file) != "txt")
			continue;
		parseFile(file);
	}

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
