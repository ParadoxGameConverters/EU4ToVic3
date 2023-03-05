#include "EU4ActivePolicy.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::EU4ActivePolicy::EU4ActivePolicy(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::EU4ActivePolicy::registerKeys()
{
	registerSetter("policy", policy);
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
