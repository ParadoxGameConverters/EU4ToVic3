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
	registerKeyword("removetrait", [this](std::istream& theStream) {
		removeTraits.emplace(commonItems::getString(theStream));
	});
	registerKeyword("addtrait", [this](std::istream& theStream) {
		addTraits.emplace(commonItems::getString(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
