#include "Country.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void V3::Country::initializeCountry(std::istream& theStream)
{
	registerKeys();
	vanillaData = VanillaCommonCountryData();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::Country::registerKeys()
{
	registerKeyword("country_type", [this](std::istream& theStream) {
		vanillaData->type = commonItems::getString(theStream);
	});
	registerKeyword("tier", [this](std::istream& theStream) {
		vanillaData->tier = commonItems::getString(theStream);
	});
	registerKeyword("cultures", [this](std::istream& theStream) {
		for (const auto& culture: commonItems::getStrings(theStream))
			vanillaData->cultures.insert(culture);
	});
	registerKeyword("religion", [this](std::istream& theStream) {
		vanillaData->religion = commonItems::getString(theStream);
	});
	registerKeyword("capital", [this](std::istream& theStream) {
		vanillaData->capitalStateName = commonItems::getString(theStream);
	});
	registerKeyword("color", [this](std::istream& theStream) {
		vanillaData->color = commonItems::Color::Factory().getColor(theStream);
	});
	registerKeyword("is_named_from_capital", [this](const std::string& unused, std::istream& theStream) {
		commonItems::ignoreItem(unused, theStream);
		vanillaData->is_named_from_capital = true;
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void V3::Country::initializeFromEU4Country()
{
	Log(LogLevel::Debug) << "--- Importing " << sourceCountry->getTag() << " into " << tag;

	// TODO: TESTS IF POSSIBLE
	// color
	if (vanillaData && vanillaData->color)
	{
		processedData.color = vanillaData->color;
	}
	else
	{
		processedData.color = sourceCountry->getNationalColors().getMapColor();
	}

	// country type
	processedData.type = "recognized";

	// tier
	processedData.tier = "kingdom";

	// cultures
	processedData.cultures = {"basque"};

	// religion
	processedData.religion = "catholic";

	// capital
	if (!substates.empty())
		processedData.capitalStateName = substates[0]->stateName;
	else
		processedData.capitalStateName = "STATE_NAVARRA";

	// namedaftercapital
	processedData.is_named_from_capital = false;

	// eu4 locs
	processedData.namesByLanguage.emplace("english", sourceCountry->getName("english"));
	processedData.namesByLanguage.emplace("spanish", sourceCountry->getName("spanish"));
	processedData.namesByLanguage.emplace("french", sourceCountry->getName("french"));
	processedData.namesByLanguage.emplace("german", sourceCountry->getName("german"));
	processedData.adjectivesByLanguage.emplace("english", sourceCountry->getAdjective("english"));
	processedData.adjectivesByLanguage.emplace("spanish", sourceCountry->getAdjective("spanish"));
	processedData.adjectivesByLanguage.emplace("french", sourceCountry->getAdjective("french"));
	processedData.adjectivesByLanguage.emplace("german", sourceCountry->getAdjective("german"));
}

void V3::Country::copyVanillaData()
{
	// TODO: TESTS
	if (!vanillaData)
		return;

	processedData.color = vanillaData->color;
	processedData.type = vanillaData->type;
	processedData.tier = vanillaData->tier;
	processedData.cultures = vanillaData->cultures;
	processedData.religion = vanillaData->religion;
	processedData.capitalStateName = vanillaData->capitalStateName;
	processedData.is_named_from_capital = vanillaData->is_named_from_capital;
}

std::string V3::Country::getName(const std::string& language) const
{
	// TODO: TESTS
	if (processedData.namesByLanguage.contains(language))
		return processedData.namesByLanguage.at(language);

	// if we're lacking a specific language, try with english.
	if (processedData.namesByLanguage.contains("english"))
		return processedData.namesByLanguage.at("english");

	// otherwise, eh.
	return {};
}

std::string V3::Country::getAdjective(const std::string& language) const
{
	// TODO: TESTS
	if (processedData.adjectivesByLanguage.contains(language))
		return processedData.adjectivesByLanguage.at(language);

	if (processedData.adjectivesByLanguage.contains("english"))
		return processedData.adjectivesByLanguage.at("english");

	return {};
}
