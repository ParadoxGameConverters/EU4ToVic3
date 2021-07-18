#include "EU4CustomColors.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::CustomColors::CustomColors(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::CustomColors::registerKeys()
{
	registerKeyword("flag", [this](std::istream& theStream) {
		const commonItems::singleInt flagInt(theStream);
		// Why, paradox, why?
		if (flagInt.getInt() < 0)
			customColors.flagIndex = 1;
		else
			customColors.flagIndex = flagInt.getInt() + 1;
	});
	registerKeyword("color", [this](std::istream& theStream) {
		customColors.colorIndex = commonItems::getInt(theStream) + 1;
	});
	registerKeyword("symbol_index", [this](std::istream& theStream) {
		customColors.symbolIndex = commonItems::getInt(theStream) + 1;
	});
	registerKeyword("flag_colors", [this](std::istream& theStream) {
		customColors.flagColors = commonItems::Color::Factory{}.getColor(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
