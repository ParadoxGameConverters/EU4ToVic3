#include "EU4NationalSymbol.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::NationalSymbol::NationalSymbol(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::NationalSymbol::registerKeys()
{
	registerKeyword("map_color", [this](std::istream& theStream) {
		mapColor = commonItems::Color::Factory{}.getColor(theStream);
	});
	registerKeyword("revolutionary_colors", [this](std::istream& theStream) {
		revolutionaryColor = commonItems::Color::Factory{}.getColor(theStream);
	});
	registerKeyword("custom_colors", [this](std::istream& theStream) {
		customColors = CustomColors(theStream).getCustomColorsBlock();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
