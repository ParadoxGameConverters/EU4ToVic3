#include "NameListEntry.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::NameListEntry::NameListEntry(std::istream& theStream)
{
	registerkeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::NameListEntry::registerkeys()
{
	registerKeyword("dynasty_names", [this](std::istream& theStream) {
		for (const auto& name: commonItems::getStrings(theStream))
			dynastyNames.emplace(name);
	});
	registerKeyword("male_names", [this](std::istream& theStream) {
		for (const auto& name: commonItems::getStrings(theStream))
			maleNames.emplace(name);
	});
	registerKeyword("female_names", [this](std::istream& theStream) {
		for (const auto& name: commonItems::getStrings(theStream))
			femaleNames.emplace(name);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
