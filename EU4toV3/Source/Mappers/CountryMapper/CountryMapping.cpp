#include "CountryMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::CountryMapping::CountryMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::CountryMapping::registerKeys()
{
	registerKeyword("eu4", [this](std::istream& theStream) {
		eu4Tag = commonItems::getString(theStream);
	});
	registerKeyword("vic3", [this](std::istream& theStream) {
		v3Tag = commonItems::getString(theStream);
	});
	registerKeyword("name", [this](std::istream& theStream) {
		name = commonItems::getString(theStream);
	});
	registerKeyword("flag_code", [this](std::istream& theStream) {
		flagCode = commonItems::getString(theStream);
	});
	registerKeyword("reform", [this](std::istream& theStream) {
		reforms.insert(commonItems::getString(theStream));
	});
	registerKeyword("flag", [this](std::istream& theStream) {
		flags.insert(commonItems::getString(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
