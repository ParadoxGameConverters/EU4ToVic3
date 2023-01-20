#include "LawEntry.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

V3::LawEntry::LawEntry(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::LawEntry::registerKeys()
{
	registerKeyword("group", [this](std::istream& theStream) {
		law.group = commonItems::getString(theStream);
	});
	registerKeyword("progressiveness", [this](std::istream& theStream) {
		law.progressiveness = commonItems::getInt(theStream);
	});
	registerKeyword("unlocking_technologies", [this](std::istream& theStream) {
		for (const auto& entry: commonItems::getStrings(theStream))
			law.requiredTechs.emplace(entry);
	});
	registerKeyword("unlocking_laws", [this](std::istream& theStream) {
		for (const auto& entry: commonItems::getStrings(theStream))
			law.requiredLaws.emplace(entry);
	});
	registerKeyword("disallowing_laws", [this](std::istream& theStream) {
		for (const auto& entry: commonItems::getStrings(theStream))
			law.blockingLaws.emplace(entry);
	});
	registerKeyword("institution", [this](std::istream& theStream) {
		law.institution = commonItems::getString(theStream);
	});
	registerKeyword("modifier", [this](std::istream& theStream) {
		modifierParser.parseStream(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	modifierParser.registerKeyword("state_bureaucracy_population_base_cost_factor_mult", [this](std::istream& theStream) {
		law.bureaucracyCostMult = commonItems::getDouble(theStream);
	});
	modifierParser.registerRegex("state_building_[a-zA-Z_]+_max_level_add", [this](const std::string& modifier, std::istream& theStream) {
		const std::regex pattern("state_building_([a-zA-Z_]+)_max_level_add");
		std::smatch building;
		if (std::regex_search(modifier, building, pattern)) // state_building_barracks_max_level_add -> barracks
		{
			law.maxBuildingLevels["building_" + building[1].str()] = commonItems::getInt(theStream);
		}
		else
		{
			Log(LogLevel::Error) << "Found a max level modifier, but could not parse it: " << modifier;
		}
	});
	modifierParser.registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
