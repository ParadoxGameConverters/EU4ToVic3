#include "BuildingMapper.h"
#include "BuildingMapping.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void mappers::BuildingMapper::loadBuildingMappings(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Parsing building mappings.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << mappings.size() << " buildings loaded.";
}

void mappers::BuildingMapper::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		const BuildingMapping newMapping(theStream);
		mappings.emplace(newMapping.getEU4Building(), newMapping.getVic3Buildings());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::set<std::string> mappers::BuildingMapper::getVic3Buildings(const std::string& eu4Building) const
{
	if (mappings.contains(eu4Building))
	{
		return mappings.at(eu4Building);
	}

	// Many buildings will not have mappings, this is expected.
	return {};
}