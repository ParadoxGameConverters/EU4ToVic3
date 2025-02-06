#include "ProductionMethodEntry.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::ProductionMethodEntry::ProductionMethodEntry(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ProductionMethodEntry::registerKeys()
{
	registerKeyword("name", [this](std::istream& theStream) {
		rule.pm = commonItems::getString(theStream);
	});
	registerKeyword("percent", [this](std::istream& theStream) {
		rule.percent = commonItems::getDouble(theStream);
	});
	registerKeyword("law_bound", [this](std::istream& theStream) {
		rule.lawBound = commonItems::getString(theStream) == "yes";
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
