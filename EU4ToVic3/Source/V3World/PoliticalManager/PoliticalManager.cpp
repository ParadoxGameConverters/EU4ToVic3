#include "PoliticalManager.h"

#include "ClayManager/ClayManager.h"
#include "ClayManager/State/Province.h"
#include "ClayManager/State/State.h"
#include "ClayManager/State/SubState.h"
#include "ColonialRegionMapper/ColonialRegionMapper.h"
#include "Configuration.h"
#include "Country/Country.h"
#include "CountryManager/EU4Country.h"
#include "CultureMapper/CultureMapper.h"
#include "DiplomacyParser/EU4Agreement.h"
#include "Loaders/CountryDefinitionLoader/CountryDefinitionLoader.h"
#include "Loaders/LawLoader/LawLoader.h"
#include "Log.h"
#include "Mappers/CountryMapper/CountryMapper.h"
#include "PopManager/PopManager.h"
#include "TechValues/TechValues.h"
#include <cmath>
#include <numeric>
#include <ranges>

namespace
{
std::vector<std::string> sortMap(const std::map<std::string, int>& theMap)
{
	std::vector<std::string> sorted;

	std::vector<std::pair<std::string, int>> pairs;
	for (const auto& theElement: theMap)
		pairs.emplace_back(theElement);

	std::sort(pairs.begin(), pairs.end(), [=](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
		return a.second > b.second;
	});

	for (const auto& tag: pairs | std::views::keys)
		sorted.emplace_back(tag);

	return sorted;
}
} // namespace


void V3::PoliticalManager::initializeVanillaCountries(const commonItems::ModFilesystem& modFS, const commonItems::ModFilesystem& vanillaFS)
{
	Log(LogLevel::Info) << "-> Loading Vanilla Countries.";
	CountryDefinitionLoader definitionLoader;
	definitionLoader.loadCommonCountries(modFS);
	definitionLoader.reloadCommonCountries(vanillaFS);
	countries = definitionLoader.getCountries();
	Log(LogLevel::Info) << "<> " << countries.size() << " vanilla countries loaded.";

	Log(LogLevel::Info) << "-> Loading Vanilla Country Histories.";
	vanillaCountryHistoryLoader.loadVanillaCountryHistories(modFS);
	for (const auto& [tag, historyElements]: vanillaCountryHistoryLoader.getCountryHistoryElements())
		if (countries.contains(tag))
			countries.at(tag)->setVanillaHistoryElements(historyElements);
	Log(LogLevel::Info) << "<> " << vanillaCountryHistoryLoader.getCountryHistoryElements().size() << " vanilla country histories loaded.";

	Log(LogLevel::Info) << "-> Loading Vanilla Population Histories.";
	vanillaPopulationHistoryLoader.loadVanillaPopulationHistories(modFS);
	for (const auto& [tag, historyElements]: vanillaPopulationHistoryLoader.getPopulationHistoryElements())
		if (countries.contains(tag))
			countries.at(tag)->setVanillaPopulationElements(historyElements);
	Log(LogLevel::Info) << "<> " << vanillaCountryHistoryLoader.getCountryHistoryElements().size() << " vanilla country histories loaded.";

	Log(LogLevel::Info) << "-> Loading Vanilla Characters.";
	vanillaCharacterLoader.loadVanillaCharacters(modFS);
	for (const auto& [tag, historyElements]: vanillaCharacterLoader.getCharacterElements())
		if (countries.contains(tag))
			countries.at(tag)->setVanillaCharacterElements(historyElements);
	Log(LogLevel::Info) << "<> " << vanillaCharacterLoader.getCharacterElements().size() << " vanilla characters loaded.";

	Log(LogLevel::Info) << "-> Loading Vanilla Diplomacy.";
	vanillaDiplomacyLoader.loadVanillaDiplomacy(modFS);
	Log(LogLevel::Info) << "<> Loaded " << vanillaDiplomacyLoader.getAgreementEntries().size() << " vanilla agreements, "
							  << vanillaDiplomacyLoader.getRelationEntries().size() << " vanilla relations and " << vanillaDiplomacyLoader.getTruceEntries().size()
							  << " vanilla truces.";

	Log(LogLevel::Info) << "-> Loading Vanilla Diplomatic Plays.";
	vanillaDiplomaticPlayLoader.loadVanillaDiplomaticPlays(modFS);
	for (const auto& [tag, historyElements]: vanillaDiplomaticPlayLoader.getDiploPlayElements())
		if (countries.contains(tag))
			countries.at(tag)->setVanillaDiplomaticPlayElements(historyElements);
	Log(LogLevel::Info) << "<> " << vanillaDiplomaticPlayLoader.getDiploPlayElements().size() << " vanilla characters loaded.";
}

void V3::PoliticalManager::loadCountryMapper(const std::shared_ptr<mappers::CountryMapper>& theCountryMapper)
{
	countryMapper = theCountryMapper;
	for (const auto& countryTag: countries | std::views::keys)
		countryMapper->registerKnownVanillaV3Tag(countryTag);
}

void V3::PoliticalManager::loadPopulationSetupMapperRules(const std::filesystem::path& filePath)
{
	populationSetupMapper.loadMappingRules(filePath);
}

void V3::PoliticalManager::loadIdeaEffectMapperRules(const std::filesystem::path& filePath)
{
	ideaEffectMapper.loadMappingRules(filePath);
}

void V3::PoliticalManager::loadTechSetupMapperRules(const std::filesystem::path& filePath)
{
	techSetupMapper.loadMappingRules(filePath);
}

void V3::PoliticalManager::loadCountryTierMapperRules(const std::filesystem::path& filePath)
{
	countryTierMapper.loadMappingRules(filePath);
}

void V3::PoliticalManager::loadLawMapperRules(const std::filesystem::path& filePath)
{
	lawMapper.loadMappingRules(filePath);
}

void V3::PoliticalManager::loadLawDefinitions(const commonItems::ModFilesystem& modFS)
{
	LawLoader loader;
	loader.loadLaws(modFS);
	lawMapper.loadLawDefinitions(loader.getLaws());
}

void V3::PoliticalManager::loadDiplomaticMapperRules(const std::filesystem::path& filePath)
{
	diplomaticMapper.loadMappingRules(filePath);
}

void V3::PoliticalManager::loadCharacterTraitMapperRules(const std::filesystem::path& filePath)
{
	characterTraitMapper.loadMappingRules(filePath);
}

void V3::PoliticalManager::loadColonialTagMapperRules(const std::filesystem::path& filePath)
{
	colonialTagMapper.loadMappingRules(filePath);
}

void V3::PoliticalManager::loadAIStrategies(const std::filesystem::path& filePath)
{
	aiStrategyMapper.loadMappingRules(filePath);
}

void V3::PoliticalManager::loadAISecretGoals(const std::filesystem::path& filePath)
{
	aiSecretGoalMapper.loadMappingRules(filePath);
}

