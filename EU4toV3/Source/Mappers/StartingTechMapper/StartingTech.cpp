#include "StartingTech.h"
#include "CommonRegexes.h"
#include "Configuration.h"
#include "ParserHelpers.h"

mappers::StartingTech::StartingTech(std::istream& theStream)
{
	registerKeyword("group", [this](const std::string& unused, std::istream& theStream) {
		group = commonItems::getString(theStream);
	});
	registerKeyword("name", [this](const std::string& unused, std::istream& theStream) {
		name = commonItems::getString(theStream);
	});
	registerKeyword("hpm", [this](const std::string& unused, std::istream& theStream) {
		hpm = commonItems::getString(theStream);
	});
	registerKeyword("score", [this](const std::string& unused, std::istream& theStream) {
		score = commonItems::getDouble(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	parseStream(theStream);
	clearRegisteredKeywords();

	if (theConfiguration.isHpmEnabled() && !hpm.empty())
	{
		name = hpm;
	}
}