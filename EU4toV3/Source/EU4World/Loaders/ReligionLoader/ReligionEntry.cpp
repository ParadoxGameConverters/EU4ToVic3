#include "ReligionEntry.h"
#include "Color.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::ReligionEntry::ReligionEntry(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::ReligionEntry::registerKeys()
{
	registerKeyword("color", [this](std::istream& theStream) {
		color = commonItems::Color::Factory().getColor(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
