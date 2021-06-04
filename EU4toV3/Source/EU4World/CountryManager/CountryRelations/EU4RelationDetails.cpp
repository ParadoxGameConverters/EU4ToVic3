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
	registerSetter("attitude", attitude);
	registerSetter("cached_sum", value);
	registerKeyword("military_access", [this](std::istream& theStream) {
		commonItems::ignoreItem("unused", theStream);
		military_access = true;
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
