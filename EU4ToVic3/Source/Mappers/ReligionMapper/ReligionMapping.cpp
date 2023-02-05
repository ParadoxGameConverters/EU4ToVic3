#include "ReligionMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::ReligionMapping::ReligionMapping(std::istream& theStream)
{
	registerkeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ReligionMapping::registerkeys()
{
	registerKeyword("vic3", [this](std::istream& theStream) {
		v3Religion = commonItems::getString(theStream);
	});
	registerKeyword("eu4", [this](std::istream& theStream) {
		eu4Religions.emplace(commonItems::getString(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
