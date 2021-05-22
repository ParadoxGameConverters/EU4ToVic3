#include "Religion.h"
#include "CommonRegexes.h"
#include "Configuration.h"
#include "ParserHelpers.h"

mappers::Religion::Religion(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();

	if (theConfiguration.isHpmEnabled() && !hpmColorString.empty())
		colorString = hpmColorString;
}

void mappers::Religion::registerKeys()
{
	registerKeyword("hpm", [this](const std::string& unused, std::istream& theStream) {
		hpm = commonItems::getString(theStream);
	});
	registerKeyword("icon", [this](const std::string& unused, std::istream& theStream) {
		icon = commonItems::getInt(theStream);
	});
	registerKeyword("color", [this](const std::string& unused, std::istream& theStream) {
		colorString = commonItems::stringOfItem(theStream).getString();
	});
	registerKeyword("hpm_color", [this](const std::string& unused, std::istream& theStream) {
		hpmColorString = commonItems::stringOfItem(theStream).getString();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}