void V3::PoliticalManager::importEU4Countries(const std::map<std::string, std::shared_ptr<EU4::Country>>& eu4Countries)
{
	Log(LogLevel::Info) << "-> Moving over EU4 Countries.";

	for (const auto& eu4country: eu4Countries | std::views::values)
	{
		auto v3Tag = countryMapper->assignV3TagToEU4Country(eu4country);

		std::shared_ptr<Country> country;
		if (countries.contains(v3Tag))
		{
			country = countries.at(v3Tag);
		}
		else
		{
			country = std::make_shared<Country>();
			country->setTag(v3Tag);
			countries.emplace(v3Tag, country);
		}

		country->setSourceCountry(eu4country);
	}

	Log(LogLevel::Info) << "<> Vic3 now has " << countries.size() << " known countries.";
}

void V3::PoliticalManager::generateDecentralizedCountries(const ClayManager& clayManager, const PopManager& popManager)
{
	Log(LogLevel::Info) << "-> Generating decentralized countries.";

	// How do we generate decentralized states? We look at states and their unowned substates.
	// Those substates with a particular culture are banded together and shoved into a new country.
	// If there's no demographic available (mapping from a wasteland or out of scope), we load
	// cultural information (not demographics!) from vanilla sources.

	auto sortedSubstates = sortSubStatesByCultures(clayManager, popManager);

	for (const auto& [culture, subStates]: sortedSubstates)
	{
		generateDecentralizedCountry(culture, subStates);
	}

	Log(LogLevel::Info) << "<> Generated " << sortedSubstates.size() << " decentralized countries.";
}

void V3::PoliticalManager::generateDecentralizedCountry(const std::string& culture, const std::vector<std::shared_ptr<SubState>>& subStates)
{
	auto v3tag = countryMapper->requestNewV3Tag();
	auto newCountry = std::make_shared<Country>();
	newCountry->setTag(v3tag);
	newCountry->setSubStates(subStates);
	ProcessedData data;
	data.cultures.emplace(culture);
	data.type = "decentralized";
	newCountry->setProcessedData(data);
	for (const auto& subState: newCountry->getSubStates())
		subState->setOwner(newCountry);

	countries.emplace(v3tag, newCountry);
	// ... fingers crossed?
}

V3::PoliticalManager::CulturalSubStates V3::PoliticalManager::sortSubStatesByCultures(const ClayManager& clayManager, const PopManager& popManager)
{
	CulturalSubStates culturalSubStates;

	for (const auto& [stateName, state]: clayManager.getStates())
	{
		for (const auto& subState: state->getSubStates())
		{
			if (subState->getOwner()) // don't need owned substates.
				continue;

			// Does this substate have a leading demographic? Or any demographic?
			std::string leadCulture;
			if (!subState->getDemographics().empty())
			{
				leadCulture = getDominantDemographic(subState->getDemographics());
			}
			else
			{
				// Ask popmanager for something vanillaish for the entire state (can't use our substates for this).
				leadCulture = popManager.getDominantDWCulture(stateName);
			}
			if (!culturalSubStates.contains(leadCulture))
				culturalSubStates.emplace(leadCulture, std::vector<std::shared_ptr<SubState>>{});
			culturalSubStates.at(leadCulture).push_back(subState);
		}
	}

	return culturalSubStates;
}

std::string V3::PoliticalManager::getDominantDemographic(const std::vector<Demographic>& demographics)
{
	std::map<std::string, double> cultureScores;

	for (const auto& demo: demographics)
	{
		if (cultureScores.contains(demo.culture))
			cultureScores.at(demo.culture) += demo.lowerRatio; // using lower ratio to represent farmers - the majority.
		else
			cultureScores.emplace(demo.culture, demo.lowerRatio);
	}

	const auto highest = std::max_element(std::begin(cultureScores), std::end(cultureScores), [](const auto& p1, const auto& p2) {
		return p1.second < p2.second;
	});

	return highest->first;
}

void V3::PoliticalManager::convertAllCountries(const ClayManager& clayManager,
	 mappers::CultureMapper& cultureMapper,
	 const mappers::ReligionMapper& religionMapper,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader,
	 const LocalizationLoader& v3LocLoader,
	 const EU4::EU4LocalizationLoader& eu4LocLoader,
	 const bool downTiers,
	 const bool vn) const
{
	Log(LogLevel::Info) << "-> Converting countries.";

	for (const auto& [tag, country]: countries)
	{
		// this is a freshly-generated decentralized country with no source data whatsoever.
		if (!country->getVanillaData() && country->getProcessedData().type == "decentralized")
			country->generateDecentralizedData(v3LocLoader, eu4LocLoader);

		// this is a vic3-only (vanilla) country with no EU4 match. It's likely extinct.
		else if (country->getVanillaData() && !country->getSourceCountry())
			country->copyVanillaData(v3LocLoader, eu4LocLoader, vn);

		// otherwise, this is a regular imported EU4 country
		else if (country->getSourceCountry())
			country->convertFromEU4Country(clayManager,
				 cultureMapper,
				 religionMapper,
				 cultureLoader,
				 religionLoader,
				 ideaEffectMapper,
				 countryTierMapper,
				 downTiers,
				 vn);

		else
			Log(LogLevel::Warning) << "Country " << tag << " has no known sources! Not importing!";
	}
}

bool V3::PoliticalManager::isTagDecentralized(const std::string& v3Tag) const
{
	if (!countries.contains(v3Tag))
		return false;
	const auto& country = countries.at(v3Tag);

	// this means it's loaded from disk, and it's decentralized.
	if (country->getVanillaData() && (*country->getVanillaData()).type == "decentralized")
		return true;
	return false;
}

std::shared_ptr<V3::Country> V3::PoliticalManager::getCountry(const std::string& v3Tag) const
{
	if (countries.contains(v3Tag))
		return countries.at(v3Tag);
	return nullptr;
}

void V3::PoliticalManager::determineAndApplyWesternization(const mappers::CultureMapper& cultureMapper,
	 const mappers::ReligionMapper& religionMapper,
	 const Configuration::EUROCENTRISM eurocentrism,
	 const Configuration::STARTDATE startDate,
	 const DatingData& datingData)
{
	Log(LogLevel::Info) << "-> Determining Westernization.";
	double topTech = 0;
	double topInstitutions = 0;

	// Determine top tech/institutions.
	for (const auto& country: countries | std::views::values)
	{
		if (!country->getSourceCountry())
			continue; // we need only eu4 imports
		const auto& sourceCountry = country->getSourceCountry();
		if (sourceCountry->getProvinces().empty())
			continue; // dead nations are stuck.
		const auto totalTechs =
			 sourceCountry->getMilTech() + sourceCountry->getAdmTech() + sourceCountry->getDipTech() + country->getProcessedData().ideaEffect.getTechMod();
		if (totalTechs > topTech)
			topTech = totalTechs;
		const auto currInstitutions = sourceCountry->getNumEmbracedInstitutions();
		if (currInstitutions > topInstitutions)
			topInstitutions = currInstitutions;
	}

	// and distribute tech level.
	int civs = 0;
	int uncivs = 0;
	for (const auto& country: countries | std::views::values)
	{
		country->determineWesternizationWealthAndLiteracy(topTech,
			 topInstitutions,
			 cultureMapper,
			 religionMapper,
			 eurocentrism,
			 startDate,
			 datingData,
			 populationSetupMapper);

		// Bookkeeping.
		if (!country->getSubStates().empty())
		{
			if (country->getProcessedData().westernized)
				++civs;
			else
				++uncivs;
		}
	}
	Log(LogLevel::Info) << "<> There are " << civs << " westernized and " << uncivs << " unwesternized landed nations.";
}

