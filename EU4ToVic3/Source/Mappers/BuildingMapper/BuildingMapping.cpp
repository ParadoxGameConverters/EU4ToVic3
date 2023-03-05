#include "BuildingMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::BuildingMapping::BuildingMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::BuildingMapping::registerKeys()
{
	registerKeyword("eu4", [this](std::istream& theStream) {
		eu4Building = commonItems::getString(theStream);
	});
	registerKeyword("vic3", [this](std::istream& theStream) {
		auto theVic3Buildings = commonItems::getStrings(theStream);
		vic3Buildings.insert(theVic3Buildings.begin(), theVic3Buildings.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}