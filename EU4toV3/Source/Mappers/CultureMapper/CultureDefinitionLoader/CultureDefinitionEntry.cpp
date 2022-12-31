#include "CultureDefinitionEntry.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

mappers::CultureDefinitionEntry::CultureDefinitionEntry(std::istream& theStream, bool skipExport)
{
	cultureDef.skipExport = skipExport;
	registerkeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::CultureDefinitionEntry::registerkeys()
{
	ethStripper.registerRegex(R"(\d+)", [this](const std::string& unused, std::istream& theStream) {
		cultureDef.ethnicities.emplace(commonItems::getString(theStream));
	});
	ethStripper.registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	registerKeyword("color", [this](std::istream& theStream) {
		cultureDef.color = commonItems::Color::Factory().getColor(theStream);
	});
	registerKeyword("religion", [this](std::istream& theStream) {
		cultureDef.religion = commonItems::getString(theStream);
	});
	registerKeyword("traits", [this](std::istream& theStream) {
		for (const auto& trait: commonItems::getStrings(theStream))
			cultureDef.traits.emplace(trait);
	});
	registerKeyword("male_common_first_names", [this](std::istream& theStream) {
		for (const auto& name: commonItems::getStrings(theStream))
			cultureDef.maleCommonFirstNames.emplace(name);
	});
	registerKeyword("female_common_first_names", [this](std::istream& theStream) {
		for (const auto& name: commonItems::getStrings(theStream))
			cultureDef.femaleCommonFirstNames.emplace(name);
	});
	registerKeyword("noble_last_names", [this](std::istream& theStream) {
		for (const auto& name: commonItems::getStrings(theStream))
			cultureDef.nobleLastNames.emplace(name);
	});
	registerKeyword("common_last_names", [this](std::istream& theStream) {
		for (const auto& name: commonItems::getStrings(theStream))
			cultureDef.commonLastNames.emplace(name);
	});
	registerKeyword("male_regal_first_names", [this](std::istream& theStream) {
		for (const auto& name: commonItems::getStrings(theStream))
			cultureDef.maleRegalFirstNames.emplace(name);
	});
	registerKeyword("female_regal_first_names", [this](std::istream& theStream) {
		for (const auto& name: commonItems::getStrings(theStream))
			cultureDef.femaleRegalFirstNames.emplace(name);
	});
	registerKeyword("regal_last_names", [this](std::istream& theStream) {
		for (const auto& name: commonItems::getStrings(theStream))
			cultureDef.regalLastNames.emplace(name);
	});
	registerKeyword("ethnicities", [this](std::istream& theStream) {
		ethStripper.parseStream(theStream);
	});
	registerKeyword("graphics", [this](std::istream& theStream) {
		cultureDef.graphics = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
