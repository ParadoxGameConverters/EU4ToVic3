#include "FactoryTechDetails.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::FactoryTechDetails::FactoryTechDetails(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::FactoryTechDetails::registerKeys()
{
	registerKeyword("activate_building", [this](const std::string& unused, std::istream& theStream) {
		factoryName = commonItems::singleString(theStream).getString();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
