#include "NewEU4CultureMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::NewEU4CultureMapping::NewEU4CultureMapping(std::istream& theStream)
{
	registerkeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::NewEU4CultureMapping::registerkeys()
{
	registerKeyword("culture", [this](std::istream& theStream) {
		cultures.emplace(commonItems::getString(theStream));
	});
	registerKeyword("removetradition", [this](std::istream& theStream) {
		removeTraditions.emplace(commonItems::getString(theStream));
	});
	registerKeyword("addtradition", [this](std::istream& theStream) {
		addTraditions.emplace(commonItems::getString(theStream));
	});
	registerKeyword("replacelanguage", [this](std::istream& theStream) {
		replaceLanguage = commonItems::getString(theStream);
	});
	registerKeyword("replaceheritage", [this](std::istream& theStream) {
		replaceHeritage = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
