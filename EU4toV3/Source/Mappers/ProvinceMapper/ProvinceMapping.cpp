#include "ProvinceMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::ProvinceMapping::ProvinceMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ProvinceMapping::registerKeys()
{
	registerKeyword("eu4", [this](std::istream& theStream) {
		eu4Provinces.push_back(commonItems::getInt(theStream));
	});
	registerKeyword("vic3", [this](std::istream& theStream) {
		v3Provinces.push_back(commonItems::getString(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
