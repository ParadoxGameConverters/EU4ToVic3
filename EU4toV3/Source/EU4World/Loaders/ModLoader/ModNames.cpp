#include "ModNames.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::ModNames::ModNames(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::ModNames::registerKeys()
{
	registerSetter("filename", path);
	registerSetter("name", name);
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
