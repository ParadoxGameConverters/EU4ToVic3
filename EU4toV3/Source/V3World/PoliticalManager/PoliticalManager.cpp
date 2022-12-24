#include "PoliticalManager.h"
#include "ClayManager/ClayManager.h"
#include "ClayManager/State/State.h"
#include "ClayManager/State/SubState.h"
#include "Country/Country.h"
#include "CountryManager/EU4Country.h"
#include "Loaders/CountryDefinitionLoader/CountryDefinitionLoader.h"
#include "Log.h"
#include "Mappers/CountryMapper/CountryMapper.h"
#include "PopManager/PopManager.h"
#include <numeric>
#include <ranges>

void V3::PoliticalManager::initializeVanillaCountries(const commonItems::ModFilesystem& modFS)
{
	Log(LogLevel::Info) << "-> Loading Vanilla Countries.";

	CountryDefinitionLoader definitionLoader;
	definitionLoader.loadCommonCountries(modFS);
	countries = definitionLoader.getCountries();

	Log(LogLevel::Info) << "<> " << countries.size() << " vanilla countries loaded.";
}

void V3::PoliticalManager::loadCountryMapper(const std::shared_ptr<mappers::CountryMapper>& theCountryMapper)
{
	countryMapper = theCountryMapper;
	for (const auto& countryTag: countries | std::views::keys)
		countryMapper->registerKnownVanillaV3Tag(countryTag);
}

void V3::PoliticalManager::loadPopulationSetupMapperRules(const std::string& filePath)
{
	populationSetupMapper.loadMappingRules(filePath);
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
				leadCulture = popManager.getDominantVanillaCulture(stateName);
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
	 const EU4::EU4LocalizationLoader& eu4LocLoader) const
{
	Log(LogLevel::Info) << "-> Converting countries.";

	for (const auto& [tag, country]: countries)
	{
		// this is a freshly-generated decentralized country with no source data whatsoever.
		if (!country->getVanillaData() && country->getProcessedData().type == "decentralized")
			country->generateDecentralizedData(v3LocLoader, eu4LocLoader);

		// this is a vic3-only (vanilla) country with no EU4 match. It's likely extinct.
		else if (country->getVanillaData() && !country->getSourceCountry())
			country->copyVanillaData(v3LocLoader, eu4LocLoader);

		// otherwise, this is a regular imported EU4 country
		else if (country->getSourceCountry())
			country->convertFromEU4Country(clayManager, cultureMapper, religionMapper, cultureLoader, religionLoader);

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
	if (country->getVanillaData() && country->getVanillaData()->type == "decentralized")
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
		const auto totalTechs = sourceCountry->getMilTech() + sourceCountry->getAdmTech() + sourceCountry->getDipTech();
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
		country
			 ->determineWesternizationWealthAndLiteracy(topTech, topInstitutions, cultureMapper, religionMapper, eurocentrism, datingData, populationSetupMapper);

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
