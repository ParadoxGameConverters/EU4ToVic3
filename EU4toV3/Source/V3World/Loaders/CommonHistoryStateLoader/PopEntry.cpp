#include "PopEntry.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

void V3::PopEntry::loadPop(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::PopEntry::registerKeys()
{
	registerKeyword("culture", [this](std::istream& theStream) {
		pop.setCulture(commonItems::getString(theStream));
	});
	registerKeyword("religion", [this](std::istream& theStream) {
		pop.setReligion(commonItems::getString(theStream));
	});
	registerKeyword("size", [this](std::istream& theStream) {
		pop.setSize(commonItems::getInt(theStream));
	});
	registerKeyword("pop_type", [this](std::istream& theStream) {
		pop.setType(commonItems::getString(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
