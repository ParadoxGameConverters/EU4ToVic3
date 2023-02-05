#include "RevolutionParser.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::RevolutionParser::RevolutionParser(std::istream& theStream)
{
	registerKeywords();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::RevolutionParser::registerKeywords()
{
	registerKeyword("revolution_target", [this](std::istream& theStream) {
		revolutionTarget = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
