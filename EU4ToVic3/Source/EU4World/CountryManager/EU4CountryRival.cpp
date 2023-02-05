#include "EU4CountryRival.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::EU4CountryRival::EU4CountryRival(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::EU4CountryRival::registerKeys()
{
	registerSetter("country", country);
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
