#include "Country.h"
#include "ClayManager/ClayManager.h"
#include "ClayManager/State/SubState.h"
#include "CommonRegexes.h"
#include "CountryManager/EU4Country.h"
#include "CultureMapper/CultureMapper.h"
#include "Loaders/LocLoader/LocalizationLoader.h"
#include "Loaders/LocalizationLoader/EU4LocalizationLoader.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "ReligionMapper/ReligionMapper.h"
#include <numeric>

namespace
{
commonItems::Color generateDecentralizedColors(const std::string& tag)
{
	if (tag.size() < 3)
		return commonItems::Color{std::array{15, 86, 112}}; // I just like these numbers.
	int r = static_cast<int>(tag[0]) * 17 % 256;				 // should be random enough.
	int g = static_cast<int>(tag[1]) * 43 % 256;
	int b = static_cast<int>(tag[2]) * 11 % 256;
	if (r < 50) // make them brighter.
		r += 50;
	if (g < 50)
		g += 50;
	if (b < 50)
		b += 50;
	return commonItems::Color{std::array{r, g, b}};
}
} // namespace

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
			vanillaData->cultures.emplace(culture);
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

void V3::Country::convertFromEU4Country(const ClayManager& clayManager,
	 mappers::CultureMapper& cultureMapper,
	 const mappers::ReligionMapper& religionMapper,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader)
{
	// color - using eu4 colors so people don't lose their shit over red venice and orange england.
	if (sourceCountry->getNationalColors().getMapColor())
	{
		processedData.color = sourceCountry->getNationalColors().getMapColor();
	}
	else
	{
		if (vanillaData)
			processedData.color = vanillaData->color;
	}

	// eu4 locs
	processedData.name = sourceCountry->getName();
	processedData.namesByLanguage = sourceCountry->getAllNameLocalizations();
	processedData.adjective = sourceCountry->getAdjective();
	processedData.adjectivesByLanguage = sourceCountry->getAllAdjectiveLocalizations();

	// TODO: UNTESTED - TESTS IF POSSIBLE WHEN CONVERSION DONE

	// Capital
	convertCapital(clayManager);

	// Religion
	convertReligion(religionMapper);

	// Culture
	convertCulture(clayManager, cultureMapper, cultureLoader, religionLoader);

	// tier
	convertTier();

	// country type
	processedData.type = "recognized";

	// namedaftercapital
	processedData.is_named_from_capital = false;
}

void V3::Country::convertTier()
{
	// TODO: Allow some dynamic configurable rules for this. VN for example has 6 incoming government ranks almost literally matching the Vic3 ones.

	if (sourceCountry->getGovernmentRank() == 1 && substates.size() == 1)
		processedData.tier = "city_state";
	else if (sourceCountry->getGovernmentRank() == 1)
		processedData.tier = "principality";
	else if (sourceCountry->getGovernmentRank() == 2 && substates.size() <= 3)
		processedData.tier = "grand_principality";
	else if (sourceCountry->getGovernmentRank() == 2)
		processedData.tier = "kingdom";
	else if (sourceCountry->getGovernmentRank() == 3 && substates.size() <= 20)
		processedData.tier = "empire";
	else
		processedData.tier = "hegemony";
}

void V3::Country::convertCulture(const ClayManager& clayManager,
	 mappers::CultureMapper& cultureMapper,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader)
{
	// Cultures can be tricky. We may have neocultures generated for our pops, meaning we need to account for being a neoculture country.
	// We also can easily have a primary culture with 0 actual pops in our country.
	// Again, as for Vic2, we're *ignoring* accepted cultures completely.

	if (sourceCountry->getPrimaryCulture().empty())
	{
		Log(LogLevel::Warning) << "EU4 " << sourceCountry->getTag() << " had no primary culture!";
		return;
	}

	// Do we have a capital?
	if (processedData.capitalStateName.empty())
	{
		// We already resurrected dead capitals... Default.
		// TODO: Try to figure out where's a majority of cores? Maybe?
		const auto& cultureMatch = cultureMapper.cultureMatch(clayManager,
			 cultureLoader,
			 religionLoader,
			 sourceCountry->getPrimaryCulture(),
			 sourceCountry->getReligion(),
			 "",
			 tag,
			 false,
			 false);
		if (cultureMatch)
			processedData.cultures.emplace(*cultureMatch);
		else
			Log(LogLevel::Warning) << "Could not set primary culture for " << tag << "!";
		return;
	}

	// We have a capital. This is good. Is by any chance the capital in some colonial region, where the eu4 primary culture is an invasive species?
	// We leave such questions to the cultureMapper.
	const auto& cultureMatch = cultureMapper.suspiciousCultureMatch(clayManager,
		 cultureLoader,
		 religionLoader,
		 sourceCountry->getPrimaryCulture(),
		 sourceCountry->getReligion(),
		 processedData.capitalStateName,
		 tag);
	if (cultureMatch)
		processedData.cultures.emplace(*cultureMatch);

	// if this fails... do nothing for now.
	if (processedData.cultures.empty())
		Log(LogLevel::Warning) << "Could not determine culture for country: " << tag;
}

void V3::Country::convertReligion(const mappers::ReligionMapper& religionMapper)
{
	if (sourceCountry->getReligion().empty())
	{
		Log(LogLevel::Warning) << "EU4 " << sourceCountry->getTag() << " had no religion!";
		return;
	}

	if (const auto& religionMatch = religionMapper.getV3Religion(sourceCountry->getReligion()); religionMatch)
		processedData.religion = *religionMatch;

	// if this fails... do nothing for now.
	if (processedData.religion.empty())
		Log(LogLevel::Warning) << "Could not determine religion for country: " << tag;
}

void V3::Country::convertCapital(const ClayManager& clayManager)
{
	for (const auto& substate: substates)
		if (substate->isCapital())
		{
			processedData.capitalStateName = substate->getHomeStateName();
			return;
		}

	// If that fails (capitals can get lost in transition, dead countries have no substates...), try anything.
	// maybe historical?
	if (const auto& historicalMatch = clayManager.getHistoricalCapitalState(sourceCountry->getTag()); historicalMatch)
	{
		processedData.capitalStateName = *historicalMatch;
		return;
	}

	// still nothing?
	if (processedData.capitalStateName.empty() && !substates.empty())
		processedData.capitalStateName = substates[0]->getHomeStateName();

	// TODO: Try anything harder. At least try to determine the majority of land?
	// TODO: After resource calc is called (call it early along with landshares!) use that.
}

void V3::Country::generateDecentralizedData(const LocalizationLoader& v3LocLoader, const EU4::EU4LocalizationLoader& eu4LocLoader)
{
	// COMMON/COUNTRY DATA
	processedData.color = generateDecentralizedColors(tag);
	processedData.tier = "principality";													// this appears to be common for decentralized nations.
	if (!substates.empty())																		// this really shouldn't be empty.
		processedData.capitalStateName = substates.front()->getHomeStateName(); // any will do.
	generateDecentralizedLocs(v3LocLoader, eu4LocLoader);

	// COMMON/HISTORY/COUNTRY - for now, let's default everything to tier: bottom regardless of geography.
	processedData.effects.emplace("effect_starting_technology_tier_7_tech"); // tech
	processedData.effects.emplace("effect_starting_politics_traditional");	 // politics
	processedData.effects.emplace("effect_native_conscription_3");				 // conscription
	processedData.laws.emplace("law_debt_slavery");									 // slavery
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
