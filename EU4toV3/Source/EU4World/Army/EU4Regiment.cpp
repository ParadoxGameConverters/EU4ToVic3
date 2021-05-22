#include "EU4Regiment.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::EU4Regiment::EU4Regiment(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::EU4Regiment::registerKeys()
{
	registerKeyword("name", [this](const std::string& unused, std::istream& theStream) {
		name = commonItems::singleString(theStream).getString();
	});
	registerKeyword("type", [this](const std::string& unused, std::istream& theStream) {
		regimentType = commonItems::singleString(theStream).getString();
	});
	registerKeyword("home", [this](const std::string& unused, std::istream& theStream) {
		home = commonItems::singleInt(theStream).getInt();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
