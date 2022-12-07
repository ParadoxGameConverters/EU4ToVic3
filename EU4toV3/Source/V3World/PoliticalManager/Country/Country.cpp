#include "Country.h"
#include "ClayManager/State/SubState.h"
#include "CommonRegexes.h"
#include "CountryManager/EU4Country.h"
#include "Loaders/LocLoader/LocalizationLoader.h"
#include "Loaders/LocalizationLoader/EU4LocalizationLoader.h"
#include "Log.h"
#include "ParserHelpers.h"
#include <numeric>

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

void V3::Country::convertFromEU4Country(const ClayManager& clayManager)
{
	// color - using eu4 colors so people don't lose their shit over red venice and orange england.
	if (sourceCountry->getNationalColors().getMapColor())
		processedData.color = sourceCountry->getNationalColors().getMapColor();
	else
		processedData.color = vanillaData->color;

	// eu4 locs
	processedData.name = sourceCountry->getName();
	processedData.namesByLanguage = sourceCountry->getAllNameLocalizations();
	processedData.adjective = sourceCountry->getAdjective();
	processedData.adjectivesByLanguage = sourceCountry->getAllAdjectiveLocalizations();

	// TODO: UNTESTED - TESTS IF POSSIBLE WHEN CONVERSION DONE
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
		processedData.capitalStateName = substates[0]->getHomeStateName();
	else
		processedData.capitalStateName = "STATE_NAVARRA";
	// namedaftercapital
	processedData.is_named_from_capital = false;
}

void V3::Country::generateDecentralizedData(const ClayManager& clayManager,
	 const LocalizationLoader& v3LocLoader,
	 const EU4::EU4LocalizationLoader& eu4LocLoader)
{
	generateDecentralizedLocs(v3LocLoader, eu4LocLoader);
}

void V3::Country::generateDecentralizedLocs(const LocalizationLoader& v3LocLoader, const EU4::EU4LocalizationLoader& eu4LocLoader)
{
	// We're basing the name off culture. Trouble is, culture can be either eu4 or vic3.
	if (processedData.cultures.empty())
		return; // uh-huh.

	const auto& culture = *processedData.cultures.begin();

	const auto eu4Locs = eu4LocLoader.getTextForKey(culture, "english");
	const auto v3Locs = v3LocLoader.getLocForKey(culture, "english");

	if (eu4Locs)
		processedData.name = *eu4Locs;
	else if (v3Locs)
		processedData.name = *v3Locs;
	else
		processedData.name = culture;

	processedData.adjective = processedData.name;
	processedData.name[0] = static_cast<char>(std::toupper(processedData.name[0])); // capitalize first letter. This is why this has to be english.

	// Let's spruce it up.
	const std::vector<std::string> suffixes = {"Territory",
		 "Expanse",
		 "Plains",
		 "Void",
		 "Dominion",
		 "Hegemony",
		 "Tribe",
		 "Horde",
		 "Alliance",
		 "Federation",
		 "Union",
		 "Collective",
		 "Council",
		 "Nation",
		 "Imperium"};

	const auto seed = std::accumulate(culture.begin(), culture.end(), 0, [](int sum, const char& letter) {
		return sum + letter;
	});
	const int selection = seed % static_cast<int>(suffixes.size());
	processedData.name += " " + suffixes[selection];
}

void V3::Country::copyVanillaData(const LocalizationLoader& v3LocLoader, const EU4::EU4LocalizationLoader& eu4LocLoader)
{
	// this is done when conversion from eu4 source is impossible - likely because this country doesn't exist in eu4.
	if (!vanillaData)
		return;

	processedData.color = vanillaData->color;
	processedData.type = vanillaData->type;
	processedData.tier = vanillaData->tier;
	processedData.cultures = vanillaData->cultures;
	processedData.religion = vanillaData->religion;
	processedData.capitalStateName = vanillaData->capitalStateName;
	processedData.is_named_from_capital = vanillaData->is_named_from_capital;

	// do we have a name waiting for us?
	const auto& tagName = v3LocLoader.getLocMapForKey(tag);
	const auto& tagAdj = v3LocLoader.getLocMapForKey(tag + "_ADJ");
	if (tagName && tagAdj)
	{
		processedData.namesByLanguage = *tagName;
		processedData.adjectivesByLanguage = *tagAdj;
	}
	else
	{
		// check for eu4 locs, in case for modded decentralized inputs.
		const auto& eu4Name = eu4LocLoader.getTextInEachLanguage(tag);
		const auto& eu4Adj = eu4LocLoader.getTextInEachLanguage(tag + "_ADJ");
		if (eu4Name && eu4Adj)
		{
			processedData.namesByLanguage = *eu4Name;
			processedData.adjectivesByLanguage = *eu4Adj;
		}
	}
}

std::string V3::Country::getName(const std::string& language) const
{
	if (processedData.namesByLanguage.contains(language))
		return processedData.namesByLanguage.at(language);

	// if we're lacking a specific language, try with english.
	if (processedData.namesByLanguage.contains("english"))
		return processedData.namesByLanguage.at("english");

	// custom name?
	if (!processedData.name.empty())
		return processedData.name;

	// otherwise, eh.
	return tag;
}

std::string V3::Country::getAdjective(const std::string& language) const
{
	if (processedData.adjectivesByLanguage.contains(language))
		return processedData.adjectivesByLanguage.at(language);

	// if we're lacking a specific language, try with english.
	if (processedData.adjectivesByLanguage.contains("english"))
		return processedData.adjectivesByLanguage.at("english");

	// custom adj?
	if (!processedData.adjective.empty())
		return processedData.adjective;

	// wing it.
	return tag + "_ADJ";
}
