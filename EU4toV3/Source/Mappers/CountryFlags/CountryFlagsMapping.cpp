#include "CountryFlagsMapping.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::CountryFlagsMapping::CountryFlagsMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::CountryFlagsMapping::registerKeys()
{
	registerKeyword("eu4", [this](const std::string& unused, std::istream& theStream) {
		eu4Flag = commonItems::singleString(theStream).getString();
	});
	registerKeyword("v2", [this](const std::string& unused, std::istream& theStream) {
		v2Flags.insert(commonItems::singleString(theStream).getString());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}