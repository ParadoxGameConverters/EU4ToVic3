#include "NationalBudgetLoader.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void V3::NationalBudgetLoader::loadNationalBudget(const std::string& filePath)
{
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << sectorBlueprints.size() << " sectors of industry loaded.";
}

void V3::NationalBudgetLoader::buildBuildingSectorMap()
{
	for (const auto& blueprint: sectorBlueprints)
		for (const auto& building: blueprint.getBuildings())
			buildingSectorMap[building] = blueprint.getName();
}

std::optional<std::string> V3::NationalBudgetLoader::getSectorName(const std::string& buildingName) const
{
	if (buildingSectorMap.contains(buildingName))
	{
		return buildingSectorMap.at(buildingName);
	}
	return std::nullopt;
}

void V3::NationalBudgetLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& sectorName, std::istream& theStream) {
		SectorLoader newSectorBlueprint;
		newSectorBlueprint.loadSector(theStream);
		newSectorBlueprint.setName(sectorName);
		sectorBlueprints.push_back(newSectorBlueprint);
	});
}
