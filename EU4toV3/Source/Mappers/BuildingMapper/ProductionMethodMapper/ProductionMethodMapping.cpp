#include "ProductionMethodMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"
#include "ProductionMethodEntry.h"

void mappers::ProductionMethodMapping::loadRules(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ProductionMethodMapping::registerKeys()
{
	registerKeyword("pm", [this](std::istream& theStream) {
		rules.push_back(ProductionMethodEntry(theStream).getRule());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
