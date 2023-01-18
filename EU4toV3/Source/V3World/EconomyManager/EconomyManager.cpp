#include "EconomyManager.h"
#include "Building/Building.h"
#include "Building/BuildingGroups.h"
#include "Building/ProductionMethods/ProductionMethod.h"
#include "Building/ProductionMethods/ProductionMethodGroup.h"
#include "ClayManager/State/State.h"
#include "ClayManager/State/StateModifier.h"
#include "ClayManager/State/SubState.h"
#include "EU4World/CountryManager/EU4Country.h"
#include "Loaders/BuildingLoader/BuildingGroupLoader.h"
#include "Loaders/BuildingLoader/BuildingLoader.h"
#include "Loaders/BuildingLoader/ProductionMethodLoader/ProductionMethodGroupLoader.h"
#include "Loaders/BuildingLoader/ProductionMethodLoader/ProductionMethodLoader.h"
#include "Loaders/DefinesLoader/EconDefinesLoader.h"
#include "Loaders/StateModifierLoader/StateModifierLoader.h"
#include "Loaders/TerrainLoader/TerrainModifierLoader.h"
#include "Log.h"
#include "PoliticalManager/Country/Country.h"
#include "PoliticalManager/PoliticalManager.h"
#include <cmath>
#include <iomanip>
#include <numeric>
#include <ranges>

void V3::EconomyManager::loadCentralizedStates(const std::map<std::string, std::shared_ptr<Country>>& countries)
{
	for (const auto& country: std::views::values(countries))
	{
		if (!country)
			continue;
		if (country->getProcessedData().type == "decentralized")
			continue;
		if (country->getSubStates().empty())
			continue;

		centralizedCountries.push_back(country);
	}
}

void V3::EconomyManager::loadMappersAndConfigs(const commonItems::ModFilesystem& modFS, const std::string& filePath)
{
	// If anything else ends up needing these, pass them in from World instead of loading direct in Econ

	loadTerrainModifierMatrices(filePath);
	loadStateTraits(modFS);
	loadBuildingInformation(modFS);
	loadBuildingMappings(filePath);
	loadEconDefines(filePath);
	loadNationalBudgets(filePath);
}

void V3::EconomyManager::establishBureaucracy(const PoliticalManager& politicalManager) const
{
	if (!buildings.contains("building_government_administration"))
	{
		Log(LogLevel::Error) << "No building definition found for: building_government_administration.";
		return;
	}

	const auto& govAdmin = buildings.at("building_government_administration");

	for (const auto& country: centralizedCountries)
	{
		// Check tech requirement for government administrations.
		if (!country->hasAnyOfTech(govAdmin.getUnlockingTechs()))
		{
			continue;
		}

		// Give 5% extra for trade routes
		const double generationTarget = country->calculateBureaucracyUsage(politicalManager.getLawsMap()) * 1.05;

		// Use the PM with the most generation available
		int PMGeneration = 35;
		const auto& PMName = pickBureaucracyPM(country);
		if (PMs.contains(PMName))
		{
			PMGeneration = PMs.at(PMName).getBureaucracy();
		}

		// find # of buildings, 100 base value generation
		const int numAdmins = static_cast<int>(std::ceil((generationTarget - 100) / PMGeneration));

		country->distributeGovAdmins(numAdmins);
	}

	setPMs();
}

void V3::EconomyManager::hardcodePorts() const
{
	for (const auto& country: centralizedCountries)
	{
		for (const auto& substate: country->getSubStates())
		{
			if (!substate->getVanillaBuildingElements().empty())
				continue; // don't affect states imported from vanilla.
			if (substate->getHomeState()->isCoastal())
			{
				substate->setBuildingLevel("building_port", 1);
				substate->getOwner()->addTech("navigation");
			}
		}
	}
}