void V3::PoliticalManager::setupTech()
{
	Log(LogLevel::Info) << "-> Setting up tech for landed EU4 countries.";

	const TechValues techValues(countries);
	auto counter = 0;
	for (const auto& [tag, country]: countries)
	{
		if (!TechValues::isValidCountryForTechConversion(*country))
			continue;

		const auto productionScore = techValues.getProductionTechPercentile(tag);
		const auto militaryScore = techValues.getMilitaryTechPercentile(tag);
		const auto societyScore = techValues.getSocietyTechPercentile(tag);
		country->setTechs(techSetupMapper, productionScore, militaryScore, societyScore);
		country->setProductionTechPercentile(productionScore);
		++counter;
	}

	Log(LogLevel::Info) << "<> " << counter << " countries initialized.";
}

void V3::PoliticalManager::setupLaws()
{
	Log(LogLevel::Info) << "-> Setting up laws for landed EU4 countries.";
	auto counter = 0;
	auto serf = 0;
	auto tenet = 0;
	auto home = 0;
	auto noschool = 0;
	auto relig = 0;
	auto priv = 0;
	auto univ = 0;
	for (const auto& country: countries | std::views::values)
	{
		if (!TechValues::isValidCountryForTechConversion(*country))
			continue;

		// ORDER matters! We can set some laws without having other blocking them if we set them first.
		// We go by what's *important*. Political first!
		grantLawFromGroup("lawgroup_governance_principles", country);
		grantLawFromGroup("lawgroup_distribution_of_power", country);
		grantLawFromGroup("lawgroup_slavery", country);
		grantLawFromGroup("lawgroup_citizenship", country);
		grantLawFromGroup("lawgroup_education_system", country); // exception to the rule, education is important to block serfdoms and other problematic reforms.
		grantLawFromGroup("lawgroup_land_reform", country);
		grantLawFromGroup("lawgroup_colonization", country);
		grantLawFromGroup("lawgroup_church_and_state", country);
		grantLawFromGroup("lawgroup_economic_system", country);
		grantLawFromGroup("lawgroup_bureaucracy", country);
		grantLawFromGroup("lawgroup_army_model", country);
		grantLawFromGroup("lawgroup_taxation", country);
		grantLawFromGroup("lawgroup_trade_policy", country);
		grantLawFromGroup("lawgroup_internal_security", country);
		grantLawFromGroup("lawgroup_migration", country);
		grantLawFromGroup("lawgroup_policing", country);
		grantLawFromGroup("lawgroup_rights_of_women", country);
		// Social LAST.
		grantLawFromGroup("lawgroup_labor_rights", country);
		grantLawFromGroup("lawgroup_free_speech", country);
		grantLawFromGroup("lawgroup_health_system", country);
		grantLawFromGroup("lawgroup_welfare", country);
		++counter;

		// Laws are set, figure out which institutions our laws have
		setupInstitutions(country);

		// Some debugging logs to help with debugging on forums.
		if (country->getProcessedData().laws.contains("law_serfdom"))
		{
			serf++;
		}
		if (country->getProcessedData().laws.contains("law_tenant_farmers"))
			tenet++;
		if (country->getProcessedData().laws.contains("law_homesteading"))
			home++;
		if (country->getProcessedData().laws.contains("law_no_schools"))
			noschool++;
		if (country->getProcessedData().laws.contains("law_religious_schools"))
			relig++;
		if (country->getProcessedData().laws.contains("law_private_schools"))
			priv++;
		if (country->getProcessedData().laws.contains("law_public_schools"))
			univ++;
	}
	Log(LogLevel::Info) << "<> " << counter << " countries codified.";
	Log(LogLevel::Debug) << "<> " << serf << " serfdom, " << tenet << " tenant farmers, " << home << " homesteading.";
	Log(LogLevel::Debug) << "<> " << noschool << " no schooling, " << relig << " religious schools, " << priv << " private schools, " << univ
								<< " public schools.";
}

void V3::PoliticalManager::setupInstitutions(const std::shared_ptr<Country>& country) const
{
	for (const auto& law: country->getProcessedData().laws)
	{
		if (auto institution = lawMapper.getLaws().at(law).institution; !institution.empty())
		{
			if (institution == "institution_schools")
				country->addInstitution(institution, 3); // boost to max to generate starting literacy.
			else
				country->addInstitution(institution, 1); // otherwise don't.
		}
	}
}

void V3::PoliticalManager::grantLawFromGroup(const std::string& lawGroup, const std::shared_ptr<Country>& country) const
{
	if (const auto law = lawMapper.grantLawFromGroup(lawGroup, *country); law)
	{
		country->addLaw(*law);

		// Knowing what's discrimination level in a country is relevant when incorporating or not incorporating states.
		if (lawGroup == "lawgroup_citizenship")
		{
			ProcessedData::DISCRIMINATION_LEVEL discrimination;
			if (*law == "law_ethnostate")
				discrimination = ProcessedData::DISCRIMINATION_LEVEL::Ethnostate;
			else if (*law == "law_national_supremacy")
				discrimination = ProcessedData::DISCRIMINATION_LEVEL::National_Supremacy;
			else if (*law == "law_racial_segregation")
				discrimination = ProcessedData::DISCRIMINATION_LEVEL::Racial_Segregation;
			else if (*law == "law_cultural_exclusion")
				discrimination = ProcessedData::DISCRIMINATION_LEVEL::Cultural_Exclusion;
			else if (*law == "law_multicultural")
				discrimination = ProcessedData::DISCRIMINATION_LEVEL::Multicultural;
			else
				discrimination = ProcessedData::DISCRIMINATION_LEVEL::Unknown;

			country->setDiscriminationLevel(discrimination);
		}
	}
}

