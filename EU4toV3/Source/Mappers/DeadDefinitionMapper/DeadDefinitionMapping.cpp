#include "DeadDefinitionMapping.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::DeadDefinitionMapping::DeadDefinitionMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::DeadDefinitionMapping::registerKeys()
{
	registerKeyword("tag", [this](const std::string& unused, std::istream& theStream) {
		deadDefinition.tag = commonItems::singleString(theStream).getString();
	});
	registerKeyword("primary_culture", [this](const std::string& unused, std::istream& theStream) {
		deadDefinition.culture = commonItems::singleString(theStream).getString();
	});
	registerKeyword("religion", [this](const std::string& unused, std::istream& theStream) {
		deadDefinition.religion = commonItems::singleString(theStream).getString();
	});
	registerKeyword("government", [this](const std::string& unused, std::istream& theStream) {
		deadDefinition.government = commonItems::singleString(theStream).getString();
	});
	registerKeyword("civilized", [this](const std::string& unused, std::istream& theStream) {
		deadDefinition.civilized = commonItems::singleString(theStream).getString() == "yes";
	});
	registerKeyword("capital", [this](const std::string& unused, std::istream& theStream) {
		deadDefinition.capital = commonItems::singleInt(theStream).getInt();
	});
	registerKeyword("add_accepted_culture", [this](const std::string& unused, std::istream& theStream) {
		deadDefinition.acceptedCultures.insert(commonItems::getString(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
