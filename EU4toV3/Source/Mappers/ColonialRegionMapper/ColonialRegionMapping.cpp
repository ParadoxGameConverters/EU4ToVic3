#include "ColonialRegionMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::ColonialRegionMapping::ColonialRegionMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ColonialRegionMapping::registerKeys()
{
	registerKeyword("region", [this](std::istream& theStream) {
		regions.emplace(commonItems::getString(theStream));
	});
	registerKeyword("alone_name", [this](std::istream& theStream) {
		aloneName = commonItems::getString(theStream);
	});
	registerKeyword("split_name", [this](std::istream& theStream) {
		splitName = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
