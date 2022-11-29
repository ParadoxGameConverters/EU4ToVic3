#include "PoliticalManager.h"
#include "ClayManager/ClayManager.h"
#include "CountryDefinitionLoader/CountryDefinitionLoader.h"
#include "Log.h"
#include "PopManager/PopManager.h"
#include <ranges>

void V3::PoliticalManager::initializeVanillaCountries(const std::string& V3Path)
{
	Log(LogLevel::Info) << "-> Loading Vanilla Countries.";

	CountryDefinitionLoader definitionLoader;
	definitionLoader.loadCommonCountries(V3Path);
	countries = definitionLoader.getCountries();

	Log(LogLevel::Info) << "<> " << countries.size() << " vanilla countries loaded.";
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
		country->initializeFromEU4Country();
	}

	Log(LogLevel::Info) << "<> Vic3 now has " << countries.size() << " known countries.";
}

void V3::PoliticalManager::importVanillaCountries()
{
	for (const auto& country: countries | std::views::values)
		if (country->getVanillaData() && !country->getSourceCountry()) // this is a vic3-only country.
			country->copyVanillaData();
}

void V3::PoliticalManager::generateDecentralizedCountries(const ClayManager& clayManager, const PopManager& popManager)
{
	// How do we generate decentralized states? We look at states and their unowned substates.
	// Those substates with a particular culture are banded together and shoved into a new country.
	// If there's no demographic available (mapping from a wasteland or out of scope), we load
	// cultural information (not demographics!) from vanilla sources.

	for (const auto& [culture, subStates]: sortSubStatesByCultures(clayManager, popManager))
	{
		auto v3tag = countryMapper->requestNewV3Tag();
		auto newCountry = std::make_shared<Country>();
		newCountry->setTag(v3tag);
		newCountry->setSubStates(subStates);
		countries.emplace(v3tag, newCountry);
		for (const auto& substate: newCountry->getSubStates())
			substate->setOwner(newCountry);
		Log(LogLevel::Debug) << "Generated new country for culture: " << culture << " from " << subStates.size() << " substates.";
		// ... fingers crossed?
	}
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
			cultureScores.at(demo.culture) += demo.upperRatio; // using upper ratio to represent nobility sway.
		else
			cultureScores.emplace(demo.culture, demo.upperRatio);
	}

	const auto highest = std::max_element(std::begin(cultureScores), std::end(cultureScores), [](const auto& p1, const auto& p2) {
		return p1.second < p2.second;
	});

	return highest->first;
}
