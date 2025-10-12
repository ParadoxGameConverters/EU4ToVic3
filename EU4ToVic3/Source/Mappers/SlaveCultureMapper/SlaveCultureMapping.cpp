#include "SlaveCultureMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::SlaveCultureMapping::SlaveCultureMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::SlaveCultureMapping::registerKeys()
{
	registerKeyword("masterHeritage", [this](std::istream& theStream) {
		masterHeritage = commonItems::getString(theStream);
	});
	registerKeyword("masterLanguage", [this](std::istream& theStream) {
		masterLanguage = commonItems::getString(theStream);
	});
	registerKeyword("slave", [this](std::istream& theStream) {
		slaveCulture = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
