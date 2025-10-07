#include "TraitDefinitionEntry.h"
#include "CommonRegexes.h"

mappers::TraitDefinitionEntry::TraitDefinitionEntry(std::istream& theStream)
{
	registerkeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::TraitDefinitionEntry::registerkeys()
{
	registerKeyword("tradition", [this](std::istream& theStream) {
		traitDef.isTradition = commonItems::getString(theStream) == "yes";
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
