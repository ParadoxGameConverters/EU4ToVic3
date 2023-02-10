#include "AIStrategyMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::AIStrategyMapping::AIStrategyMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::AIStrategyMapping::registerKeys()
{
	registerKeyword("laws", [this](std::istream& theStream) {
		lawsParser.parseStream(theStream);
	});
	registerKeyword("capital", [this](std::istream& theStream) {
		capitalParser.parseStream(theStream);
	});
	registerKeyword("tags", [this](std::istream& theStream) {
		auto theTags = commonItems::getStrings(theStream);
		tags.insert(theTags.begin(), theTags.end());
	});
	registerKeyword("gp", [this](std::istream& theStream) {
		gp = commonItems::getInt(theStream);
	});
	registerKeyword("not_gp", [this](std::istream& theStream) {
		notGP = commonItems::getInt(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	lawsParser.registerRegex(commonItems::catchallRegex, [this](const std::string& lawName, std::istream& theStream) {
		laws.emplace(lawName, commonItems::getInt(theStream));
	});

	capitalParser.registerRegex(commonItems::catchallRegex, [this](const std::string& regionName, std::istream& theStream) {
		capitalRegions.emplace(regionName, commonItems::getInt(theStream));
	});
}
