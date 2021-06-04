#include "EU4CountryFlags.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::EU4CountryFlags::EU4CountryFlags(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::EU4CountryFlags::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& flag, std::istream& theStream) {
		commonItems::ignoreItem(flag, theStream);
		flags.insert(flag);
	});
}
