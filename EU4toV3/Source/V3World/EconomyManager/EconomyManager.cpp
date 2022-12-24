#include "EconomyManager.h"
#include "ClayManager/State/State.h"
#include "ClayManager/State/StateModifier.h"
#include "ClayManager/State/SubState.h"
#include "EU4World/CountryManager/EU4Country.h"
#include "Loaders/TerrainLoader/TerrainModifierLoader.h"
#include "Log.h"
#include "PoliticalManager/Country/Country.h"
#include "PoliticalManager/PoliticalManager.h"
#include <cmath>
#include <iomanip>
#include <ranges>

void V3::EconomyManager::loadCentralizedStates(const std::map<std::string, std::shared_ptr<Country>>& countries)
{
	for (const auto& country: std::views::values(countries))
	{
		if (!country)
			continue;
		if (country->getProcessedData().type == "decentralized")
			continue;

		centralizedCountries.push_back(country);
	}
}

void V3::EconomyManager::assignCountryCPBudgets(const Configuration::ECONOMY economyType, const PoliticalManager& politicalManager) const
{
	// Some global value of CP to spend, calibrate to Vanilla.
	double globalCP = 1208050;

	// TODO(Gawquon): adjust based on date
	double dateFactor = 0;

	// adjust based on amount of world centralized by population, calibrated to Vanilla
	const double centralizedPopRatio = static_cast<double>(PoliticalManager::getCountriesPopCount(centralizedCountries)) / politicalManager.getWorldPopCount();
	const double globalPopFactor = (centralizedPopRatio / ) - 1; // TODO(Gawquon) Clean up these ratios readability, maybe add some helpers

	Log(LogLevel::Info) << std::fixed << std::setprecision(0) << "<> The world is " << centralizedPopRatio * 100
							  << "% Centralized by population. Adjusting global CP values by: " << globalPopFactor * 100 << "%";

	double totalIndustryScore = 0;
	double specialFactors = 0;

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
		const double globalIndustryFactor = (totalIndustryFactor / static_cast<double>(centralizedCountries.size()) / 0.8) - 1;
		specialFactors = globalIndustryFactor;

		Log(LogLevel::Info) << std::fixed << std::setprecision(0) << "<> The world is " << (globalIndustryFactor + 1) * 100
								  << "% industrial compared to baseline. Compensating";
	}


	// distribute each country its budget
	globalCP *= (1 + dateFactor + globalPopFactor + specialFactors);
	Log(LogLevel::Info) << std::fixed << std::setprecision(0) << "<> The world has " << globalCP << " CP to spend on industry.";
	distributeBudget(globalCP, totalIndustryScore);

	/*
	// TODO(Gawquon): Dev based
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

		globalCP *= (1 + globalPopFactor);
	}
	*/
}

void V3::EconomyManager::loadTerrainModifierMatrices()
{
	TerrainModifierLoader terrainModifierLoader;
	terrainModifierLoader.loadTerrainModifiers("configurables/terrain_econ_modifiers.txt");

	stateTerrainModifiers = terrainModifierLoader.getTerrainStateModifiers();
	buildingTerrainModifiers = terrainModifierLoader.getTerrainBuildingModifiers();
}

void V3::EconomyManager::assignSubStateCPBudgets(const Configuration::ECONOMY economyType,
	 const std::map<std::string, std::shared_ptr<StateModifier>>& stateTraits) const
{
	// Distribute CP budget from a country to its substates
	for (const auto& country: centralizedCountries)
	{
		double totalIndustryScore = 0.0; // accumulate
		for (const auto& subState: country->getSubStates())
		{
			if (!subState)
				continue;

			double base = 0;

			if (economyType == Configuration::ECONOMY::TechGroup)
			{
				// Score is based on population
				base = subState->getSubStatePops().getPopCount();
			}
			if (economyType == Configuration::ECONOMY::DevBased)
			{
				// TODO(Gawquon): Dev based
				// Score is based on Dev, penalized by population
				// base = subState->getSourceProvinceData() ;
			}

			// Adjust for terrain
			double terrainMultiplier = -1; // Config is based around 1.0 for human readability
			for (const auto& [terrain, frequency]: subState->getTerrainFrequencies())
			{
				if (!stateTerrainModifiers.contains(terrain))
				{
					if (terrain == "ocean" || terrain == "lake")
					{
						Log(LogLevel::Debug) << "Water terrain found in " << country->getTag() << "'s SubState " << subState->getHomeState()->getName()
													<< " WTF Paradox. Ignoring.";
					}
					else
					{
						Log(LogLevel::Warning) << "Unrecognized terrain, " << terrain << ", in " << country->getTag() << "'s SubState "
													  << subState->getHomeState()->getName() << ". Ignoring.";
					}
					continue;
				}
				terrainMultiplier += stateTerrainModifiers.at(terrain) * frequency;
			}

			// Adjust for state modifiers
			double stateTraitMultiplier = 0;
			for (const auto& trait: subState->getHomeState()->getTraits())
			{
				if (!stateTraits.contains(trait))
				{
					Log(LogLevel::Warning) << "Unrecognized state trait in " << subState->getHomeState()->getName() << ". Ignoring.";
					continue;
				}
				const auto& stateTrait = stateTraits.at(trait);

				// Throughput bonuses to goods, buildings or whole building groups factor in
				const double goodsModifiers = stateTrait->getAllBonuses(stateTrait->getGoodsModifiersMap());
				const double buildingsModifiers = stateTrait->getAllBonuses(stateTrait->getBuildingModifiersMap());
				const double buildingGroupsModifiers = stateTrait->getAllBonuses(stateTrait->getBuildingGroupModifiersMap());

				stateTraitMultiplier += goodsModifiers + buildingsModifiers + buildingGroupsModifiers;
			}

			subState->setIndustryScore(std::max(0.0, base * (1 + terrainMultiplier + stateTraitMultiplier)));
			totalIndustryScore += subState->getIndustryScore();
		}

		// Now back through substates, give each a percentage of country's budget based on percent of total industry score
		for (const auto& subState: country->getSubStates())
		{
			const double stateBudget = country->getCPBudget() * subState->getIndustryScore() / totalIndustryScore;
			subState->setCPBudget(static_cast<int>(std::round(stateBudget)));
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

double V3::EconomyManager::calculatePopDistanceFactor(const int countryPopulation, const double geoMeanPopulation)
{
	if (const double popPercent = geoMeanPopulation / countryPopulation; popPercent >= 1)
	{
		return log(popPercent) + 1;
	}
	else
	{
		return log1p(popPercent) + 0.7;
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

	Log(LogLevel::Debug) << "The Geometric Mean of all centralized countries population is : " << exp(sum);
	return exp(sum);
}

void V3::EconomyManager::distributeBudget(const double globalCP, const double totalIndustryScore) const
{
	for (const auto& country: centralizedCountries)
	{
		country->setCPBudget(static_cast<int>(std::round(globalCP * (country->getIndustryScore() / totalIndustryScore))));
	}
}