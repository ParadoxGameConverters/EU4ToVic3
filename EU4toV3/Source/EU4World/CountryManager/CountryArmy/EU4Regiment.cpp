#include "EU4Regiment.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::EU4Regiment::EU4Regiment(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::EU4Regiment::registerKeys()
{
	registerSetter("name", name);
	registerSetter("type", unitType.unitType);
	registerSetter("home", home);
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
