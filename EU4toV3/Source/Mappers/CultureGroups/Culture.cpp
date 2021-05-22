#include "Culture.h"
#include "CommonRegexes.h"
#include "Configuration.h"
#include "ParserHelpers.h"
#include <random>

mappers::Culture::Culture(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();

	if (theConfiguration.isHpmEnabled() && !hpmLastNames.empty())
		lastNames = hpmLastNames;
}

void mappers::Culture::registerKeys()
{
	registerKeyword("hpm", [this](const std::string& unused, std::istream& theStream) {
		hpm = commonItems::singleString(theStream).getString();
	});
	registerKeyword("primary", [this](const std::string& unused, std::istream& theStream) {
		primaryTag = commonItems::singleString(theStream).getString();
	});
	registerKeyword("male_names", [this](const std::string& unused, std::istream& theStream) {
		maleNames = commonItems::stringList(theStream).getStrings();
	});
	registerKeyword("female_names", [this](const std::string& unused, std::istream& theStream) {
		femaleNames = commonItems::stringList(theStream).getStrings();
	});
	registerKeyword("first_names", [this](const std::string& unused, std::istream& theStream) {
		firstNames = commonItems::stringList(theStream).getStrings();
	});
	registerKeyword("last_names", [this](const std::string& unused, std::istream& theStream) {
		lastNames = commonItems::stringList(theStream).getStrings();
	});
	registerKeyword("hpm_last_names", [this](const std::string& unused, std::istream& theStream) {
		hpmLastNames = commonItems::stringList(theStream).getStrings();
	});
	registerKeyword("dynasty_names", [this](const std::string& unused, std::istream& theStream) {
		dynastyNames = commonItems::stringList(theStream).getStrings();
	});
	registerKeyword("color", [this](const std::string& unused, std::istream& theStream) {
		color = commonItems::Color::Factory{}.getColor(theStream);
	});
	registerKeyword("radicalism", [this](const std::string& unused, std::istream& theStream) {
		radicalism = commonItems::singleInt(theStream).getInt();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void mappers::Culture::transmogrify()
{
	firstNames = maleNames;
	lastNames = dynastyNames;

	std::random_device theRandomDevice;
	std::mt19937 theEngine(theRandomDevice());
	std::uniform_int_distribution<> theDistribution(0, 255);

	const auto r = theDistribution(theEngine);
	const auto g = theDistribution(theEngine);
	const auto b = theDistribution(theEngine);
	color = commonItems::Color(std::array<int, 3>{r, g, b});
	primaryTag.clear();
}
