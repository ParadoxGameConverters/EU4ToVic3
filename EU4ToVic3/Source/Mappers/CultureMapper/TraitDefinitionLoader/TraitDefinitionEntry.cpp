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
	registerKeyword("heritage", [this](std::istream& theStream) {
		traitDef.isHeritageTrait = commonItems::getString(theStream) == "yes";
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
