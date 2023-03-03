#include "TechEntry.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

V3::TechEntry::TechEntry(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::TechEntry::registerKeys()
{
	registerKeyword("modifier", [this](std::istream& theStream) {
		modifierParser.parseStream(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	modifierParser.registerKeyword("state_infrastructure_from_population_add", [this](std::istream& theStream) {
		tech.infrastructureMult = commonItems::getDouble(theStream);
	});
	modifierParser.registerKeyword("state_infrastructure_from_population_max_add", [this](std::istream& theStream) {
		tech.infrastructureMax = commonItems::getInt(theStream);
	});
	modifierParser.registerRegex("state_building_[a-zA-Z_]+_max_level_add", [this](const std::string& modifier, std::istream& theStream) {
		const std::regex pattern("state_building_([a-zA-Z_]+)_max_level_add");
		std::smatch building;
		if (std::regex_search(modifier, building, pattern)) // state_building_port_max_level_add -> port
		{
			tech.maxBuildingLevels["building_" + building[1].str()] = commonItems::getInt(theStream);
		}
		else
		{
			Log(LogLevel::Error) << "Found a max level modifier, but could not parse it: " << modifier;
		}
	});
	modifierParser.registerKeyword("building_economy_of_scale_level_cap_add", [this](std::istream& theStream) {
		tech.throughputMax = commonItems::getInt(theStream);
	});
	modifierParser.registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