void V3::PoliticalManager::convertDiplomacy(const std::vector<EU4::EU4Agreement>& eu4Agreements)
{
	Log(LogLevel::Info) << "-> Transcribing diplomatic agreements.";

	const std::set<std::string> subjects = {"dominion", "protectorate", "tributary", "personal_union", "puppet", "vassal"};

	for (auto& agreement: eu4Agreements)
	{
		const auto& EU4Tag1 = agreement.getOriginTag();
		auto ifV3Tag1 = countryMapper->getV3Tag(EU4Tag1);
		if (!ifV3Tag1)
			continue;
		auto V3Tag1 = *ifV3Tag1;

		const auto& EU4Tag2 = agreement.getTargetTag();
		auto ifV3Tag2 = countryMapper->getV3Tag(EU4Tag2);
		if (!ifV3Tag2)
			continue;
		auto V3Tag2 = *ifV3Tag2;

		const auto& country1 = countries.find(V3Tag1);
		const auto& country2 = countries.find(V3Tag2);
		if (country1 == countries.end())
		{
			Log(LogLevel::Warning) << "Vic3 country " << V3Tag1 << " used in diplomatic agreement doesn't exist";
			continue;
		}
		if (country2 == countries.end())
		{
			Log(LogLevel::Warning) << "Vic3 country " << V3Tag2 << " used in diplomatic agreement doesn't exist";
			continue;
		}

		// Don't create for/with nations that didn't survive province conversion!
		if (country1->second->getSubStates().empty())
			continue;
		if (country2->second->getSubStates().empty())
			continue;

		auto& r1 = country1->second->getRelation(V3Tag2); // relation TO target
		auto& r2 = country2->second->getRelation(V3Tag1); // relation TO source

		Agreement newAgreement;
		newAgreement.first = V3Tag1;
		newAgreement.second = V3Tag2;
		newAgreement.start_date = agreement.getStartDate();

		// translate eu4 to vic3 agreement.
		if (const auto& newType = diplomaticMapper.getAgreementType(agreement.getAgreementType()); newType)
			newAgreement.type = *newType;

		// boost relations.
		const auto boost = diplomaticMapper.getRelationshipBoost(agreement.getAgreementType());
		r1.increaseRelations(boost);
		r2.increaseRelations(boost);

		// fix specifics
		if (agreement.getAgreementType() == "private_enterprise" && country2->second->getSourceCountry()->isTradeCompany())
		{
			// if this is a TC agreement, push everyone out of isolationism into mercantilism!
			country1->second->leaveIsolationism();
			country2->second->leaveIsolationism();
		}
		if (newAgreement.type == "vassal")
		{
			// Watch for westernization!
			if (country1->second->getProcessedData().type != "unrecognized")
				newAgreement.type = "puppet";
		}
		if (newAgreement.type == "puppet" || newAgreement.type == "dominion")
		{
			// Watch for westernization!
			if (country1->second->getProcessedData().type == "unrecognized")
				newAgreement.type = "vassal";
		}
		if (newAgreement.type == "tributary")
		{
			// Watch for westernization!
			if (country1->second->getProcessedData().type != "unrecognized")
				newAgreement.type = "protectorate";
		}
		if (newAgreement.type == "protectorate")
		{
			// Watch for westernization!
			if (country1->second->getProcessedData().type == "unrecognized")
				newAgreement.type = "tributary";
		}
		/* if (newAgreement.type == "customs_union") // Obsoleted by the new "Power Bloc" system
		{
			// vic3 has this reversed
			newAgreement.first = V3Tag2;
			newAgreement.second = V3Tag1;

			// if someone's isolationist, bail.
			if (country1->second->getProcessedData().laws.contains("law_isolationism") || country2->second->getProcessedData().laws.contains("law_isolationism"))
				continue;
		}*/
		if (newAgreement.type == "double_defensive_pact")
		{
			// fix and file twice.
			newAgreement.type = "defensive_pact";
			agreements.push_back(newAgreement);
			newAgreement.first = V3Tag2;
			newAgreement.second = V3Tag1;

			// if anyone doesn't have required diplotech, bail.
			if (!country1->second->getProcessedData().techs.contains("international_relations") ||
				 !country2->second->getProcessedData().techs.contains("international_relations"))
				continue;
		}

		if (subjects.contains(newAgreement.type))
		{
			// For subjects, set libertyDesire, reduced by 50 because Vic3 default is 50.
			newAgreement.libertyDesire = country2->second->getSourceCountry()->getLibertyDesire() - 50;
			// and record overlordship locally for further use.
			country2->second->setOverlord(V3Tag1);
		}

		// store agreement
		if (!newAgreement.type.empty())
		{
			agreements.push_back(newAgreement);
		}
	}
	Log(LogLevel::Info) << "<> Transcribed " << agreements.size() << " agreements.";
}

void V3::PoliticalManager::createPowerBlocs()
{
	Log(LogLevel::Info) << "-> Creating Power Blocs.";

	// We ask if countries have subjects, and then make some determination based of their governing principles.
	// The major exception is HRE, which we utterly ignore in this context and throw them manually into separate power bloc.

	std::map<std::string, std::set<std::string>> ownerSubjects;
	std::map<std::string, std::set<std::string>> mergedOwnerSubjects;
	std::map<std::string, std::string> subjectOwner;
	std::set<std::string> subjectTypes = {"dominion", "protectorate", "tributary", "personal_union", "puppet", "vassal"};
	std::map<std::string, date> blocStartDates;

	for (const auto& agreement: agreements)
	{
		auto first = countries.at(agreement.first);
		auto second = countries.at(agreement.second);

		// skip anything to do with HREmembers. They can't be members of any bloc but HRE. Their subjects also are NOT in HRE unless they are HREmembers.
		if (first->getProcessedData().isHREmember || first->getProcessedData().isHREmperor)
			continue;
		if (second->getProcessedData().isHREmember || second->getProcessedData().isHREmperor)
			continue;


		if (subjectTypes.contains(agreement.type))
		{
			if (!ownerSubjects.contains(agreement.first))
				ownerSubjects.emplace(agreement.first, std::set<std::string>{});
			ownerSubjects.at(agreement.first).emplace(agreement.second);
			subjectOwner.emplace(agreement.second, agreement.first);

			// As for dating, only update if it's older than the current one we're processing.
			if (!blocStartDates.contains(agreement.first))
				blocStartDates.emplace(agreement.first, agreement.start_date);
			// We're effectively saying the block started existing when the owner got its first subject, whomever it was.
			if (blocStartDates.at(agreement.first) > agreement.start_date)
				blocStartDates.at(agreement.first) = agreement.start_date;
		}
	}

	// For subject chains, we need to throw everything under the overlord, including the owners.
	for (const auto& [owner, subjects]: ownerSubjects)
	{
		if (subjects.contains(owner))
		{
			// This nation is both a subject and an owner, possibly a colonial nation or PU having own vassals.
			auto overlord = subjectOwner.at(owner);
			auto third = countries.at(overlord);

			// If overlord is a HREmember, bail. Rest of the chain won't be a part of any bloc (they aren't in HRE unless they physically are HREmembers).
			if (third->getProcessedData().isHREmember || third->getProcessedData().isHREmperor)
				continue;

			// Throw both owner and its subjects under the overlord.
			if (!mergedOwnerSubjects.contains(overlord))
				mergedOwnerSubjects.emplace(overlord, std::set<std::string>{});
			mergedOwnerSubjects.at(overlord).emplace(owner);
			mergedOwnerSubjects.at(overlord).insert(subjects.begin(), subjects.end());
		}
		else
		{
			mergedOwnerSubjects.emplace(owner, subjects);
		}
		// We'll be retaining the overlord's startDate of grabbing the original subject regardless of the subject's dates.
	}

	// Now throw everything into blocs.
	for (const auto& [overlord, subjects]: mergedOwnerSubjects)
	{
		std::string identity;
		std::string principle;
		std::string locSuffix;

		auto owner = countries.at(overlord);
		// Ignore dead people.
		if (owner->getSubStates().empty())
			continue;

		// Are we, perchance, a revolutionary target?
		if (owner->getSourceCountry()->isRevolutionary())
		{
			identity = "identity_ideological_union";
			principle = "principle_aggressive_coordination_2";
			locSuffix = generatePowerBlockSuffix(overlord, identity);
		}
		// Or are we a native or tribal and a GP?
		else if ((owner->getSourceCountry()->getGovernment() == "native" || owner->getSourceCountry()->getGovernment() == "tribal") && owner->getWasGP())
		{
			identity = "identity_military_treaty_organization";
			principle = "principle_defensive_cooperation_2";
			locSuffix = generatePowerBlockSuffix(overlord, identity);
		}
		// Monarchies to empires
		else if (owner->getSourceCountry()->getGovernment() == "monarchy" && owner->getWasGP())
		{
			identity = "identity_sovereign_empire";
			principle = "principle_vassalization_1";
			locSuffix = generatePowerBlockSuffix(overlord, identity);
		}
		// Theologies to religious
		else if (owner->getSourceCountry()->getGovernment() == "theocracy" && owner->getWasGP())
		{
			identity = "identity_religious";
			principle = "principle_divine_economics_1";
			locSuffix = generatePowerBlockSuffix(overlord, identity);
		}
		// Republics to trade_leagues
		else if (owner->getSourceCountry()->getGovernment() == "republic" && owner->getWasGP())
		{
			identity = "identity_trade_league";
			principle = "principle_internal_trade_1";
			locSuffix = generatePowerBlockSuffix(overlord, identity);
		}

		// Overlord's block just wasn't good enough to become an actual block. This is fine.
		if (identity.empty())
			continue;

		// Fix leftover blockers.
		bool bail = false;
		owner->leaveIsolationism();
		for (const auto& tag: subjects)
		{
			auto subject = countries.at(tag);
			subject->leaveIsolationism();
			// Ignore dead people.
			if (subject->getSubStates().empty())
				bail = true;
		}
		if (bail)
			continue;

		auto bloc = PowerBloc(overlord, subjects, identity, overlord + "_BLOC", principle, owner->getProcessedData().color, blocStartDates.at(overlord));

		// Let's briefly consider localization. In English only. Let's not even pretend we can dynamically generate bloc names for non-english.
		const std::set<std::string> knownVic3Localizations =
			 {"braz_por", "english", "french", "german", "japanese", "korean", "polish", "russian", "simp_chinese", "spanish", "turkish"};

		for (const auto& locLanguage: knownVic3Localizations)
		{
			bloc.localizations.emplace(locLanguage, owner->getAdjective("english") + " " + locSuffix);
		}

		powerBlocs.emplace_back(bloc);
	}
	Log(LogLevel::Info) << "<> Crafted " << powerBlocs.size() << " power blocs.";
	// Onto HRE.
}

