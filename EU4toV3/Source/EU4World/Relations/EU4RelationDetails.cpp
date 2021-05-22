#include "EU4RelationDetails.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::EU4RelationDetails::EU4RelationDetails(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::EU4RelationDetails::registerKeys()
{
	registerRegex("value|cached_sum", [this](const std::string& unused, std::istream& theStream) {
		value = commonItems::getInt(theStream);
	});
	registerKeyword("military_access", [this](std::istream& theStream) {
		commonItems::ignoreItem("unused", theStream);
		military_access = true;
	});
	registerKeyword("attitude", [this](std::istream& theStream) {
		attitude = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
