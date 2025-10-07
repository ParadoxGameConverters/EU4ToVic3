#include "CultureTraitMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::CultureTraitMapping::CultureTraitMapping(std::istream& theStream)
{
	registerkeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::CultureTraitMapping::registerkeys()
{
	registerKeyword("eu4", [this](std::istream& theStream) {
		culture = commonItems::getString(theStream);
	});
	registerKeyword("eu4group", [this](std::istream& theStream) {
		cultureGroup = commonItems::getString(theStream);
	});
	registerKeyword("tradition", [this](std::istream& theStream) {
		traditions.emplace(commonItems::getString(theStream));
	});
	registerKeyword("language", [this](std::istream& theStream) {
		language = commonItems::getString(theStream);
	});
	registerKeyword("heritage", [this](std::istream& theStream) {
		heritage = commonItems::getString(theStream);
	});
	registerKeyword("ethnicity", [this](std::istream& theStream) {
		ethnicity = commonItems::getString(theStream);
	});
	registerKeyword("graphics", [this](std::istream& theStream) {
		graphics = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