std::string V3::PoliticalManager::generatePowerBlockSuffix(const std::string& tag, const std::string& identity)
{
	std::vector<std::string> empireNames = {"Empire", "Imperium", "United Front", "Association", "Dominion", "Imperial Circle", "Grand Coalition", "Expanse"};
	std::vector<std::string> theocracyNames = {"Holy League", "Union", "Virtuous League", "Crusade", "League of the Faithful", "Harmony"};
	std::vector<std::string> republicanNames = {"Syndicate", "Convention", "Trade Pact", "Customs Group", "Financial Bloc", "Import Export"};
	std::vector<std::string> defensiveNames = {"Coalition", "Response Force", "Preventionist League", "Defensive Alliance", "Strategic Confederation"};
	std::vector<std::string> ideologicalNames = {"League for Truth", "Congress", "Normalization Pact", "Pacification League", "Convention"};
	std::string locSuffix;

	int pick = tag[0] * 23 + tag[1] * 13 + tag[2] * 7 + 11; // Why the hell not. I will die on this hill.

	if (identity == "identity_sovereign_empire")
		return empireNames[pick % static_cast<int>(empireNames.size())];
	if (identity == "identity_religious")
		return theocracyNames[pick % static_cast<int>(theocracyNames.size())];
	if (identity == "identity_trade_league")
		return republicanNames[pick % static_cast<int>(republicanNames.size())];
	if (identity == "identity_military_treaty_organization")
		return defensiveNames[pick % static_cast<int>(defensiveNames.size())];
	if (identity == "identity_ideological_union")
		return ideologicalNames[pick % static_cast<int>(ideologicalNames.size())];

	return "Joint Mass-Confederate Coalition of Force";
}

void V3::PoliticalManager::createHREPowerBloc(bool HREexists, bool decentralized)
{
	Log(LogLevel::Info) << "-> Creating HREBloc.";

	if (!HREexists)
	{
		Log(LogLevel::Info) << "<> No HREmperor detected.";
		return;
	}

	std::set<std::string> members;
	std::string emperor;

	for (const auto& [tag, country]: countries)
	{
		// Ignore dead people.
		if (country->getSubStates().empty())
			continue;

		if (country->getProcessedData().isHREmperor)
		{
			emperor = tag;
			continue;
		}
		if (country->getProcessedData().isHREmember)
		{
			members.emplace(tag);
		}
	}

	if (members.empty())
	{
		Log(LogLevel::Info) << "<> No HRE members means no HRE.";
		return;
	}

	if (emperor.empty())
	{
		Log(LogLevel::Warning) << "<> HRE exists but no emperor? Bailing!";
		return;
	}

	std::string identity;
	std::string principle;
	if (decentralized)
	{
		identity = "identity_military_treaty_organization";
		principle = "principle_defensive_cooperation_2";
	}
	else
	{
		identity = "identity_sovereign_empire";
		principle = "principle_vassalization_2";
	}

	powerBlocs.emplace_back(
		 PowerBloc(emperor, members, identity, "HOLY_ROMAN_EMPIRE_BLOC", principle, commonItems::Color(std::array<int, 3>{150, 177, 161}), date("962.2.2")));

	Log(LogLevel::Info) << "<> HREBloc created, " << members.size() << " members, emperor " << emperor << ".";
}

void V3::PoliticalManager::convertRivals()
{
	Log(LogLevel::Info) << "-> Transcribing rivalries.";
	auto counter = 0;

	for (const auto& [tag, country]: countries)
	{
		if (!country->getSourceCountry())
			continue;
		if (country->getSubStates().empty())
			continue;
		const auto& eu4Rivals = country->getSourceCountry()->getRivals();
		std::set<std::string> newRivals;
		for (const auto& rival: eu4Rivals)
		{
			if (!isEU4CountryConvertedAndLanded(rival))
				continue;

			const auto& rivalTag = countryMapper->getV3Tag(rival);
			const auto& rivalCountry = countries.at(*rivalTag);
			if (rivalCountry->getSubStates().empty())
				continue;

			auto& r1 = country->getRelation(*rivalTag);
			auto& r2 = rivalCountry->getRelation(tag);

			r1.increaseRelations(-100);
			r2.increaseRelations(-100);

			// only create an actual rivalry if it's a same-westernization country, or it'll autobreak at game start.
			if (country->getProcessedData().westernized == rivalCountry->getProcessedData().westernized)
				newRivals.emplace(*rivalTag);
			++counter;
		}
		country->setRivals(newRivals); // rivals are one-way, but relations suck both ways.
	}
	Log(LogLevel::Info) << "<> Transcribed " << counter << " rivalries.";
}

