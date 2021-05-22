#include "FactoryInventionDetails.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::FactoryInventionDetails::FactoryInventionDetails(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::FactoryInventionDetails::registerKeys()
{
	registerRegex("activate_building", [this](const std::string& unused, std::istream& theStream) {
		factoryName = commonItems::singleString(theStream).getString();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
