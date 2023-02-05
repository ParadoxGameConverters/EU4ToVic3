#include "EmpireParser.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::EmpireParser::EmpireParser(std::istream& theStream)
{
	registerKeywords();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::EmpireParser::registerKeywords()
{
	registerKeyword("emperor", [this](std::istream& theStream) {
		emperor = commonItems::getString(theStream);
	});
	registerKeyword("passed_reform", [this](std::istream& theStream) {
		reforms.insert(commonItems::getString(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