void V3::PoliticalManager::convertTruces(const date& lastEU4Date)
{
	Log(LogLevel::Info) << "-> Transcribing truces.";
	auto counter = 0;

	for (const auto& country: countries | std::views::values)
	{
		if (!country->getSourceCountry())
			continue;
		if (country->getSubStates().empty())
			continue;
		for (const auto& [target, relation]: country->getSourceCountry()->getRelations())
		{
			if (!relation.getTruceExpiry())
				continue;
			if (!isEU4CountryConvertedAndLanded(target))
				continue;
			const auto& targetTag = countryMapper->getV3Tag(target);

			auto nominalExpiry = *relation.getTruceExpiry();
			const int conversionDateDays = lastEU4Date.getYear() * 365 + lastEU4Date.getMonth() * 30 + lastEU4Date.getDay(); // approximating

			const auto remainingTruceDays = nominalExpiry.getYear() * 365 + nominalExpiry.getMonth() * 12 + nominalExpiry.getDay() - conversionDateDays;
			const auto remainingTruceMonths = static_cast<int>(std::round(static_cast<double>(remainingTruceDays) / 30.417)); // let's .. approximate.

			country->addTruce(*targetTag, remainingTruceMonths);
			counter++;
		}
	}
	Log(LogLevel::Info) << "<> Transcribed " << counter << " truces.";
}

bool V3::PoliticalManager::isEU4CountryConvertedAndLanded(const std::string& eu4Tag) const
{
	const auto& targetTag = countryMapper->getV3Tag(eu4Tag);
	if (!targetTag)
		return false;
	if (!countries.contains(*targetTag))
		return false;
	const auto& targetCountry = countries.at(*targetTag);
	if (targetCountry->getSubStates().empty())
		return false;
	return true;
}

void V3::PoliticalManager::convertCharacters(const date& conversionDate,
	 Configuration::STARTDATE startDate,
	 const ClayManager& clayManager,
	 mappers::CultureMapper& cultureMapper,
	 const mappers::ReligionMapper& religionMapper,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader)
{
	Log(LogLevel::Info) << "-> Importing Characters.";
	auto counter = 0;

	float ageShift = 0; // how much do we shift the character's ages forward? We don't want a bunch of 200-year olds.
	if (startDate == Configuration::STARTDATE::Vanilla)
		ageShift = date("1836.1.1").diffInYears(conversionDate);

	for (const auto& country: countries | std::views::values)
	{
		if (!TechValues::isValidCountryForTechConversion(*country))
			continue;
		country->convertCharacters(characterTraitMapper, ageShift, clayManager, cultureMapper, religionMapper, cultureLoader, religionLoader, conversionDate);
		counter += static_cast<int>(country->getProcessedData().characters.size());
	}
	Log(LogLevel::Info) << "<> Imported " << counter << " Characters. Some died along the way. We buried those.";
	Log(LogLevel::Debug) << "You won't find the corpses.";
}

void V3::PoliticalManager::attemptColonialTagReplacement(const mappers::ColonialRegionMapper& colonialRegionMapper, const ClayManager& clayManager)
{
	Log(LogLevel::Info) << "-> Attempting colonial tag replacement.";

	std::map<std::string, std::string> tagsToUpdate; // new->old

	for (const auto& [tag, country]: countries)
	{
		if (!TechValues::isValidCountryForTechConversion(*country))
			continue;
		if (!country->getSourceCountry()->isColony())
			continue;

		// focus only on CXX countries. On vic3 side as well! We don't want to delete canonical vic3 countries!
		const auto& eu4tag = country->getSourceCountry()->getTag();
		if (!eu4tag.starts_with("C") || !countryMapper->tagIsDynamic(eu4tag) || !countryMapper->tagIsDynamic(tag))
			continue;

		if (const auto replacement = colonialTagMapper.matchColonialTag(*country, colonialRegionMapper, clayManager); replacement)
		{
			if (isValidForColonialReplacement(*replacement) && !tagsToUpdate.contains(*replacement))
				tagsToUpdate.emplace(*replacement, tag);
		}
	}

	for (const auto& [replacement, tag]: tagsToUpdate)
		changeTag(replacement, tag);

	Log(LogLevel::Info) << "<> Swapped " << tagsToUpdate.size() << " countries for colonial tags.";
}

bool V3::PoliticalManager::isValidForColonialReplacement(const std::string& tag) const
{
	if (!countries.contains(tag))
		return true;
	if (!countries.at(tag)->getSourceCountry())
		return true;
	return false;
}

void V3::PoliticalManager::changeTag(const std::string& replacement, const std::string& tag)
{
	if (countries.contains(replacement))
		countries[replacement] = countries[tag];
	else
		countries.emplace(replacement, countries.at(tag));
	countries.erase(tag);

	countries[replacement]->setTag(replacement);

	countryMapper->relink(countries.at(replacement)->getSourceCountry()->getTag(), tag, replacement);
}

void V3::PoliticalManager::importVNColonialDiplomacy(const ClayManager& clayManager)
{
	Log(LogLevel::Info) << "-> Importing VN Colonial Diplomacy";
	auto counter = 0;

	for (const auto& colonyRule: clayManager.getVNColonialMapper().getVNColonies())
	{
		if (colonyRule.getSubjects().empty())
			continue;
		const auto& keyProvince = colonyRule.getKeyProvince();
		const auto keyOwnerTag = clayManager.getProvinceOwnerTag(keyProvince);
		if (!keyOwnerTag || !countries.contains(*keyOwnerTag))
			continue;
		const auto& keyOwner = countries.at(*keyOwnerTag);

		for (const auto& targetTag: colonyRule.getSubjects())
		{
			if (!countries.contains(targetTag))
				continue;
			const auto& target = countries.at(targetTag);

			auto& r1 = keyOwner->getRelation(targetTag);
			auto& r2 = target->getRelation(*keyOwnerTag);
			r1.increaseRelations(50);
			r2.increaseRelations(50);

			Agreement newAgreement;
			newAgreement.first = *keyOwnerTag;
			newAgreement.second = targetTag;
			newAgreement.type = colonyRule.getSubjectType();
			agreements.push_back(newAgreement);
			++counter;
		}
	}

	Log(LogLevel::Info) << "<> Imported " << counter << " subject agreements.";
}

int V3::PoliticalManager::getWorldPopCount() const
{
	auto theCountries = std::views::values(countries);
	return getCountriesPopCount({theCountries.begin(), theCountries.end()});
}

int V3::PoliticalManager::getCountriesPopCount(std::vector<std::shared_ptr<Country>> theCountries)
{
	return std::accumulate(theCountries.begin(), theCountries.end(), 0, [](int sum, const auto& country) {
		return sum + country->getPopCount();
	});
}

