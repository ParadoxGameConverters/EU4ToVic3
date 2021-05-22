#include "ColonialFlag.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::ColonialFlag::ColonialFlag(std::istream& theStream, const std::string& theRegion): region(theRegion)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ColonialFlag::registerKeys()
{
	registerKeyword("name", [this](const std::string& unused, std::istream& theStream) {
		name = commonItems::singleString(theStream).getString();
		std::transform(name.begin(), name.end(), name.begin(), ::tolower);
	});
	registerKeyword("unique", [this](const std::string& unused, std::istream& theStream) {
		commonItems::ignoreItem(unused, theStream);
		unique = true;
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

