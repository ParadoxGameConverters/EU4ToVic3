#include "CultureParser.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::CultureParser::CultureParser(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::CultureParser::registerKeys()
{
	registerKeyword("primary", [this](std::istream& theStream) {
		primaryTag = commonItems::getString(theStream);
	});
	registerKeyword("male_names", [this](std::istream& theStream) {
		maleNames = commonItems::getStrings(theStream);
	});
	registerKeyword("female_names", [this](std::istream& theStream) {
		femaleNames = commonItems::getStrings(theStream);
	});
	registerKeyword("dynasty_names", [this](std::istream& theStream) {
		dynastyNames = commonItems::getStrings(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
