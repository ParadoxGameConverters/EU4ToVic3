#include "ColonialRegion.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::ColonialRegion::ColonialRegion(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::ColonialRegion::registerKeys()
{
	registerKeyword("provinces", [this](std::istream& theStream) {
		provinces = commonItems::getInts(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
