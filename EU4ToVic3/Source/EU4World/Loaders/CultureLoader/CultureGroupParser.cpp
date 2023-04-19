#include "CultureGroupParser.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"
#include <ranges>

EU4::CultureGroupParser::CultureGroupParser(std::string theName, std::istream& theStream, std::set<std::string> theKnownCultures):
	 cultureGroupName(std::move(theName)), knownCultures(std::move(theKnownCultures))
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();

	if (!maleNames.empty())
		for (auto& culture: cultures | std::views::values)
			culture.addMaleNames(maleNames);

	if (!femaleNames.empty())
		for (auto& culture: cultures | std::views::values)
			culture.addFemaleNames(femaleNames);

	if (!dynastyNames.empty())
		for (auto& culture: cultures | std::views::values)
			culture.addDynastyNames(dynastyNames);
}

void EU4::CultureGroupParser::registerKeys()
{
	registerKeyword("country", commonItems::ignoreItem);
	registerKeyword("province", commonItems::ignoreItem);
	registerRegex("second_graphical_culture|graphical_culture", [this](const std::string& unused, std::istream& theStream) {
		commonItems::ignoreItem(unused, theStream);
	});
	registerKeyword("male_names", [this](std::istream& theStream) {
		maleNames = commonItems::getStrings(theStream);
	});
	registerKeyword("female_names", [this](std::istream& theStream) {
		femaleNames = commonItems::getStrings(theStream);
	});
	registerKeyword("dynasty_names", [this](std::istream& theStream) {
		dynastyNames = commonItems::getStrings(theStream);
	});
	registerRegex(commonItems::stringRegex, [this](const std::string& cultureName, std::istream& theStream) {
		auto newCulture = CultureParser(theStream);
		if (!knownCultures.contains(cultureName))
			cultures.emplace(cultureName, newCulture);
	});
}

void EU4::CultureGroupParser::mergeCulture(const std::string& cultureName, const CultureParser& cultureParser)
{
	const auto& cultureItr = cultures.find(cultureName);
	if (cultureItr != cultures.end())
	{
		cultureItr->second.addMaleNames(cultureParser.getMaleNames());
		cultureItr->second.addFemaleNames(cultureParser.getFemaleNames());
		cultureItr->second.addDynastyNames(cultureParser.getDynastyNames());
		if (!cultureParser.getPrimaryTag().empty())
			cultureItr->second.setPrimaryTag(cultureParser.getPrimaryTag()); // overwriting with modded.
	}
	else
	{
		cultures.emplace(cultureName, cultureParser);
	}
}

std::set<std::string> EU4::CultureGroupParser::getCultureNames() const
{
	std::set<std::string> toReturn;
	for (const auto& cultureName: cultures | std::views::keys)
		toReturn.emplace(cultureName);
	return toReturn;
}
