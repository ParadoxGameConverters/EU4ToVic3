#include "EU4NationalSymbol.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::NationalSymbol::NationalSymbol(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::NationalSymbol::registerKeys()
{
	registerKeyword("map_color", [this](const std::string& unused, std::istream& theStream) {
		mapColor = commonItems::Color::Factory{}.getColor(theStream);
	});
	registerKeyword("revolutionary_colors", [this](const std::string& unused, std::istream& theStream) {
		revolutionaryColor = commonItems::Color::Factory{}.getColor(theStream);
	});
	registerKeyword("custom_colors", [this](const std::string& colorsString, std::istream& theStream) {
		customColors = std::make_shared<CustomColors>(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
