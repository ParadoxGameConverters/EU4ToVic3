#include "EU4Empire.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::EU4Empire::EU4Empire(std::istream& theStream)
{
	registerKeywords();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::EU4Empire::registerKeywords()
{
	registerKeyword("emperor", [this](std::istream& theStream) {
		emperor = commonItems::getString(theStream);
	});
	registerKeyword("passed_reform", [this](std::istream& theStream) {
		reforms.insert(commonItems::getString(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
