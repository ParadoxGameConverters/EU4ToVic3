#include "ReligionDefinitionEntry.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::ReligionDefinitionEntry::ReligionDefinitionEntry(std::istream& theStream)
{
	registerkeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ReligionDefinitionEntry::registerkeys()
{
	registerKeyword("icon", [this](std::istream& theStream) {
		religionDef.texture = commonItems::getString(theStream);
	});
	registerKeyword("heritage", [this](std::istream& theStream) {
		religionDef.heritage = commonItems::getString(theStream);
	});
	registerKeyword("color", [this](std::istream& theStream) {
		religionDef.color = commonItems::Color::Factory().getColor(theStream);
	});
	registerKeyword("taboos", [this](std::istream& theStream) {
		for (const auto& taboo: commonItems::getStrings(theStream))
			religionDef.taboos.emplace(taboo);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
