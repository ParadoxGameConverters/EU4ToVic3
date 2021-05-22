#include "EU4LeaderID.h"
#include "Log.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::LeaderID::LeaderID(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::LeaderID::registerKeys()
{
	registerSetter("id", IDNum);
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
