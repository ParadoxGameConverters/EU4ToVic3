#include "TechGroups.h"
#include "CommonRegexes.h"
#include "Configuration.h"
#include "ParserHelpers.h"

mappers::TechGroups::TechGroups(std::istream& theStream)
{
	registerKeyword("westernization", [this](const std::string& unused, std::istream& theStream) {
		westernization = commonItems::getInt(theStream);
	});
	registerKeyword("literacy", [this](const std::string& unused, std::istream& theStream) {
		literacyBoost = commonItems::getInt(theStream);
	});
	registerKeyword("cultures", [this](const std::string& unused, std::istream& theStream) {
		const auto& cultureList = commonItems::getStrings(theStream);
		cultures.insert(cultureList.begin(), cultureList.end());
	});
	registerKeyword("hpm_cultures", [this](const std::string& unused, std::istream& theStream) {
		const auto& cultureList = commonItems::getStrings(theStream);
		hpmCultures.insert(cultureList.begin(), cultureList.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	parseStream(theStream);
	clearRegisteredKeywords();

	if (theConfiguration.isHpmEnabled() && !hpmCultures.empty())
		cultures = hpmCultures;
}