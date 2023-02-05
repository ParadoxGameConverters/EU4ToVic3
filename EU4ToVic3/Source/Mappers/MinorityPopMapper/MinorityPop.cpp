#include "MinorityPop.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::MinorityPop::MinorityPop(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::MinorityPop::registerKeys()
{
	registerKeyword("religion", [this](std::istream& theStream) {
		religion = commonItems::getString(theStream);
	});
	registerKeyword("culture", [this](std::istream& theStream) {
		culture = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
