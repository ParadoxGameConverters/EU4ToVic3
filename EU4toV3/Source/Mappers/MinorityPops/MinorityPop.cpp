#include "MinorityPop.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::MinorityPop::MinorityPop(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::MinorityPop::registerKeys()
{
	registerKeyword("religion", [this](const std::string& unused, std::istream& theStream) {
		religion = commonItems::singleString(theStream).getString();
	});
	registerKeyword("culture", [this](const std::string& unused, std::istream& theStream) {
		culture = commonItems::singleString(theStream).getString();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
