#include "EU4CountryModifier.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::EU4CountryModifier::EU4CountryModifier(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::EU4CountryModifier::registerKeys()
{
	registerSetter("modifier", modifier);
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
