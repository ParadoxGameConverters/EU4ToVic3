#include "PopulationSetupMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::PopulationSetupMapping::PopulationSetupMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::PopulationSetupMapping::registerKeys()
{
	registerKeyword("literacy", [this](std::istream& theStream) {
		literacy = commonItems::getDouble(theStream);
	});
	registerKeyword("averagedev", [this](std::istream& theStream) {
		averageDev = commonItems::getDouble(theStream);
	});
	registerKeyword("effect", [this](std::istream& theStream) {
		effect = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
