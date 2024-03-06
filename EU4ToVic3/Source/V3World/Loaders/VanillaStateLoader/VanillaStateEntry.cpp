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
		auto homelandString = commonItems::getString(theStream);
		if (homelandString.starts_with("cu:"))
			homelands.emplace(homelandString.substr(3, homelandString.length()));
		else
			homelands.emplace(homelandString);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
