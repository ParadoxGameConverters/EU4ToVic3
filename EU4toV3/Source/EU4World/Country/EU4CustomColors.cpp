#include "EU4CustomColors.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::CustomColors::CustomColors(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::CustomColors::registerKeys()
{
	registerKeyword("flag", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt flagInt(theStream);
		// Why, paradox, why?
		if (flagInt.getInt() < 0)
			customColors.flag = 1;
		else
			customColors.flag = flagInt.getInt() + 1;
	});
	registerKeyword("color", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt colorInt(theStream);
		customColors.color = colorInt.getInt() + 1;
	});
	registerKeyword("symbol_index", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt siInt(theStream);
		customColors.symbolIndex = siInt.getInt() + 1;
	});
	registerKeyword("flag_colors", [this](const std::string& colorsString, std::istream& theStream) {
		customColors.flagColors = commonItems::Color::Factory{}.getColor(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
