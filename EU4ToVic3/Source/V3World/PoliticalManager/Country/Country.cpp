#include "Country.h"
#include "ClayManager/ClayManager.h"
#include "ClayManager/State/SubState.h"
#include "CommonRegexes.h"
#include "CountryManager/EU4Country.h"
#include "CountryTierMapper/CountryTierMapper.h"
#include "CultureMapper/CultureMapper.h"
#include "Loaders/LawLoader/Law.h"
#include "Loaders/LocLoader/LocalizationLoader.h"
#include "Loaders/LocalizationLoader/EU4LocalizationLoader.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "PopulationSetupMapper/PopulationSetupMapper.h"
#include "ReligionMapper/ReligionMapper.h"
#include "TechSetupMapper/TechSetupMapper.h"
#include <algorithm>
#include <cmath>
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

void V3::Country::storeVanillaCountryType(std::istream& theStream)
{
	registerVanillaTypeKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

std::vector<std::shared_ptr<V3::SubState>> V3::Country::topPercentileStatesByPop(const double percentile) const
{
	std::vector<std::shared_ptr<SubState>> incorporatedStates;
	for (const auto& state: subStates)
		if (state->isIncorporated())
			incorporatedStates.emplace_back(state);
	if (incorporatedStates.empty())
		return incorporatedStates;

	// Ranks this country's substates by population then returns the top x% of them by population, the largest state will always be returned.
	auto sortedSubStates = incorporatedStates;

	// descending order
	auto popComparison = [](const std::shared_ptr<SubState>& lhs, const std::shared_ptr<SubState>& rhs) {
		return lhs->getSubStatePops().getPopCount() > rhs->getSubStatePops().getPopCount();
	};

	std::sort(sortedSubStates.begin(), sortedSubStates.end(), popComparison);

	const int numTopSubStates = std::max(static_cast<int>(static_cast<double>(sortedSubStates.size()) * percentile), 1);

	return std::vector<std::shared_ptr<SubState>>{sortedSubStates.begin(), sortedSubStates.begin() + numTopSubStates};
}

double V3::Country::calculateBureaucracyUsage(const std::map<std::string, Law>& lawsMap) const
{
	// None of the hard-coded Vic3 values needed to calc this are in the defines for some reason.
	double usage = 0.0;

	usage += calcSubStateBureaucracy(lawsMap);
	usage += calcInstitutionBureaucracy();
	usage += calcCharacterBureaucracy();

	return usage;
}

int V3::Country::getTechInfraCap(const std::map<std::string, Tech>& techMap) const
{
	return std::accumulate(processedData.techs.begin(), processedData.techs.end(), 0, [this, techMap](int sum, const std::string& techName) {
		if (const auto& tech = getTechFromMap(techName, techMap); tech)
		{
			return sum + tech.value().infrastructureMax;
		}
		return sum;
	});
}

double V3::Country::getTechInfraMult(const std::map<std::string, Tech>& techMap) const
{
	return std::accumulate(processedData.techs.begin(), processedData.techs.end(), 0.0, [this, techMap](double sum, const std::string& techName) {
		if (const auto& tech = getTechFromMap(techName, techMap); tech)
		{
			return sum + tech.value().infrastructureMult;
		}
		return sum;
	});
}

int V3::Country::getThroughputMax(const std::map<std::string, Tech>& techMap) const
{
	// 20 is base level Economy of Scale NOT a define.
	return 20 + std::accumulate(processedData.techs.begin(), processedData.techs.end(), 0, [this, techMap](int sum, const std::string& techName) {
		if (const auto& tech = getTechFromMap(techName, techMap); tech)
		{
			return sum + tech.value().throughputMax;
		}
		return sum;
	});
}

bool V3::Country::hasAnyOfTech(const std::set<std::string>& techs) const
{
	if (techs.empty())
	{
		return true;
	}

	return std::ranges::any_of(techs, [&](const std::string& tech) {
		return processedData.techs.contains(tech);
	});
}

int V3::Country::getGovBuildingMax(const std::string& building, const std::map<std::string, Law>& lawsMap, const std::map<std::string, Tech>& techMap) const
{
	const auto tech = std::accumulate(processedData.techs.begin(), processedData.techs.end(), 0, [building, techMap](const int sum, const std::string& tech) {
		if (!techMap.contains(tech))
		{
			Log(LogLevel::Error) << "Couldn't find tech definition for: " << tech;
			return sum;
		}
		if (!techMap.at(tech).maxBuildingLevels.contains(building))
		{
			return sum;
		}

		return sum + techMap.at(tech).maxBuildingLevels.at(building);
	});

	const auto laws = std::accumulate(processedData.laws.begin(), processedData.laws.end(), 0, [building, lawsMap](const int sum, const std::string& law) {
		if (!lawsMap.contains(law))
		{
			Log(LogLevel::Error) << "Couldn't find law definition for: " << law;
			return sum;
		}
		if (!lawsMap.at(law).maxBuildingLevels.contains(building))
		{
			return sum;
		}

		return sum + lawsMap.at(law).maxBuildingLevels.at(building);
	});
	return tech + laws;
}

void V3::Country::distributeGovAdmins(const double target, const int PMGeneration, const std::map<std::string, V3::Tech>& techMap) const
{
	const auto topSubstates = topPercentileStatesByPop(0.3);
	if (topSubstates.empty())
		return;
	const auto topPop = getPopCount(topSubstates);

	// Pass out buildings by pop proportion of this subset of States. Generate production targets and account for throughput bonuses.
	double generated = 0;
	for (const auto& substate: topSubstates)
	{
		const double popProportion = static_cast<double>(substate->getSubStatePops().getPopCount()) / topPop;
		const double stateTarget = target * popProportion;

		// Calc levels for generation
		int levels = static_cast<int>(stateTarget * 100 / 101 / PMGeneration);
		double generation = levels * PMGeneration;
		if (const int throughputMax = getThroughputMax(techMap); levels > throughputMax)
		{
			levels = static_cast<int>(stateTarget / (PMGeneration + PMGeneration * (throughputMax / 100.0)));
			generation = levels * PMGeneration * (1 + throughputMax / 100.0);
		}

		const auto govAdmin = std::make_shared<Building>();
		govAdmin->setName("building_government_administration");
		govAdmin->setLevel(levels);

		substate->addBuilding(govAdmin);

		generated += generation;
	}

	// Handing out remainders, should be less than # of topSubstates
	// Treating throughput bonuses as negligible here
	for (const auto& substate: topSubstates)
	{
		auto isGovAdmin = [](const std::shared_ptr<Building>& b) {
			return b->getName() == "building_government_administration";
		};

		if (auto govAdmin = std::ranges::find_if(substate->getBuildings(), isGovAdmin); govAdmin != substate->getBuildings().end())
		{
			const auto levels = govAdmin->get()->getLevel();
			govAdmin->get()->setLevel(levels + 1);
			generated += PMGeneration;

			if (target <= generated + PMGeneration)
			{
				break;
			}
		}
		else
		{
			Log(LogLevel::Error) << "Failed to find building_government_administration in: " << substate->getHomeStateName();
		}
	}
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

void V3::Country::registerVanillaTypeKeys()
{
	registerKeyword("country_type", [this](std::istream& theStream) {
		vanillaData->vanillaType = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

bool V3::Country::humanPlayed() const
{
	if (!sourceCountry)
        {
		return false;
        }
	return sourceCountry->humanPlayed();
}

void V3::Country::convertFromEU4Country(const ClayManager& clayManager,
	 mappers::CultureMapper& cultureMapper,
	 const mappers::ReligionMapper& religionMapper,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader,
	 const mappers::IdeaEffectsMapper& ideaEffectMapper,
	 const mappers::CountryTierMapper& countryTierMapper,
	 const bool downTiers,
	 const bool vn)
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
	convertTier(countryTierMapper, downTiers, vn);

	// country type is determined after westernization is set.

	// namedaftercapital ? Unsure what to do with this.
	processedData.is_named_from_capital = false;

	// idea effects.
	processedData.ideaEffect = ideaEffectMapper.getEffectForIdeas(sourceCountry->getNationalIdeas());

	// slavery - we're setting this law right here and now as it's a base for further laws later when we have techs.
	if (sourceCountry->hasModifier("the_abolish_slavery_act"))
		processedData.laws.emplace("law_slavery_banned");

	// custom flag?
	if (sourceCountry->getNationalColors().getCustomColors())
		processedData.customColors = sourceCountry->getNationalColors().getCustomColors();
	if (sourceCountry->isRevolutionary() && sourceCountry->getNationalColors().getRevolutionaryColor())
		processedData.revolutionaryColor = sourceCountry->getNationalColors().getRevolutionaryColor();
}

void V3::Country::convertTier(const mappers::CountryTierMapper& countryTierMapper, bool downTiers, const bool vn)
{
	if (downTiers && vanillaData && !vanillaData->tier.empty())
	{
		processedData.tier = vanillaData->tier;
		return;
	}

	auto startingRank = sourceCountry->getGovernmentRank();
	if (downTiers)
	{
		if (startingRank > 2)
			--startingRank;
	}

	if (const auto& match = countryTierMapper.matchCountryTier(startingRank, subStates.size(), vn); match)
	{
		processedData.tier = *match;
	}
	else
		Log(LogLevel::Warning) << "No match for government rank " << sourceCountry->getGovernmentRank() << " (size " << subStates.size()
									  << ") in country_tiers.txt!";
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
	for (const auto& subState: subStates)
		if (subState->isCapital())
		{
			processedData.capitalStateName = subState->getHomeStateName();
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
	if (processedData.capitalStateName.empty() && !subStates.empty())
		processedData.capitalStateName = subStates[0]->getHomeStateName();

	// TODO: Try anything harder. At least try to determine the majority of land?
	// TODO: After resource calc is called (call it early along with landshares!) use that.
}

void V3::Country::generateDecentralizedData(const LocalizationLoader& v3LocLoader, const EU4::EU4LocalizationLoader& eu4LocLoader)
{
	// COMMON/COUNTRY DATA
	processedData.color = generateDecentralizedColors(tag);
	processedData.tier = "principality";													// this appears to be common for decentralized nations.
	if (!subStates.empty())																		// this really shouldn't be empty.
		processedData.capitalStateName = subStates.front()->getHomeStateName(); // any will do.
	generateDecentralizedLocs(v3LocLoader, eu4LocLoader);
	setDecentralizedEffects();
}

void V3::Country::setDecentralizedEffects()
{
	// COMMON/HISTORY/COUNTRY - for now, let's default everything to tier: bottom regardless of geography.
	processedData.effects.clear();
	processedData.effects.emplace("effect_starting_technology_tier_7_tech"); // tech
	processedData.effects.emplace("effect_starting_politics_traditional");	 // politics
	processedData.effects.emplace("effect_native_conscription_3");				 // conscription
	processedData.laws.clear();
	processedData.laws.emplace("law_debt_slavery"); // slavery
	processedData.populationEffects.clear();
	processedData.populationEffects.emplace("effect_starting_pop_literacy_baseline"); // no literacy
	processedData.populationEffects.emplace("effect_starting_pop_wealth_low");			 // no wealth
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

void V3::Country::copyVanillaData(const LocalizationLoader& v3LocLoader, const EU4::EU4LocalizationLoader& eu4LocLoader, bool vn)
{
	// this is done when conversion from eu4 source is impossible - likely because this country doesn't exist in eu4.
	if (!vanillaData)
		return;

	processedData.color = vanillaData->color;
	if (!vanillaData->vanillaType.empty() && vn) // restore *vanilla* vanilla country type.
		processedData.type = vanillaData->vanillaType;
	else
		processedData.type = vanillaData->type;
	processedData.tier = vanillaData->tier;
	processedData.cultures = vanillaData->cultures;
	processedData.religion = vanillaData->religion;
	processedData.capitalStateName = vanillaData->capitalStateName;
	processedData.is_named_from_capital = vanillaData->is_named_from_capital;

	// By default we're copying DECENTRALIZED nations. This means their effects should be set as if they were decentralized.
	if (!vn)
	{
		setDecentralizedEffects();
	}
	else
	{
		// When VN imports non-decentralized countries, we want entire history, such as it is.
		processedData.vanillaHistoryElements = unprocessedData.vanillaHistoryElements;
		processedData.vanillaPopulationElements = unprocessedData.vanillaPopulationElements;
		processedData.vanillaCharacterElements = unprocessedData.vanillaCharacterElements;
		processedData.vanillaDiplomaticPlayElements = unprocessedData.vanillaDiplomaticPlayElements;
	}

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

double V3::Country::getTotalDev() const
{
	return std::accumulate(subStates.begin(), subStates.end(), 0.0, [](double sum, const auto& substate) {
		return sum + substate->getTotalDev();
	});
}

double V3::Country::getOverPopulation() const
{
	double pops = 0;
	double capacity = 0;
	for (const auto& subState: subStates)
	{
		pops += subState->getSubStatePops().getPopCount();
		capacity += subState->getResource("bg_agriculture");
	}
	capacity *= 5000;
	if (capacity < 5000)
	{
		return 10.0;
	}
	const auto ratio = pops / capacity;
	if (ratio < 1.0)
	{
		return 1.0;
	}
	return ratio;
}

void V3::Country::determineWesternizationWealthAndLiteracy(double topTech,
	 double topInstitutions,
	 const mappers::CultureMapper& cultureMapper,
	 const mappers::ReligionMapper& religionMapper,
	 Configuration::EUROCENTRISM eurocentrism,
	 Configuration::STARTDATE startDate,
	 const DatingData& datingData,
	 const mappers::PopulationSetupMapper& populationSetupMapper)
{
	if (!sourceCountry)
		return; // don't do non-imports.

	calculateBaseLiteracy(religionMapper);
	calculateWesternization(topTech, topInstitutions, cultureMapper, eurocentrism);
	if (startDate != Configuration::STARTDATE::Vanilla)
		adjustLiteracy(datingData, cultureMapper, eurocentrism);
	applyLiteracyAndWealthEffects(populationSetupMapper);
	determineCountryType();
}

void V3::Country::determineCountryType()
{
	if (!processedData.westernized)
	{
		// 20 is the cutoff so american natives that heavily invest into tech
		// end up with more than 20 civLevel score and remain unrecognized.
		if (processedData.civLevel <= 20)
		{
			processedData.type = "decentralized";
			setDecentralizedEffects();
		}
		else
			processedData.type = "unrecognized";
	}
	else
	{
		// civilized/westernized nations can either be recognized or colonial.
		if (sourceCountry->isColony())
			processedData.type = "colonial";
		else
			processedData.type = "recognized";
	}
}

[[nodiscard]] double V3::Country::calcSubStateBureaucracy(const std::map<std::string, Law>& lawsMap) const
{
	double lawsMult = 0;
	for (const auto& law: processedData.laws)
	{
		if (!lawsMap.contains(law))
		{
			Log(LogLevel::Warning) << "Finding bureaucracy multiplier of law: " << law << " that has no definition! Skipping.";
			continue;
		}

		lawsMult += lawsMap.at(law).bureaucracyCostMult;
	}
	lawsMult = std::max(lawsMult + 1.0, 0.0);


	double usage = 0;
	for (const auto& subState: subStates)
	{
		if (!subState->isIncorporated())
		{
			continue;
		}
		// Incorporated States - 10 per incorporated state
		usage += 10;

		// Pops - only pops in incorporated states count
		// Modified by laws - game caps this at 0
		usage += subState->getSubStatePops().getPopCount() / 25000.0 * lawsMult;
	}
	return usage;
}

double V3::Country::calcInstitutionBureaucracy() const
{
	double usage = 0;
	const double cost = getIncorporatedPopCount() / 100000.0;
	for (const auto& level: processedData.institutions | std::views::values)
	{
		usage += cost * level; // If we end up mapping institution levels, it is cost * levels
	}
	return usage;
}

double V3::Country::calcCharacterBureaucracy() const
{
	double usage = 0;

	for (const auto& character: processedData.characters)
	{
		if (!character.admiral && !character.general)
			continue;

		// Defaulted commanders & rulers
		if (character.commanderRank.empty() || character.commanderRank.find("ruler") != std::string::npos)
		{
			usage += 10;
			continue;
		}

		// Commanders with ranks 1->5
		try
		{
			const int rank = std::stoi(character.commanderRank.substr(character.commanderRank.length() - 1));
			usage += 5 * (rank + 1);
		}
		catch (std::exception& e)
		{
			Log(LogLevel::Error) << "Broken military leader rank: " << character.commanderRank << " - " << e.what();
		}
	}

	return usage;
}

std::optional<V3::Tech> V3::Country::getTechFromMap(const std::string& techName, const std::map<std::string, Tech>& techMap) const
{
	if (!techMap.contains(techName))
	{
		Log(LogLevel::Error) << tag << "'s technology: " << techName << ", is not a recognized technology.";
		return std::nullopt;
	}
	return techMap.at(techName);
}

void V3::Country::applyLiteracyAndWealthEffects(const mappers::PopulationSetupMapper& populationSetupMapper)
{
	auto literacyEffect = populationSetupMapper.getLiteracyEffectForLiteracy(processedData.literacy);
	if (literacyEffect.empty())
		Log(LogLevel::Warning) << "Literacy effect for " << tag << " is empty! Something's wrong!";
	else
		processedData.populationEffects.emplace(literacyEffect);

	const auto& averageDev = sourceCountry->getAverageDevelopment();
	auto wealthEffect = populationSetupMapper.getWealthEffectForDev(averageDev);
	if (wealthEffect.empty())
		Log(LogLevel::Warning) << "Wealth effect for " << tag << " is empty! Something's wrong!";
	else
		processedData.populationEffects.emplace(wealthEffect);
}

void V3::Country::adjustLiteracy(const DatingData& datingData, const mappers::CultureMapper& cultureMapper, const Configuration::EUROCENTRISM eurocentrism)
{
	auto lastDate = datingData.lastEU4Date;
	if (lastDate > datingData.hardEndingDate)
		lastDate = datingData.hardEndingDate;

	processedData.literacy *= yearCapFactor(lastDate);

	// Apply cultural mod.
	if (eurocentrism == Configuration::EUROCENTRISM::EuroCentric)
		processedData.literacy *= 1 + (static_cast<double>(cultureMapper.getLiteracyScoreForCulture(*processedData.cultures.begin())) - 5.0) * 10.0 / 100.0;

	// Reduce the literacy for non-westernized nations according to their civLevel score.
	// -> Hardcoded exception to non-westernized literacy reduction for shinto countries so japan-likes may retain high industrialization potential.
	if (sourceCountry->getReligion() != "shinto")
		processedData.literacy *= pow(10, processedData.civLevel / 100 * 0.9 + 0.1) / 10;
}

double V3::Country::yearCapFactor(const date& targetDate)
{
	/*
		Drop nominal literacy or industry according to starting date. The curve is crafted to hit the following literacy percentage points:
		1836: 1
		1821: 0.85
		1750: 0.5
		1650: 0.3
		1490: 0.2
		1350: 0.15
		It will fail to hit those points exactly but won't err by much.
	*/
	const auto currentYear = std::fmax(targetDate.diffInYears(date("0.1.1")), 0);
	const auto yearFactor = (0.1 + 4'614'700 * currentYear) / (1 + static_cast<double>(103'810'000.0f) * currentYear - 54'029 * pow(currentYear, 2));
	return yearFactor;
}

void V3::Country::calculateWesternization(double topTech,
	 double topInstitutions,
	 const mappers::CultureMapper& cultureMapper,
	 Configuration::EUROCENTRISM eurocentrism)
{
	// This is base calc, from EU4. Even western countries in severe tech deficit will have a lower civLevel score.
	const auto totalTechs = sourceCountry->getMilTech() + sourceCountry->getAdmTech() + sourceCountry->getDipTech() + processedData.ideaEffect.getTechMod();
	// default cutoff point for civilization is ... 6! techs/ideas behind (total). (31 - 6) * 4 = 100 which is civilized, assuming no institution deficit.
	processedData.civLevel = (totalTechs + 31.0 - topTech) * 4;
	processedData.civLevel += (static_cast<double>(sourceCountry->getNumEmbracedInstitutions()) - topInstitutions) * 8;

	// If we're eurocentric, we're *ignoring* tech deficits and artificially deflating whatever score was achieved earlier for non-western countries.
	if (eurocentrism == Configuration::EUROCENTRISM::EuroCentric)
	{
		if (processedData.cultures.empty())
			Log(LogLevel::Warning) << "Trying to determine westernization of " << tag << " with no cultures!";
		else
		{
			const auto eurociv = cultureMapper.getWesternizationScoreForCulture(*processedData.cultures.begin());
			if (eurociv == 10)
			{
				processedData.civLevel = 100;
			}
			else
			{
				// Try to preserve eu4 centralized countries that suffer from *shit* tech.
				if (processedData.civLevel >= 20)
					processedData.civLevel = std::min(processedData.civLevel, eurociv * 10.0);
				else
					processedData.civLevel = std::max(processedData.civLevel, eurociv * 10.0);
			}
			processedData.industryFactor = cultureMapper.getIndustryScoreForCulture(*processedData.cultures.begin()) / 5.0; // ranges 0.0-2.0
		}
	}

	if (processedData.civLevel < 0)
	{
		processedData.civLevel = 0;
	}
	if (processedData.civLevel >= 100)
	{
		processedData.civLevel = 100;
		processedData.westernized = true;
	}
}

void V3::Country::calculateBaseLiteracy(const mappers::ReligionMapper& religionMapper)
{
	auto literacy = 0.4;

	if (religionMapper.getV3ReligionDefinitions().contains(processedData.religion))
	{
		const auto& religion = religionMapper.getV3ReligionDefinitions().at(processedData.religion);
		const auto& traits = religion.traits;
		if (religion.name == "protestant" || traits.contains("eastern"))
			literacy += 0.1;
	}

	if (sourceCountry->hasModifier("sunday_schools"))
		literacy += 0.05;
	if (sourceCountry->hasModifier("the_education_act"))
		literacy += 0.05;
	if (sourceCountry->hasModifier("monastic_education_system"))
		literacy += 0.05;
	if (sourceCountry->hasModifier("western_embassy_mission"))
		literacy += 0.05;

	// Universities grant at most 10% literacy, with either having 10 or when having them in 10% of provinces, whichever comes sooner.

	const auto& provinces = sourceCountry->getProvinces();
	const auto numProvinces = provinces.size();
	auto numUniversities = 0;

	for (const auto& province: provinces | std::views::values)
		if (province->hasBuilding("university"))
			numUniversities++;

	double universityBonus1 = 0;
	if (numProvinces > 0)
	{
		universityBonus1 = static_cast<double>(numUniversities) / static_cast<double>(numProvinces);
	}
	const auto universityBonus2 = numUniversities * 0.01;

	const auto universityBonus = std::min(std::max(universityBonus1, universityBonus2), 0.1);

	literacy += universityBonus;

	// Adding whatever literacy bonus or malus we have from eu4 ideas.
	processedData.literacy = literacy + static_cast<double>(processedData.ideaEffect.literacy) / 100.0;
}

void V3::Country::setTechs(const mappers::TechSetupMapper& techSetupMapper, double productionScore, double militaryScore, double societyScore)
{
	auto productionTechs = techSetupMapper.getTechsForScoreTrack("production", productionScore);
	auto militaryTechs = techSetupMapper.getTechsForScoreTrack("military", militaryScore);
	auto societyTechs = techSetupMapper.getTechsForScoreTrack("society", societyScore);

	processedData.techs.insert(productionTechs.begin(), productionTechs.end());
	processedData.techs.insert(militaryTechs.begin(), militaryTechs.end());
	processedData.techs.insert(societyTechs.begin(), societyTechs.end());
}

V3::Relation& V3::Country::getRelation(const std::string& target)
{
	const auto& relation = processedData.relations.find(target);
	if (relation != processedData.relations.end())
		return relation->second;
	Relation newRelation(target);
	processedData.relations.emplace(target, newRelation);
	const auto& newRelRef = processedData.relations.find(target);
	return newRelRef->second;
}

void V3::Country::convertCharacters(const mappers::CharacterTraitMapper& characterTraitMapper,
	 const float ageShift,
	 const ClayManager& clayManager,
	 mappers::CultureMapper& cultureMapper,
	 const mappers::ReligionMapper& religionMapper,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader,
	 const date& conversionDate)
{
	bool ruler = false;
	bool consort = false;
	// check for married status.
	for (const auto& eu4Character: sourceCountry->getCharacters())
		if (eu4Character.ruler)
			ruler = true;
		else if (eu4Character.consort)
			consort = true;

	for (const auto& eu4Character: sourceCountry->getCharacters())
	{
		if (eu4Character.name.empty() && eu4Character.leaderName.empty())
			continue; // start-date regencies and similar
		auto character = Character(eu4Character,
			 characterTraitMapper,
			 ageShift,
			 clayManager,
			 cultureMapper,
			 religionMapper,
			 cultureLoader,
			 religionLoader,
			 processedData.capitalStateName,
			 tag,
			 conversionDate);
		if (character.ruler && ruler && consort)
			character.married = true;

		processedData.characters.emplace_back(character);
	}
}

int V3::Country::getPopCount() const
{
	return getPopCount(subStates);
}

int V3::Country::getIncorporatedPopCount() const
{
	std::vector<std::shared_ptr<SubState>> incorporatedSubStates;
	for (const auto& subState: subStates)
		if (subState->isIncorporated())
			incorporatedSubStates.emplace_back(subState);
	return getPopCount(incorporatedSubStates);
}

int V3::Country::getPopCount(const std::vector<std::shared_ptr<SubState>>& theSubStates)
{
	return std::accumulate(theSubStates.begin(), theSubStates.end(), 0, [](int sum, const auto& substate) {
		return sum + substate->getSubStatePops().getPopCount();
	});
}

int V3::Country::getVanillaPopCount() const
{
	return std::accumulate(subStates.begin(), subStates.end(), 0, [](int sum, const auto& substate) {
		return sum + substate->getVanillaPopCount();
	});
}

void V3::Country::leaveIsolationism()
{
	if (processedData.laws.contains("law_isolationism"))
	{
		processedData.laws.erase("law_isolationism");
		processedData.laws.emplace("law_mercantilism");
		processedData.techs.emplace("international_trade");
		processedData.techs.emplace("tech_bureaucracy");
		processedData.techs.emplace("urbanization");
	}
}

bool V3::Country::isCultureDiscriminated(const std::string& culture, const mappers::CultureMapper& cultureMapper) const
{
	if (processedData.cultures.contains(culture))
		return false;

	if (processedData.discriminationLevel == ProcessedData::DISCRIMINATION_LEVEL::National_Supremacy)
	{
		for (const auto& primaryCulture: processedData.cultures)
		{
			if (cultureMapper.doCulturesShareHeritageTrait(primaryCulture, culture) && *cultureMapper.doCulturesShareHeritageTrait(primaryCulture, culture) &&
				 cultureMapper.doCulturesShareNonHeritageTrait(primaryCulture, culture) && *cultureMapper.doCulturesShareNonHeritageTrait(primaryCulture, culture))
				return false;
		}
	}

	if (processedData.discriminationLevel == ProcessedData::DISCRIMINATION_LEVEL::Racial_Segregation)
	{
		for (const auto& primaryCulture: processedData.cultures)
		{
			if (cultureMapper.doCulturesShareHeritageTrait(primaryCulture, culture) && *cultureMapper.doCulturesShareHeritageTrait(primaryCulture, culture))
				return false;
		}
	}

	if (processedData.discriminationLevel == ProcessedData::DISCRIMINATION_LEVEL::Cultural_Exclusion)
	{
		for (const auto& primaryCulture: processedData.cultures)
		{
			if (cultureMapper.doCulturesShareHeritageTrait(primaryCulture, culture) && *cultureMapper.doCulturesShareHeritageTrait(primaryCulture, culture) ||
				 cultureMapper.doCulturesShareNonHeritageTrait(primaryCulture, culture) && *cultureMapper.doCulturesShareNonHeritageTrait(primaryCulture, culture))
			{
				return false;
			}
		}
	}

	if (processedData.discriminationLevel == ProcessedData::DISCRIMINATION_LEVEL::Multicultural)
	{
		return false;
	}

	return true;
}
