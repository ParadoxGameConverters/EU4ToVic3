#include "LawGroup.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::LawGroup::LawGroup(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::LawGroup::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& lawName, std::istream& theStream) {
		laws.emplace(lawName, LawMapping(theStream));
	});
}
