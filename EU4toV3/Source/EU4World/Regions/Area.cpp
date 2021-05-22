#include "Area.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::Area::Area(std::istream& theStream)
{
	registerKeywords();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::Area::registerKeywords()
{
	registerKeyword("color", commonItems::ignoreItem);
	registerRegex(commonItems::integerRegex, [this](const std::string& number, std::istream& theStream) {
		// This is a peculiar file format where we pull free-floating numbers from thin air
		provinces.insert(std::stoi(number));
	});
}
