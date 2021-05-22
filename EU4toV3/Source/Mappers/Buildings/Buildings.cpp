#include "Buildings.h"
#include "Configuration.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::Buildings::Buildings()
{
	registerKeys();

	for (const auto& filename: commonItems::GetAllFilesInFolder(theConfiguration.getEU4Path() + "/common/buildings/"))
		parseFile(theConfiguration.getEU4Path() + "/common/buildings/" + filename);

	for (const auto& modName: theConfiguration.getEU4Mods())
		for (const auto& filename: commonItems::GetAllFilesInFolder(modName + "/common/buildings/"))
			parseFile(modName + "/common/buildings/" + filename);

	clearRegisteredKeywords();
}

mappers::Buildings::Buildings(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::Buildings::registerKeys()
{
	registerRegex("[a-zA-Z0-9_]+", [this](const std::string& buildingName, std::istream& theStream) {
		Building building(theStream);
		buildings.insert(std::make_pair(buildingName, building));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<mappers::Building> mappers::Buildings::getBuilding(const std::string& buildingName) const
{
	if (const auto& building = buildings.find(buildingName); building != buildings.end())
		return building->second;
	else
		return std::nullopt;
}
