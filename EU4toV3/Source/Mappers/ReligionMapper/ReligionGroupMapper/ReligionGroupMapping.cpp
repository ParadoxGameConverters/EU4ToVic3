#include "ReligionGroupMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::ReligionGroupMapping::ReligionGroupMapping(std::istream& theStream)
{
	registerkeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ReligionGroupMapping::registerkeys()
{
	registerKeyword("vic3", [this](std::istream& theStream) {
		trait = commonItems::getString(theStream);
	});
	registerKeyword("eu4", [this](std::istream& theStream) {
		eu4groups.emplace(commonItems::getString(theStream));
	});
	registerKeyword("taboo", [this](std::istream& theStream) {
		taboos.emplace(commonItems::getString(theStream));
	});
	registerKeyword("icon", [this](std::istream& theStream) {
		icon = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
