#include "EconomyManager.h"
#include "ClayManager/State/SubState.h"
#include "EU4World/CountryManager/EU4Country.h"
#include "Loaders/TerrainLoader/TerrainModifierLoader.h"
#include "Log.h"
#include "PoliticalManager/Country/Country.h"
#include <cmath>
#include <iomanip>
#include <numeric>
#include <ranges>

#include "PoliticalManager/PoliticalManager.h"

void V3::EconomyManager::loadCentralizedCountries(const std::map<std::string, std::shared_ptr<Country>>& countries)
{
	auto selectCentralized = [](std::shared_ptr<Country> country) {
		return country && country->getProcessedData().type != "decentralized";
	};

	for (const auto& country: std::ranges::filter_view(std::views::values(countries), selectCentralized))
	{
		centralizedCountries.push_back(country);
	}
}

void V3::EconomyManager::assignCountryCPBudgets(const Configuration::ECONOMY economyType, const PoliticalManager& politicalManager) const
{
	// Some global value of CP to spend, calibrate to Vanilla.
	double globalCP = 1208050;
	// TODO(Gawquon): adjust based on date
	globalCP *= 1;
	// adjust based on amount of world centralized by population, calibrated to Vanilla
	const double centralizedPopRatio = static_cast<double>(getCentralizedWorldPopCount()) / politicalManager.getWorldPopCount();
	const double globalPopFactor = centralizedPopRatio / .975;

	Log(LogLevel::Info) << std::fixed << std::setprecision(0) << "<> The world is " << centralizedPopRatio * 100
							  << "% Centralized by population. Adjusting global CP values by: " << globalPopFactor * 100 << "%";

	double totalIndustryScore = 0;

	// config option 1 the default way. Pop & culture TechGroup
	if (economyType == Configuration::ECONOMY::TechGroup)
	{
		const double geoMeanPop = calculateGeoMeanCentralizedPops();

		// while determining individual country's industry score, accumulate total industry score & factor
		double totalIndustryFactor = 0;
		for (const auto& country: centralizedCountries)
		{
			const int popCount = country->getPopCount();
			country->setIndustryScore(popCount * country->getIndustryFactor() * calculatePopDistanceFactor(popCount, geoMeanPop));
			totalIndustryScore += country->getIndustryScore();
			totalIndustryFactor += country->getIndustryFactor();
		}

		// adjust global total by average industry factor compared to baseline
		const double globalIndustryFactor = totalIndustryFactor / static_cast<double>(centralizedCountries.size()) / 0.8;
		globalCP *= (globalIndustryFactor + globalPopFactor);

		Log(LogLevel::Info) << std::fixed << std::setprecision(0) << "<> The world is " << globalIndustryFactor * 100
								  << "% Developed compared to baseline. Compensating";
	}

	// distribute each country its budget
	Log(LogLevel::Info) << std::fixed << std::setprecision(0) << "<> The world has " << globalCP << " to spend on industry.";
	distributeBudget(globalCP, totalIndustryScore);

	/*
	// config option 2. Pop & development
	if (economyType == Configuration::ECONOMY::DevBased)
	{
		// The more pop you have per dev, the less powerful your development
		// This is loosely assuming Dev = Pop + Economy so Economy = Dev - Pop

		// while determining individual country's industry score, accumulate total industry score & factor
		double totalIndustryScore = 0;
		double totalIndustryFactor = 0;
		for (const auto& country: centralizedCountries)
		{
			country->setIndustryScore(country->getPopCount() * 1);
			totalIndustryScore += country->getIndustryScore();
			totalIndustryFactor += country->getIndustryFactor();
		}

		globalCP *= globalPopFactor;
	}
	*/
}

void V3::EconomyManager::loadTerrainModifierMatrices()
{
	// Config file, loads in a mapping to the class data
	// Something where desert = priority 0.7 and desert = glassworks 1.2
	// So a country invests less in a state with desert
	// But also tends to build more glassworks than other factories in the desert

	TerrainModifierLoader terrainModifierLoader;
	terrainModifierLoader.loadTerrainModifiers("configurables/terrain_econ_modifiers.txt");

	stateTerrainModifiers = terrainModifierLoader.getTerrainStateModifiers();
	buildingTerrainModifiers = terrainModifierLoader.getTerrainBuildingModifiers();
}

void V3::EconomyManager::assignSubStateCPBudgets(const Configuration::ECONOMY economyType) const
{
	// Distribute CP budget from a country to its substates
	for (const auto& country: centralizedCountries)
	{
		if (economyType != Configuration::ECONOMY::DevBased)
		{
			// Score is based on population and adjusted by state modifiers/terrain
		}
		else
		{
			// Score is based on Dev, penalized by population and adjusted by state modifiers/terrain
		}
	}
}

void V3::EconomyManager::balanceNationalBudgets() const
{
	// The Big Kahuna, where politics and economy collide
	// Implement last
	// Anything that effects what type of economy a country should build goes here, manufacturies, interest groups, all of it.

	// End result is each country has a set of instructions on what buildings have priority in its economy
}

void V3::EconomyManager::buildBuildings() const
{
	// Each substate consults its country's national budget and spends the substates CP budget
	// Substate modifies the building priorities by local terrain and state modifiers and maybe EU4 manufacturies?
	for (const auto& country: centralizedCountries)
	{
		for (const auto& substate: country->getSubStates())
		{
			// while(substate->getCPBudget() > 0)
			{
				// if(infrastructureCost > infrastructure)
				//     try to make more infra
				// if (can't make more infra)
				//    exit early
			}
		}
	}
}

int V3::EconomyManager::getCentralizedWorldPopCount() const
{
	return std::accumulate(centralizedCountries.begin(), centralizedCountries.end(), 0, [](int sum, const auto& country) {
		return sum + country->getPopCount();
	});
}

double V3::EconomyManager::calculatePopDistanceFactor(const int countryPopulation, const double geoMeanPopulation)
{
	if (const double popPercent = geoMeanPopulation / countryPopulation; popPercent >= 1)
	{
		return log(popPercent) + 1;
	}
	else
	{
		return log(popPercent + 1) + 0.7;
	}
}

double V3::EconomyManager::calculateGeoMeanCentralizedPops() const
{
	double sum = 0;

	for (const auto& country: centralizedCountries)
	{
		sum += log(country->getPopCount());
	}

	sum /= static_cast<double>(centralizedCountries.size());

	return exp(sum);
}

void V3::EconomyManager::distributeBudget(const double globalCP, const double totalIndustryScore) const
{
	for (const auto& country: centralizedCountries)
	{
		country->setCPBudget(static_cast<int>(std::round(globalCP * (country->getIndustryScore() / totalIndustryScore))));
	}
}