void V3::EconomyManager::assignCountryCPBudgets(const Configuration::ECONOMY economyType,
	 const Configuration::STARTDATE startDate,
	 const DatingData& dateData,
	 const PoliticalManager& politicalManager) const
{
	// Some global value of CP to spend
	double globalCP = econDefines.getGlobalCP();
	const double dateFactor = calculateDateFactor(startDate, dateData);
	const double globalPopFactor = calculateGlobalPopFactor(politicalManager); // Adjust based on amount of world centralized by population

	// Each country figures out its CP budget weight and reports back
	auto [totalIndustryWeight, specialFactors] = countryBudgetCalcs(economyType);

	// distribute each country its budget
	globalCP *= (1 + dateFactor + globalPopFactor + specialFactors);
	Log(LogLevel::Info) << std::fixed << std::setprecision(0) << "<> The world has " << globalCP << " CP to spend on industry.";
	distributeBudget(globalCP, totalIndustryWeight);
}

void V3::EconomyManager::assignSubStateCPBudgets(const Configuration::ECONOMY economyType) const
{
	// Distribute CP budget from a country to its substates
	for (const auto& country: centralizedCountries)
	{
		double totalIndustryWeight = 0.0; // accumulate
		for (const auto& subState: country->getSubStates())
		{
			if (!subState)
				continue;
			if (subState->getProvinces().empty())
				continue;

			const double base = getBaseSubStateWeight(subState, economyType);

			// Adjust for terrain & state traits
			const double terrainMultiplier = calculateTerrainMultiplier(subState);
			const double stateTraitMultiplier = calculateStateTraitMultiplier(subState);

			subState->setIndustryWeight(std::max(0.0, base * (1 + terrainMultiplier + stateTraitMultiplier)));
			totalIndustryWeight += subState->getIndustryWeight();
		}

		// Now back through substates, give each a percentage of country's budget based on percent of total industry score
		for (const auto& subState: country->getSubStates())
		{
			const double stateBudget = country->getCPBudget() * subState->getIndustryWeight() / totalIndustryWeight;
			subState->setCPBudget(static_cast<int>(std::round(stateBudget)));
		}
	}
}

void V3::EconomyManager::balanceNationalBudgets() const
{
	// TODO(Gawquon): Implement and add sector map to country processed data
	for (const auto& country: centralizedCountries)
	{
		double totalWeight = 0;

		for (const auto& blueprint: nationalBudgets.getSectorBlueprints())
		{
			// Add to country Sector Map. Sector name -> Sector
			// Accumulate totalWeight
		}

		// for (const auto& sector: country->getProcessedData().sectors | std::views::values)
		// {
		//	sector.calculateBudget(totalWeight, country->getCPBudget());
		// }
	}
}