void V3::PoliticalManager::importVanillaDiplomacy()
{
	Log(LogLevel::Info) << "-> Importing VN Vanilla Diplomacy";
	auto agrCounter = 0;
	auto relCounter = 0;
	auto truCounter = 0;

	for (const auto& agreement: vanillaDiplomacyLoader.getAgreementEntries())
	{
		if (!isVanillaCountryAndLanded(agreement.getSourceTag()))
			continue;
		if (!isVanillaCountryAndLanded(agreement.getTargetTag()))
			continue;
		Agreement newAgreement;
		newAgreement.first = agreement.getSourceTag();
		newAgreement.second = agreement.getTargetTag();
		newAgreement.type = agreement.getAgreementType();
		agreements.push_back(newAgreement);
		++agrCounter;
	}

	for (const auto& relation: vanillaDiplomacyLoader.getRelationEntries())
	{
		if (!isVanillaCountryAndLanded(relation.getSourceTag()))
			continue;
		if (!isVanillaCountryAndLanded(relation.getTargetTag()))
			continue;
		auto& rel = countries.at(relation.getSourceTag())->getRelation(relation.getTargetTag());
		rel.setRelations(relation.getRelationValue());
		++relCounter;
	}

	for (const auto& truce: vanillaDiplomacyLoader.getTruceEntries())
	{
		if (!isVanillaCountryAndLanded(truce.getSourceTag()))
			continue;
		if (!isVanillaCountryAndLanded(truce.getTargetTag()))
			continue;
		countries.at(truce.getSourceTag())->addTruce(truce.getTargetTag(), truce.getDuration());
		++truCounter;
	}

	Log(LogLevel::Info) << "<> Imported " << agrCounter << " vanilla agreements, " << relCounter << " vanilla relations and " << truCounter
							  << " vanilla truces.";
}

bool V3::PoliticalManager::isVanillaCountryAndLanded(const std::string& tag) const
{
	if (!countries.contains(tag))
		return false;
	const auto& country = countries.at(tag);
	if (country->getSourceCountry())
		return false;
	if (country->getSubStates().empty())
		return false;
	return true;
}

void V3::PoliticalManager::injectDynamicCulturesIntoFormables(const mappers::CultureMapper& cultureMapper)
{
	Log(LogLevel::Info) << "-> Injecting dynamic cultures into formables.";
	auto counter = 0;
	const auto& related = cultureMapper.getRelatedCultures();

	// The difference between formables and releasables, is that formables never existed. No cores.

	for (const auto& country: countries | std::views::values)
	{
		if (!country->getSubStates().empty())
			continue; // only need dead people.
		if (!country->getUnownedCoreSubStates().empty())
			continue; // without cores.

		bool updated = false;
		auto data = country->getProcessedData();
		for (const auto& culture: country->getProcessedData().cultures)
		{
			if (!related.contains(culture))
				continue;
			data.cultures.insert(related.at(culture).begin(), related.at(culture).end());
			updated = true;
		}
		if (updated)
		{
			++counter;
			country->setProcessedData(data);
		}
	}
	Log(LogLevel::Info) << "<> Updated " << counter << " formable countries.";
}

void V3::PoliticalManager::expandReleasablesFootprint(const ClayManager& clayManager)
{
	Log(LogLevel::Info) << "-> Expanding Releasables footprint.";
	auto counter = 0;

	// For releasables that have cores on >60% state provinces, we want to release entire state, not just the provinces they owned.
	// Only reason for this is cleaner borders.

	for (const auto& country: countries | std::views::values)
	{
		bool expanded = false;
		// releasables own no states but have unowned cores somewhere.
		if (!country->getSubStates().empty() || country->getUnownedCoreSubStates().empty())
			continue;
		std::map<std::string, std::set<std::string>> stateProvinces;
		for (const auto& subState: country->getUnownedCoreSubStates())
		{
			if (!stateProvinces.contains(subState->getHomeStateName()))
				stateProvinces.emplace(subState->getHomeStateName(), std::set<std::string>{});
			auto stateProvinceIDs = subState->getProvinceIDs();
			stateProvinces.at(subState->getHomeStateName()).insert(stateProvinceIDs.begin(), stateProvinceIDs.end());
		}

		for (const auto& [stateName, provinces]: stateProvinces)
		{
			auto allProvinceIDs = clayManager.getStateProvinceIDs(stateName);
			if (static_cast<double>(provinces.size()) / static_cast<double>(allProvinceIDs.size()) > 0.6)
			{
				stateProvinces.at(stateName) = allProvinceIDs;
				expanded = true;
			}
		}
		for (const auto& provinces: stateProvinces | std::views::values)
			country->addUnownedProvinces(provinces);
		if (expanded)
			++counter;
	}
	Log(LogLevel::Info) << "<> Expanded " << counter << " releasables.";
}

void V3::PoliticalManager::alterReleasablesReligion()
{
	Log(LogLevel::Info) << "-> Altering Releasables state religions.";
	auto counter = 0;

	// Releasables are dead. We look at their core territories, make a census of their primary cultures' religions there,
	// and pick the dominant one.
	for (const auto& country: countries | std::views::values)
	{
		// releasables own no states but have unowned cores somewhere.
		if (!country->getSubStates().empty() || country->getUnownedCoreSubStates().empty())
			continue;

		// the census.
		const auto& stateCultures = country->getProcessedData().cultures;
		std::map<std::string, int> religionCounts;
		for (const auto& subState: country->getUnownedCoreSubStates())
			for (const auto& pop: subState->getSubStatePops().getPops())
				if (stateCultures.contains(pop.getCulture()) && !pop.getReligion().empty())
					religionCounts[pop.getReligion()] += pop.getSize();

		const auto sortedReligions = sortMap(religionCounts);
		if (sortedReligions.empty()) // possible with 0 pops, but bad. Not our problem, this is legitimate under "don't delete dead nations".
			continue;

		if (*sortedReligions.begin() == country->getProcessedData().religion)
			continue;

		country->setReligion(*sortedReligions.begin());
		++counter;
	}
	Log(LogLevel::Info) << "<> Updated religion for " << counter << " releasables.";
}

void V3::PoliticalManager::loadMajorFormables(const std::string& filePath)
{
	majorFormablesLoader.loadMajorFormables(filePath);
}

void V3::PoliticalManager::alterMajorFormables()
{
	Log(LogLevel::Info) << "-> Altering Major Formables Requirements.";
	auto counter = 0;

	auto formables = majorFormablesLoader.getMajorFormables();
	for (auto& [tag, formable]: formables)
	{
		if (!countries.contains(tag))
			continue;
		const auto& cultures = countries.at(tag)->getProcessedData().cultures;
		bool altered = false;
		for (const auto& culture: cultures)
			if (!formable.getCultures().contains(culture))
			{
				altered = true;
				formable.addCulture(culture);
			}
		if (altered)
			++counter;
	}
	majorFormablesLoader.setFormables(formables);
	Log(LogLevel::Info) << "<> Altered " << counter << " major formables.";
}

void V3::PoliticalManager::loadIGIdeologiesMapperRules(const std::filesystem::path& filePath)
{
	igIdeologiesMapper.loadMappingRules(filePath);
}

