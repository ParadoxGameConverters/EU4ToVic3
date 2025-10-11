#include "TraitDefinitionEntry.h"
#include "CommonRegexes.h"

mappers::TraitDefinitionEntry::TraitDefinitionEntry(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::TraitDefinitionEntry::registerKeys()
{
	registerKeyword("tradition", [this](std::istream& theStream) {
		traitDef.isTradition = commonItems::getString(theStream) == "yes";
	});
	registerKeyword("heritage", [this](std::istream& theStream) {
		traitDef.isHeritage = commonItems::getString(theStream) == "yes";
	});
	registerKeyword("language", [this](std::istream& theStream) {
		traitDef.isLanguage = commonItems::getString(theStream) == "yes";
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
