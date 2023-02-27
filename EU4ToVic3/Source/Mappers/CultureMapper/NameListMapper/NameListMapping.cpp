#include "NameListMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::NameListMapping::NameListMapping(std::istream& theStream)
{
	registerkeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::NameListMapping::registerkeys()
{
	registerKeyword("eu4", [this](std::istream& theStream) {
		culture = commonItems::getString(theStream);
	});
	registerKeyword("eu4group", [this](std::istream& theStream) {
		cultureGroup = commonItems::getString(theStream);
	});
	registerKeyword("name_pool", [this](std::istream& theStream) {
		namePool = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
