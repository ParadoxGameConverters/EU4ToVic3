#include "PartyName.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::PartyName::PartyName(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::PartyName::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& language, std::istream& theStream) {
		languageToNameMap.insert(std::make_pair(language, commonItems::singleString(theStream).getString()));
	});
}
