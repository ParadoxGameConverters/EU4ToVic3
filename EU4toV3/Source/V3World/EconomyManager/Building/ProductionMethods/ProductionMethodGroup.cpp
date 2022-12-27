#include "ProductionMethodGroup.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

void V3::ProductionMethodGroup::loadProductionMethodGroup(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::ProductionMethodGroup::registerKeys()
{
	registerKeyword("production_methods", [this](std::istream& theStream) {
		PMs = commonItems::getStrings(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
