#include "EU4ActivePolicy.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::EU4ActivePolicy::EU4ActivePolicy(std::istream& theStream)
{
	registerKeyword("policy", [this](const std::string& unused, std::istream& theStream) {
		policy = commonItems::singleString(theStream).getString();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	parseStream(theStream);
	clearRegisteredKeywords();
}