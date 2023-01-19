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
		vic3Buildings = commonItems::getStrings(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}