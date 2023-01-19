#include "VanillaStateEntry.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

V3::VanillaStateEntry::VanillaStateEntry(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::VanillaStateEntry::registerKeys()
{
	registerKeyword("create_state", [this](std::istream& theStream) {
		subStateEntries.emplace_back(VanillaSubStateEntry(theStream));
	});
	registerKeyword("add_homeland", [this](std::istream& theStream) {
		homelands.emplace(commonItems::getString(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
