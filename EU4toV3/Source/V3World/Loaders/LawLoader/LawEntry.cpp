#include "LawEntry.h"
#include "CommonRegexes.h"
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
	modifierParser.registerKeyword("state_building_barracks_max_level_add", [this](std::istream& theStream) {
		law.barracksMax = commonItems::getInt(theStream);
	});
	modifierParser.registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