void V3::EconomyManager::buildBuildings() const
{
	// The great negotiation
	// 1. The substate w/ the most CP asks to build it's highest scoring building
	// 2. The country checks if the sector that building belongs to has enough CP for at least 1 building
	// 2b. If not, the substate moves on to the next highest scoring building until it exhausts its list.
	// 2c. If a substate exhausts its building list without matching with a country's sector, it gets to ignore the country budget.
	// 2d. The CP comes out of the country's biggest sector
	// 3. The state then builds as many buildings of that kind as it can, limited by capacity, packet size and sector CP
	// 3b. capacity is RGO/arable land capacity
	// 3c. packet size is based on the mean amount of CP states have left to build and is configurable
	// 4. If a sector or substate end up with less CP than the cost for any possible valid building, they relinquish it to the next sector/substate

	for (const auto& country: centralizedCountries)
	{
		// Prepare sorting lambda, we will use it more than once
		auto greaterBudget = [](const std::shared_ptr<SubState>& lhs, const std::shared_ptr<SubState>& rhs) {
			return lhs->getCPBudget() > rhs->getCPBudget();
		};

		// Copy substate vector. We will be sorting this one and removing finished substates until it is empty
		// Sort Vector, remove substates that have less than the minimum pre-set construction cost, default to 50
		auto subStatesByBudget(country->getSubStates());
		std::ranges::sort(subStatesByBudget, greaterBudget);

		// When removing substates grant their extra CP to the top State

		while (!subStatesByBudget.empty())
		{
			// Enter negotiation
			// Pick the substate with the most budget
			const auto& substate = subStatesByBudget[0];


			// Copy Sectors?
			// while Sectors not empty
			// Pick highest scoring building
			// Country sees if it agrees

			// Spend
			// Chosen building construction cost
			substate->spendCPBudget(50);

			// Sort
			std::ranges::sort(subStatesByBudget, greaterBudget);


			// After spend, remove substate if now low budget
			removeSubStateIfFinished(subStatesByBudget, subStatesByBudget.end() - 1);
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

double V3::EconomyManager::calculateDateFactor(const Configuration::STARTDATE startDate, const DatingData& dateData)
{
	if (startDate == Configuration::STARTDATE::Dynamic)
	{
		const double factor = Country::yearCapFactor(dateData.lastEU4Date) - 1.0;
		LOG(LogLevel::Info) << std::fixed << std::setprecision(0) << "Altering global industry by " << factor * 100 << "% due to start date of "
								  << dateData.lastEU4Date << ".";
		return factor;
	}
	return 0.0;
}

std::pair<double, double> V3::EconomyManager::countryBudgetCalcs(const Configuration::ECONOMY economyType) const
{
	// Returns total weight, and any special multiplicative factors specific to the method.
	double totalWeight = 0;

	if (Configuration::ECONOMY::CivLevel == economyType)
		return {totalWeight, civlevelCountryBudgets(totalWeight)};
	if (Configuration::ECONOMY::DevPerPop == economyType)
		return {totalWeight, devCountryBudgets(totalWeight)};

	return {totalWeight, 0.0};
}

double V3::EconomyManager::civlevelCountryBudgets(double& accumulatedWeight) const
{
	// The default way.

	double totalCivLevel = 0.0;
	const double geoMeanPop = calculateGeoMeanCentralizedPops();
	// while determining individual country's industry score, accumulate total industry factor & weight

	for (const auto& country: centralizedCountries)
	{
		const int popCount = country->getPopCount();
		country->setIndustryWeight(popCount * (country->getProcessedData().civLevel / 100) * calculatePopDistanceFactor(popCount, geoMeanPop));
		accumulatedWeight += country->getIndustryWeight();
		totalCivLevel += country->getProcessedData().civLevel;
	}

	// adjust global total by average industry factor compared to baseline
	const double globalIndustryFactor = (totalCivLevel / static_cast<double>(centralizedCountries.size()) / econDefines.getMeanCivlevel()) - 1;

	Log(LogLevel::Info) << std::fixed << std::setprecision(0) << "<> The world is " << (globalIndustryFactor + 1) * 100
							  << "% industrial compared to baseline. Compensating";

	return globalIndustryFactor;
}

double V3::EconomyManager::devCountryBudgets(double& accumulatedWeight) const
{
	// TODO(Gawquon)
	// config option 3. Pop & development
	// The more pop you have per dev, the less powerful your development
	// This is loosely assuming Dev = Pop + Economy so Economy = Dev - Pop

	return 0.0;
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

std::string V3::EconomyManager::pickBureaucracyPM(const std::shared_ptr<Country>& country) const
{
	int generation = 0;
	std::string bestPMName = "pm_simple_organization";

	if (!PMGroups.contains("pmg_base_building_government_administration"))
	{
		Log(LogLevel::Error) << "pmg_base_building_government_administration: Not in loaded Production Method Groups";
		return bestPMName;
	}

	for (const auto& PMName: PMGroups.at("pmg_base_building_government_administration").getPMs())
	{
		if (!PMs.contains(PMName))
		{
			Log(LogLevel::Error) << PMName << ": Not in loaded Production Methods";
			return bestPMName;
		}

		const auto& PM = PMs.at(PMName);

		// Only use PMs we have unlocked
		if (!country->hasAnyOfTech(PM.getUnlockingTechs()))
		{
			continue;
		}

		// Update best if the PM has a higher bureaucracy value
		if (PM.getBureaucracy() > generation)
		{
			generation = PM.getBureaucracy();
			bestPMName = PM.getName();
		}
	}

	return bestPMName;
}

double V3::EconomyManager::calculateGlobalPopFactor(const PoliticalManager& politicalManager) const
{
	const double centralizedPopRatio = static_cast<double>(PoliticalManager::getCountriesPopCount(centralizedCountries)) / politicalManager.getWorldPopCount();
	const double globalPopFactor = centralizedPopRatio / econDefines.getCentralizedPopRatio() - 1;

	Log(LogLevel::Info) << std::fixed << std::setprecision(0) << "<> The world is " << centralizedPopRatio * 100
							  << "% Centralized by population. Adjusting global CP values by: " << globalPopFactor * 100 << "%";
	return globalPopFactor;
}

double V3::EconomyManager::calculateTerrainMultiplier(const std::shared_ptr<SubState>& subState) const
{
	const auto& country = subState->getOwner();
	double terrainMultiplier = 0;

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
		// 55% desert 45% plains state has a modifier that is 0.55*desert_modifer + 0.45*plains_modifier
		terrainMultiplier += stateTerrainModifiers.at(terrain) * frequency;
	}

	return terrainMultiplier;
}

double V3::EconomyManager::calculateStateTraitMultiplier(const std::shared_ptr<SubState>& subState) const
{
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
		const double goodsModifiers = stateTrait.getAllBonuses(stateTrait.getGoodsModifiersMap());
		const double buildingsModifiers = stateTrait.getAllBonuses(stateTrait.getBuildingModifiersMap());
		const double buildingGroupsModifiers = stateTrait.getAllBonuses(stateTrait.getBuildingGroupModifiersMap());

		// (20% goods bonus + -30% building bonus + 15% building group bonus) @ half strength = 2.5% Bonus = 0.025
		stateTraitMultiplier += (goodsModifiers + buildingsModifiers + buildingGroupsModifiers) * econDefines.getStateTraitStrength();
	}
	return stateTraitMultiplier;
}

void V3::EconomyManager::distributeBudget(const double globalCP, const double totalIndustryScore) const
{
	for (const auto& country: centralizedCountries)
	{
		country->setCPBudget(static_cast<int>(std::round(globalCP * (country->getIndustryWeight() / totalIndustryScore))));
	}
}

void V3::EconomyManager::setPMs() const
{
	// For now just set bureaucracy, default PMs everywhere else
	// More detailed PM picking will happen later

	for (const auto& country: centralizedCountries)
	{
		auto data = country->getProcessedData();
		const auto& PMName = pickBureaucracyPM(country);
		data.productionMethods["building_government_administration"] = {PMName};
		data.productionMethods["building_port"] = {"pm_basic_port"};
		country->setProductionMethods(data.productionMethods);
	}
}

double V3::EconomyManager::getBaseSubStateWeight(const std::shared_ptr<SubState>& subState, const Configuration::ECONOMY economyType) const
{
	if (economyType == Configuration::ECONOMY::CivLevel)
	{
		// Score is based on population
		return subState->getSubStatePops().getPopCount();
	}
	if (economyType == Configuration::ECONOMY::DevPerPop)
	{
		// TODO(Gawquon): Dev based for now not static, might use an econ define
		// Score is based on Dev, penalized by population
		// base = subState->getSourceProvinceData() ;
		return 0;
	}
	return 0;
}

void V3::EconomyManager::removeNoBuildSubStates(std::vector<std::shared_ptr<SubState>>& subStates) const
{
	auto budgetBelowMinimumCost = [](const std::shared_ptr<SubState>& substate) {
		return substate->getCPBudget() < 50; // getConstructionMinCost();
	};


	// Collect budgets below threshold
	int carryBudget = 0;
	for (const auto& substate: subStates)
	{
		if (budgetBelowMinimumCost(substate))
			carryBudget += substate->getCPBudget();
	}
	std::erase_if(subStates, budgetBelowMinimumCost);

	// Add to top
	if (!subStates.empty())
	{
		subStates[0]->spendCPBudget(-carryBudget);
	}
}

void V3::EconomyManager::removeSubStateIfFinished(std::vector<std::shared_ptr<SubState>>& subStates,
	 const std::vector<std::shared_ptr<SubState>>::iterator& it) const
{
	// if (it->get()->getBuildingWeights.empty())
	//{

	if (subStates.size() >= 2)
	{
		// Carry over budget to current highest budgeted state.
		subStates[0]->spendCPBudget(-it->get()->getCPBudget());
	}

	subStates.erase(it);
	//}
}

void V3::EconomyManager::loadTerrainModifierMatrices(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Loading Terrain Modifier Matrices.";

	TerrainModifierLoader terrainModifierLoader;
	terrainModifierLoader.loadTerrainModifiers(filePath + "configurables/economy/terrain_econ_modifiers.txt");

	stateTerrainModifiers = terrainModifierLoader.getTerrainStateModifiers();
	buildingTerrainModifiers = terrainModifierLoader.getTerrainBuildingModifiers();

	Log(LogLevel::Info) << "<> Loaded " << stateTerrainModifiers.size() << "state and " << buildingTerrainModifiers.size() << " building terrain modifiers.";
}

void V3::EconomyManager::loadStateTraits(const commonItems::ModFilesystem& modFS)
{
	Log(LogLevel::Info) << "-> Loading State Traits.";

	StateModifierLoader stateModifierLoader;
	stateModifierLoader.loadStateModifiers(modFS);
	stateTraits = stateModifierLoader.getStateModifiers();

	Log(LogLevel::Info) << "<> Loaded " << stateTraits.size() << " state traits.";
}

void V3::EconomyManager::loadBuildingInformation(const commonItems::ModFilesystem& modFS)
{
	Log(LogLevel::Info) << "-> Loading building information.";

	BuildingLoader buildingLoader;
	BuildingGroupLoader buildingGroupLoader;
	ProductionMethodLoader PMLoader;
	ProductionMethodGroupLoader PMGroupLoader;

	buildingLoader.loadBuildings(modFS);
	buildingGroupLoader.loadBuildingGroups(modFS);
	PMLoader.loadPMs(modFS);
	PMGroupLoader.loadPMGroups(modFS);

	buildings = buildingLoader.getBuildings();
	buildingGroups = buildingGroupLoader.getBuildingGroups();
	PMs = PMLoader.getPMs();
	PMGroups = PMGroupLoader.getPMGroups();

	Log(LogLevel::Info) << "<> Loaded " << buildings.size() << " buildings, " << buildingGroups.getBuildingGroupMap().size() << " building groups, "
							  << PMs.size() << " PMs and " << PMGroups.size() << " PM groups.";
}

void V3::EconomyManager::loadBuildingMappings(const std::string& filePath)
{
	buildingMapper.loadBuildingMappings("configurables/economy/buildings_map.txt");
}

void V3::EconomyManager::loadEconDefines(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Loading economy defines.";

	econDefines.loadEconDefines(filePath + "configurables/economy/econ_defines.txt");

	Log(LogLevel::Info) << "<> Economy defines loaded.";
}

void V3::EconomyManager::loadNationalBudgets(const std::string& filePath)
{
	nationalBudgets.loadNationalBudget(filePath + "configurables/economy/national_budget.txt");
	nationalBudgets.buildBuildingSectorMap();
}

void V3::EconomyManager::loadTechMap(const commonItems::ModFilesystem& modFS)
{
	techMap.loadTechs(modFS);
}
