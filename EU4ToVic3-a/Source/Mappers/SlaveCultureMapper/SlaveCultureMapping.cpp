#include "SlaveCultureMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::SlaveCultureMapping::SlaveCultureMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::SlaveCultureMapping::registerKeys()
{
	registerKeyword("master", [this](std::istream& theStream) {
		masterTrait = commonItems::getString(theStream);
	});
	registerKeyword("slave", [this](std::istream& theStream) {
		slaveCulture = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