void V3::PoliticalManager::alterIGIdeologies(const mappers::CultureMapper& cultureMapper,
	 const mappers::ReligionMapper& religionMapper,
	 const ClayManager& clayManager)
{
	Log(LogLevel::Info) << "-> Altering IG Ideologies.";
	auto counter = 0;
	for (const auto& country: countries | std::views::values)
	{
		const auto igMods = igIdeologiesMapper.getIGIdeologyMods(*country, cultureMapper, religionMapper, clayManager);
		if (!igMods.empty())
		{
			++counter;
			country->setIGIdeologyModifiers(igMods);
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " countries had IGs altered.";
}

void V3::PoliticalManager::generateAIStrategies(const ClayManager& clayManager)
{
	Log(LogLevel::Info) << "-> Generating AI Strategies.";
	size_t counter = 0;

	for (const auto& country: countries | std::views::values)
	{
		if (!country->getSourceCountry() || country->getProcessedData().type == "decentralized" || country->getSubStates().empty())
			continue;

		country->setAdmStrategies(aiStrategyMapper.getAdmStrategies(*country, clayManager));
		country->setDipStrategies(aiStrategyMapper.getDipStrategies(*country, clayManager));
		country->setPolStrategies(aiStrategyMapper.getPolStrategies(*country, clayManager));

		counter += country->getProcessedData().admStrategies.size() + country->getProcessedData().dipStrategies.size() +
					  country->getProcessedData().polStrategies.size();
	}
	Log(LogLevel::Info) << "<> Generated a total of " << counter << " strategies.";
}

void V3::PoliticalManager::generateAISecretGoals(const ClayManager& clayManager)
{
	Log(LogLevel::Info) << "-> Generating AI Secret Goals.";
	auto counter = 0;

	std::vector<std::shared_ptr<Country>> neededCountries;

	for (const auto& country: countries | std::views::values)
	{
		if (!country->getSourceCountry() || country->getProcessedData().type == "decentralized" || country->getSubStates().empty())
			continue;
		neededCountries.emplace_back(country);
	}

	// O(n^2), but... no other way to do it.
	for (const auto& country: neededCountries)
		for (const auto& target: neededCountries)
		{
			if (country->getTag() == target->getTag())
				continue;
			if (const auto goal = aiSecretGoalMapper.matchSecretGoal(*country, *target, clayManager); goal)
			{
				country->addGoal(target->getTag(), *goal);
				++counter;
			}
		}

	Log(LogLevel::Info) << "<> Generated a total of " << counter << " secret goals.";
}

void V3::PoliticalManager::incorporateStates(const mappers::CultureMapper& cultureMapper, const ClayManager& clayManager)
{
	Log(LogLevel::Info) << "-> Incorporating States.";

	auto incorporated = 0;
	auto unIncorporated = 0;

	for (const auto& country: countries | std::views::values)
	{
		const auto capitalState = country->getProcessedData().capitalStateName;
		const auto capitalRegionName = clayManager.getParentRegionName(capitalState);

		for (const auto& subState: country->getSubStates())
		{
			// If the state is in same region as capital, incorporate.

			if (capitalRegionName)
			{
				const auto actualRegionName = clayManager.getParentRegionName(subState->getHomeStateName());
				if (*actualRegionName == *capitalRegionName)
				{
					subState->setIncorporated(true);
					++incorporated;
					continue;
				}
			}

			// Otherwise check if any of its homelands are not discriminated. If there's at least some accepted people (via homeland status), incorporate.

			bool match = false;
			for (const auto& culture: subState->getHomeState()->getHomelands())
			{
				if (!country->isCultureDiscriminated(culture, cultureMapper))
				{
					subState->setIncorporated(true);
					match = true;
					++incorporated;
					break;
				}
			}
			if (!match)
			{
				subState->setIncorporated(false);
				++unIncorporated;
			}
		}
	}

	Log(LogLevel::Info) << "<> Incorporated " << incorporated << " states, " << unIncorporated << " are unincorporated.";
}

void V3::PoliticalManager::designateTreatyPorts(const ClayManager& clayManager)
{
	Log(LogLevel::Info) << "-> Designating Treaty Ports and Vacating Pops.";

	auto count = 0;

	for (const auto& country: countries | std::views::values)
	{
		const auto capitalState = country->getProcessedData().capitalStateName;
		const auto capitalRegionName = clayManager.getParentRegionName(capitalState);

		for (const auto& subState: country->getSubStates())
		{
			// If the state is in same region as capital, skip!

			if (capitalRegionName)
			{
				const auto actualRegionName = clayManager.getParentRegionName(subState->getHomeStateName());
				if (*actualRegionName == *capitalRegionName)
				{
					continue;
				}
			}

			// Otherwise check if it's a single-province state and a named port.

			if (subState->getProvinces().size() != 1)
				continue;

			const auto& theProvince = subState->getProvinces().begin()->second;

			if (theProvince->isPort())
			{
				subState->setTreatyPort();
				subState->vacateTreatyPortPops();
				++count;
			}
		}
	}

	Log(LogLevel::Info) << "<> Designated " << count << " treaty ports.";
}

void V3::PoliticalManager::distributeColonialClaims(const ClayManager& clayManager)
{
	Log(LogLevel::Info) << "-> Distributing Colonial Claims.";
	auto colonialCounter = 0;
	auto destinyCounter = 0;

	// sort all states with decentralized nations in them into a registry.
	std::map<std::string, std::vector<std::shared_ptr<SubState>>> colonizableStates;
	// And also all substates in a colonial region regardless of ownership.
	std::map<std::string, std::vector<std::shared_ptr<SubState>>> manifestDestinyRegions;

	for (const auto& [stateName, state]: clayManager.getStates())
	{
		for (const auto& subState: state->getSubStates())
		{
			if (subState->getOwner()->getProcessedData().type == "decentralized")
			{

				if (!colonizableStates.contains(stateName))
					colonizableStates.emplace(stateName, std::vector<std::shared_ptr<SubState>>{});
				colonizableStates.at(stateName).emplace_back(subState);
			}

			if (const auto& regionName = clayManager.getParentRegionName(subState->getHomeStateName()); regionName)
			{
				if (!manifestDestinyRegions.contains(*regionName))
					manifestDestinyRegions.emplace(*regionName, std::vector<std::shared_ptr<SubState>>{});
				manifestDestinyRegions.at(*regionName).emplace_back(subState);
			}
		}
	}

	// Now, for all countries, that are colonial, if they have a state (presence) in one of these states, claim all the uncolonized substates in the region.
	// Also, for capital location, claim all substates in capital region.

	for (const auto& country: countries | std::views::values)
	{
		if (country->getProcessedData().type == "colonial") // this affects both indeps and deps, no worries.
		{
			std::set<std::string> statesToClaim;
			for (const auto& subState: country->getSubStates())
				statesToClaim.emplace(subState->getHomeStateName());

			const auto& capitalRegion = clayManager.getParentRegionName(country->getProcessedData().capitalStateName);

			// now for all subStates in that state, that contain uncolonized clay (in the registry above), file a claim.

			for (const auto& stateName: statesToClaim)
			{
				if (colonizableStates.contains(stateName))
				{
					for (const auto& subState: colonizableStates.at(stateName))
					{
						subState->addClaim(country->getTag());
						++colonialCounter;
					}
				}

				if (const auto& regionName = clayManager.getParentRegionName(stateName); regionName)
				{
					if (capitalRegion && *regionName == *capitalRegion)
					{
						for (const auto& subState: manifestDestinyRegions.at(*regionName))
						{
							subState->addClaim(country->getTag());
							++destinyCounter;
						}
					}
				}
			}
		}
	}

	Log(LogLevel::Info) << "<> Distributed " << colonialCounter << " colonial claims and " << destinyCounter << " manifest destiny claims.";
}
