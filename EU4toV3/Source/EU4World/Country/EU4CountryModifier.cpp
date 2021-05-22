#include "EU4CountryModifier.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::EU4CountryModifier::EU4CountryModifier(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::EU4CountryModifier::registerKeys()
{
	registerKeyword("modifier", [this](const std::string& unused, std::istream& theStream) {
		modifier = commonItems::singleString(theStream).getString();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
