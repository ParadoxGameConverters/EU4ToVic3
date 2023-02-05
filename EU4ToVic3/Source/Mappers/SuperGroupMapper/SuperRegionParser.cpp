#include "SuperRegionParser.h"
#include "AssimilationParser.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::SuperRegionParser::SuperRegionParser(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::SuperRegionParser::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& superRegionName, std::istream& theStream) {
		const AssimilationParser assimilationBlock(theStream);
		superRegionAssimilationMap.insert(std::make_pair(superRegionName, assimilationBlock.getAssimilation()));
		superRegionNames.insert(superRegionName);
	});
}
