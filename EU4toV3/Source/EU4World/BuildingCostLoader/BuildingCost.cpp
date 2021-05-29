#include "BuildingCost.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::BuildingCost::BuildingCost(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::BuildingCost::registerKeys()
{
	registerSetter("cost", cost);
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
