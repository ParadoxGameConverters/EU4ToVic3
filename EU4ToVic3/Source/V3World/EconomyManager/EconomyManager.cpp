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
#include "Packet.h"
#include "PoliticalManager/Country/Country.h"
#include "PoliticalManager/PoliticalManager.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <numeric>
#include <queue>
#include <ranges>
void V3::EconomyManager::loadCentralizedStates(const std::map<std::string, std::shared_ptr<Country>>& countries)
{
	Log(LogLevel::Info) << "-> Loading Centralized Countries for Economy Application.";
	for (const auto& country: std::views::values(countries))
	{
		if (!country)
			continue;
		if (country->getProcessedData().type == "decentralized")
			continue;
		if (country->getSubStates().empty())
			continue;
		if (!country->getSourceCountry())
			continue;
		if (country->getPopCount() <= 0)
			continue;

		centralizedCountries.push_back(country);
	}
	Log(LogLevel::Info) << "<> Loaded " << centralizedCountries.size() << " Centralized Countries.";
}

void V3::EconomyManager::loadMappersAndConfigs(const commonItems::ModFilesystem& modFS, const std::string& filePath)
{
	// If anything else ends up needing these, pass them in from World instead of loading direct in Econ

	loadTerrainModifierMatrices(filePath);
	loadStateTraits(modFS);
	loadBuildingInformation(modFS);
	loadBuildingMappings(filePath);
	loadPMMappings(filePath);
	loadEconDefines(filePath);
	loadNationalBudgets(filePath);
	loadOwnerships(filePath);
	loadTechMap(modFS);
	loadDemand(modFS);
	loadPopTypes(modFS);
}

void V3::EconomyManager::establishBureaucracy(const Country& country, const std::map<std::string, Law>& lawsMap, const Vic3DefinesLoader& defines) const
{
	if (!buildings.contains("building_government_administration"))
	{
		Log(LogLevel::Error) << "No building definition found for: building_government_administration.";
		return;
	}

	const auto& govAdmin = buildings.at("building_government_administration");

	// Check tech requirement for government administrations.
	if (!country.hasAnyOfTech(govAdmin.getUnlockingTechs()))
	{
		return;
	}

	// Give 10% extra for trade routes - cap at +400
	const double usage = country.calculateBureaucracyUsage(lawsMap, defines);
	const double generationTarget = std::min(usage * 1.1, usage + 500) - 100;

	// Use the PM with the most generation available
	int PMGeneration = 35;
	const auto& PMName = pickBureaucracyPM(country);
	if (PMs.contains(PMName))
	{
		PMGeneration = PMs.at(PMName).getBureaucracy();
	}

	country.distributeGovAdmins(generationTarget, PMGeneration, techMap.getTechs(), buildings.at("building_government_administration"));
}

void V3::EconomyManager::hardcodePorts(const Country& country) const
{

	for (const auto& subState: country.getSubStates())
	{
		if (!subState->getVanillaBuildingElements().empty())
			continue; // don't affect states imported from vanilla.
		if (!subState->isCoastal())
			continue;

		auto port = std::make_shared<Building>(buildings.at("building_port"));
		port->setLevel(1);

		subState->addBuilding(port);
		subState->getOwner()->addTech("navigation");
	}
}

void V3::EconomyManager::integrateHardcodedBuildings(const Country& country,
	 double defaultRatio,
	 const std::map<std::string, std::tuple<int, double>>& estimatedPMs,
	 const std::map<std::string, Law>& lawsMap,
	 const std::map<std::string, PopType>& popTypes,
	 MarketTracker& market) const
{
	for (const auto& subState: country.getSubStates())
	{
		for (const auto& building_ref: subState->getBuildings())
		{
			const auto& building = *building_ref;
			if (building.getLevel() <= 0)
				continue;

			market.integrateBuilding(building,
				 building.getLevel(),
				 defaultRatio,
				 estimatedPMs,
				 PMGroups,
				 PMs,
				 buildingGroups,
				 {}, // All currently hardcoded buildings do not use ownership.
				 lawsMap,
				 techMap.getTechs(),
				 stateTraits,
				 popTypes,
				 buildings,
				 subState);
		}
	}
}

void V3::EconomyManager::assignCountryCPBudgets(const Configuration::ECONOMY economyType,
	 const Configuration::STARTDATE startDate,
	 const DatingData& dateData,
	 const PoliticalManager& politicalManager,
	 const bool vn) const
{
	Log(LogLevel::Info) << "-> Assigning CP Budgets to Countries.";
	// Some global value of CP to spend
	double globalCP;
	if (!vn)
		globalCP = econDefines.getGlobalCP();
	else
		globalCP = econDefines.getVNGlobalCP();
	const double dateFactor = calculateDateFactor(startDate, dateData);
	const double globalPopFactor = calculateGlobalPopFactor(politicalManager); // Adjust based on amount of world centralized by population

	// Each country figures out its CP budget weight and reports back
	auto [totalIndustryWeight, specialFactors] = countryBudgetCalcs(economyType);

	// distribute each country its budget
	globalCP *= (1 + dateFactor + globalPopFactor + specialFactors);
	distributeBudget(globalCP, totalIndustryWeight);
	Log(LogLevel::Info) << std::fixed << std::setprecision(0) << "<> The world has " << globalCP << " CP to spend on industry.";
}

void V3::EconomyManager::assignSubStateCPBudgets(const Configuration::ECONOMY economyType) const
{
	Log(LogLevel::Info) << "-> Distributing Economy Budgets to Substates.";
	auto counter = 0;
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

			// Adjust for terrain, incorporation status & state traits
			const double terrainMultiplier = calculateTerrainMultiplier(subState);
			const double stateTraitMultiplier = calculateStateTraitMultiplier(subState);
			const double incorporatedMultiplier = subState->isIncorporated() ? 0.0 : econDefines.getIncorporatedModifier();

			subState->setIndustryWeight(std::max(0.0, base * (1 + terrainMultiplier + stateTraitMultiplier + incorporatedMultiplier)));
			totalIndustryWeight += subState->getIndustryWeight();
		}

		// Now back through substates, give each a percentage of country's budget based on percent of total industry score
		for (const auto& subState: country->getSubStates())
		{
			const double stateBudget = country->getCPBudget() * subState->getIndustryWeight() / totalIndustryWeight;
			subState->setCPBudget(static_cast<int>(std::round(stateBudget)));
			subState->setOriginalCPBudget(subState->getCPBudget());
			++counter;
		}
	}
	Log(LogLevel::Info) << "<> Primed " << counter << " Substates for construction.";
}

void V3::EconomyManager::balanceNationalBudgets() const
{
	Log(LogLevel::Info) << "-> Balancing Sub-National Budgets into Industry Sectors.";
	for (const auto& country: centralizedCountries)
	{
		double totalWeight = 0;

		for (const auto& blueprint: nationalBudgets.getSectorBlueprints())
		{
			const auto sector = std::make_shared<Sector>(blueprint, *country);
			country->addSector(blueprint.getName(), sector);
			totalWeight += sector->getWeight();
		}

		for (const auto& sector: country->getProcessedData().industrySectors | std::views::values)
		{
			sector->calculateBudget(totalWeight, country->getCPBudget());
		}
	}
	Log(LogLevel::Info) << "<> Industry Sectors Primed.";
}

void V3::EconomyManager::buildBuildings(const std::map<std::string, Law>& lawsMap,
	 const std::map<std::string, mappers::CultureDef>& cultures,
	 const std::map<std::string, mappers::ReligionDef>& religions,
	 const Vic3DefinesLoader& defines) const
{
	Log(LogLevel::Info) << "-> Building buildings.";
	auto counter = 0;

	// The great negotiation
	// 1. The substate w/ the most CP asks to build its highest scoring building
	// 2. The country checks if the sector that building belongs to has enough CP for at least 1 building
	// 2b. If not, the substate moves on to the next highest scoring building until it exhausts its list.
	// 2c. If a substate exhausts its building list without matching with a country's sector, it gets to ignore the country budget.
	// 2d. The CP then comes out of the country's biggest sector
	// 3. The state then builds as many buildings of that kind as it can, limited by capacity, packet size and sector CP
	// 3b. capacity is RGO/arable land capacity, or can be law/tech based.
	// 3c. packet size is based on the mean amount of CP states have left to build and is configurable
	// 4. If a substate ends up with less CP than the cost for any possible valid building, they relinquish it to the next sector/substate

	MarketTracker market(demand.getGoodsMap(), buildings.at("building_manor_house").getPMGroups(), PMGroups, PMs);

	for (const auto& country: centralizedCountries)
	{
		// Make estimates from country data.
		const auto& sectors = country->getProcessedData().industrySectors;
		auto subStatesByBudget = prepareSubStatesByBudget(country, lawsMap);
		const auto& estimatedPMs = PMMapper.estimatePMs(*country, PMs, PMGroups, buildings);
		const auto& estimatedOwnershipFracs = estimateInvestorBuildings(*country);

		// Initialize the market in a no-buildings state.
		market.resetMarket();
		market.loadPeasants(*country,
			 defines.getWorkingAdultRatioBase(),
			 lawsMap,
			 estimatedPMs,
			 PMs,
			 PMGroups,
			 popTypeLoader.getPopTypes(),
			 buildings,
			 buildingGroups,
			 stateTraits);
		market.loadCultures(country->getCultureBreakdown());

		// Initialize hardcoded buildings needed for balance.
		establishBureaucracy(*country, lawsMap, defines);
		hardcodePorts(*country);
		integrateHardcodedBuildings(*country, defines.getWorkingAdultRatioBase(), estimatedPMs, lawsMap, popTypeLoader.getPopTypes(), market);

		// Until no substate can build.
		while (!subStatesByBudget.empty())
		{
			// Update the pop's demand.
			market.updatePopNeeds(*country, defines, demand, country->getProcessedData().laws, popTypeLoader.getPopTypes(), cultures, religions, lawsMap);

			// Enter negotiation.
			// Pick the substate with the most budget.
			negotiateBuilding(subStatesByBudget[0],
				 sectors,
				 lawsMap,
				 subStatesByBudget,
				 estimatedPMs,
				 estimatedOwnershipFracs,
				 defines.getWorkingAdultRatioBase(),
				 market);
			++counter;

			// A Building has now been built, process for next round.
			std::sort(subStatesByBudget.begin(), subStatesByBudget.end(), SubState::greaterBudget);
			removeSubStateIfFinished(subStatesByBudget, subStatesByBudget.end() - 1, lawsMap);
		}
	}
	Log(LogLevel::Info) << "<> Built " << counter << " buildings world-wide.";
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
		double factor = Country::yearCapFactor(dateData.lastEU4Date) - 1.0;
		// Hard cap factor to 100% as we really don't want *10000% for converting a 1948 ET game.
		factor = std::min(factor, 1.0);
		Log(LogLevel::Info) << std::fixed << std::setprecision(0) << "Altering global industry by " << factor * 100 << "% due to start date of "
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
		return civLevelCountryBudgets();
	if (Configuration::ECONOMY::DevPopVanilla == economyType || Configuration::ECONOMY::DevPopActual == economyType)
		return devCountryBudgets(economyType);

	return {totalWeight, 0.0};
}

std::pair<double, double> V3::EconomyManager::civLevelCountryBudgets() const
{
	// The default way.
	double accumulatedWeight = 0;
	double totalCivLevel = 0.0;
	const double geoMeanPop = calculateGeoMeanCentralizedPops();
	// While determining individual countries' industry scores, accumulate total industry factor and weight.

	for (const auto& country: centralizedCountries)
	{
		const int popCount = country->getPopCount();
		double weight = popCount * (country->getProcessedData().civLevel * 0.01);
		weight *= calculatePopDistanceFactor(popCount, geoMeanPop);
		country->setIndustryWeight(weight);
		accumulatedWeight += country->getIndustryWeight();
		totalCivLevel += country->getProcessedData().civLevel;
	}

	// Adjust global total by average industry factor compared to baseline.
	const double globalIndustryFactor = (totalCivLevel / static_cast<double>(centralizedCountries.size()) / econDefines.getMeanCivlevel()) - 1;

	Log(LogLevel::Info) << std::fixed << std::setprecision(0) << "<> The world is " << (globalIndustryFactor + 1) * 100
							  << "% industrial compared to baseline. Compensating.";

	return {accumulatedWeight, globalIndustryFactor};
}

std::pair<double, double> V3::EconomyManager::devCountryBudgets(const Configuration::ECONOMY perCapitaType) const
{
	double accumulatedWeight = 0;

	for (const auto& country: centralizedCountries)
	{
		if (perCapitaType == Configuration::ECONOMY::DevPopVanilla)
		{
			if (country->getVanillaPopCount() <= 0)
				continue;
			country->setPerCapitaDev(country->getTotalDev() / country->getVanillaPopCount());
		}
		if (perCapitaType == Configuration::ECONOMY::DevPopActual)
		{
			if (country->getPopCount() <= 0)
				continue;
			country->setPerCapitaDev(country->getTotalDev() / country->getPopCount());
		}
		const double techFactor = std::min(0.5, country->getProcessedData().productionTechPercentile) + 0.2;
		const double densityFactor = getDensityFactor(country->getProcessedData().perCapitaDev);

		country->setIndustryWeight(country->getTotalDev() * densityFactor * techFactor);
		accumulatedWeight += country->getIndustryWeight();
	}

	return {accumulatedWeight, 0};
}

double V3::EconomyManager::calculateGeoMeanCentralizedPops() const
{
	double sum = 0;

	for (const auto& country: centralizedCountries)
	{
		sum += log(country->getPopCount());
	}

	sum /= static_cast<double>(centralizedCountries.size());

	// Log(LogLevel::Debug) << "The Geometric Mean of all centralized countries population is : " << exp(sum);
	return exp(sum);
}

std::string V3::EconomyManager::pickBureaucracyPM(const Country& country) const
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
		if (!country.hasAnyOfTech(PM.getUnlockingTechs()))
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
							  << "% Centralized by population. Adjusting global CP values by: " << globalPopFactor * 100 << "%.";
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
				// Silently ignore oceans in Sahara. Yes, there are oceans in Sahara.
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

double V3::EconomyManager::getDensityFactor(const double perCapitaDev) const
{
	// Plot a line between min and max per capita dev. Return a factor between minFactor and 1 based on % position on that line.
	constexpr double minFactor = 0.3;
	const double p = (perCapitaDev - econDefines.getMinDevPerPop() / econDefines.getMaxDevPerPop() - econDefines.getMinDevPerPop());
	if (p < 0)
		return minFactor;
	if (p > 1)
		return 1;
	return p + (1 - p) * minFactor;
}

void V3::EconomyManager::distributeBudget(const double globalCP, const double totalIndustryScore) const
{
	for (const auto& country: centralizedCountries)
	{
		int budget = static_cast<int>(std::round(globalCP * (country->getIndustryWeight() / totalIndustryScore)));
		country->setCPBudget(budget);
	}
}

void V3::EconomyManager::investCapital(const std::map<std::string, std::shared_ptr<Country>>& countries) const
{
	Log(LogLevel::Info) << "-> Pops buying ownership shares.";

	// Each building get's it's ownership information from the ownership loader.
	// Ownership levels are apportioned.
	// Non-0 levels are further apportioned based on local/foreign/capital ownership
	int investedLevels = 0;
	int investedBuildings = 0;

	for (const auto& country: centralizedCountries)
	{
		std::map<std::string, double> investorIOUs;
		std::map<std::string, double> capitalIOUs;
		std::map<std::string, double> overlordIOUs;

		const std::string& overlordTag = country->getOverlord();
		std::string overlordCapital;

		if (const auto& overlordIt = countries.find(overlordTag); overlordIt != countries.end())
		{
			overlordCapital = overlordIt->second->getProcessedData().capitalStateName;
		}

		for (const auto& subState: country->getSubStates())
		{
			for (const auto& building: subState->getBuildings())
			{
				if (building->getLevel() == 0)
				{
					continue;
				}

				// First assemble the investor weights conditional on the country's state-space
				const auto& ownershipMap = ownershipLoader.getOwnershipsFromBuilding(building->getName());
				const auto& investorFractions = calcInvestorFractions(ownershipMap, *country);


				// Now apportion the buildings out among the different investor types
				const auto& investorApportionment = apportionInvestors(building->getLevel(), investorFractions, investorIOUs);

				// For each investor class with assigned buildings, split owners between local/foreign/capital as directed
				for (const auto& [type, levels]: investorApportionment)
				{
					if (levels == 0)
					{
						continue;
					}

					int capitalLevels = 0, empireLevels = 0;
					const auto& ownership = ownershipMap.at(type);

					// Send some owners to the capital
					if (ownership.financialCenterFrac > 0)
					{
						std::map<std::string, double> capitalWeights{{"local", 1 - ownership.financialCenterFrac}, {"capital", ownership.financialCenterFrac}};
						capitalLevels = apportionInvestors(levels, capitalWeights, capitalIOUs).at("capital");
					}

					// Send some owners to the empire
					if (ownership.colonialFrac > 0 && overlordTag != "" && overlordCapital != "")
					{
						std::map<std::string, double> imperialWeights{{"local", 1 - ownership.colonialFrac}, {"overlord", ownership.colonialFrac}};
						empireLevels = apportionInvestors(levels, imperialWeights, overlordIOUs).at("overlord");
					}

					if (levels - capitalLevels - empireLevels > 0)
					{
						building->addInvestor(levels - capitalLevels - empireLevels, type, subState->getHomeStateName(), country->getTag());
						investedLevels += levels - capitalLevels - empireLevels;
					}
					if (capitalLevels > 0)
					{
						building->addInvestor(capitalLevels, type, country->getProcessedData().capitalStateName, country->getTag());
						investedLevels += capitalLevels;
					}
					if (empireLevels > 0)
					{
						building->addInvestor(empireLevels, type, overlordCapital, overlordTag);
						investedLevels += empireLevels;
					}
				}
				investedBuildings += 1;
			}
		}
	}
	Log(LogLevel::Info) << "<> Pops invested in " << investedLevels << " shares of " << investedBuildings << " buildings world-wide.";
}

void V3::EconomyManager::setPMs() const
{
	for (const auto& country: centralizedCountries)
	{
		PMMapper.applyRules(*country, PMs, PMGroups);

		// Gov Admin are special
		const auto& adminPMName = pickBureaucracyPM(*country);
		for (const auto& subState: country->getSubStates())
		{
			for (const auto& building: subState->getBuildings())
			{
				if (building->getName() != "building_government_administration")
					continue;

				building->addPM(adminPMName);
			}
		}
	}
}

double V3::EconomyManager::getBaseSubStateWeight(const std::shared_ptr<SubState>& subState, const Configuration::ECONOMY economyType) const
{
	if (economyType == Configuration::ECONOMY::CivLevel)
	{
		// Score is based on population
		return subState->getSubStatePops().getPopCount();
	}
	if (economyType == Configuration::ECONOMY::DevPopVanilla)
	{
		// Score is based on Dev, penalized by unadjusted population
		return subState->getTotalDev() * getDensityFactor(subState->getTotalDev() / subState->getVanillaPopCount());
	}
	if (economyType == Configuration::ECONOMY::DevPopActual)
	{
		// Score is based on Dev, penalized by actual population present
		return subState->getTotalDev() * getDensityFactor(subState->getTotalDev() / subState->getSubStatePops().getPopCount());
	}
	return 0;
}

std::vector<std::shared_ptr<V3::SubState>> V3::EconomyManager::prepareSubStatesByBudget(const std::shared_ptr<Country>& country,
	 const std::map<std::string, Law>& lawsMap) const
{
	// Copy substate vector. We will be sorting this one and removing finished substates until it is empty
	auto subStatesByBudget(country->getSubStates());
	std::sort(subStatesByBudget.begin(), subStatesByBudget.end(), SubState::greaterBudget);

	// Make buildings from template buildings
	// Only valid building will be added to the vector
	for (const auto& subState: subStatesByBudget)
	{
		subState->gatherPossibleBuildings(buildings,
			 buildingGroups,
			 buildingTerrainModifiers,
			 buildingMapper,
			 lawsMap,
			 techMap.getTechs(),
			 stateTraits,
			 econDefines.getStateTraitStrength());
	}

	// For logging and possibly compensating purposes.
	/*
	const auto removedBudget =
		 std::accumulate(subStatesByBudget.begin(), subStatesByBudget.end(), 0, [this, lawsMap](const int sum, const std::shared_ptr<SubState>& substate) {
			 if (!substate->hasValidBuildings(buildingGroups, lawsMap, techMap.getTechs(), stateTraits))
			 {
				 return sum + substate->getCPBudget();
			 }
			 return sum;
		 });
	*/

	// Eliminate states with no building options
	std::erase_if(subStatesByBudget, [this, lawsMap](const std::shared_ptr<SubState>& subState) {
		return !subState->hasValidBuildings(buildingGroups, lawsMap, techMap.getTechs(), stateTraits);
	});

	return subStatesByBudget;
}

void V3::EconomyManager::negotiateBuilding(const std::shared_ptr<SubState>& subState,
	 const std::map<std::string, std::shared_ptr<Sector>>& sectors,
	 const std::map<std::string, Law>& lawsMap,
	 const std::vector<std::shared_ptr<SubState>>& subStates,
	 const std::map<std::string, std::tuple<int, double>>& estimatedPMs,
	 const std::map<std::string, std::map<std::string, double>>& estimatedOwnershipFracs,
	 const double defaultRatio,
	 MarketTracker& market) const
{
	// Whether or not the negotiation succeeds, a building MUST be built.

	// Flag to see if negotiation was successful
	bool talksFail = true;

	// Find the building the state wants most that is in the country budget
	subState->weightBuildings(buildingGroups,
		 buildingTerrainModifiers,
		 buildingMapper,
		 lawsMap,
		 techMap.getTechs(),
		 stateTraits,
		 econDefines.getStateTraitStrength());
	for (const auto& building: subState->getBuildings())
	{
		const int baseCost = building->getConstructionCost();
		const auto& sector = nationalBudgets.getSectorName(building->getName());
		if (!sector)
		{
			continue; // silently skip buildings we're deliberately ignoring.
		}
		if (!sectors.contains(sector.value()))
		{
			Log(LogLevel::Error) << "Unknown Industrial Sector : " << sector.value() << ".";
			continue;
		}
		if (baseCost > sectors.at(sector.value())->getCPBudget())
		{
			continue;
		}

		// So we're a valid building in a valid sector and there is budget for us. Great!
		buildBuilding(building, subState, sectors.at(sector.value()), lawsMap, subStates, estimatedPMs, estimatedOwnershipFracs, defaultRatio, market);
		talksFail = false;
		break;
	}

	if (talksFail)
	{
		// Negotiation failed
		// State picks its favorite building, takes from biggest sector
		buildBuilding(subState->getBuildings()[0],
			 subState,
			 getSectorWithMostBudget(sectors),
			 lawsMap,
			 subStates,
			 estimatedPMs,
			 estimatedOwnershipFracs,
			 defaultRatio,
			 market);
	}
}

std::shared_ptr<V3::Sector> V3::EconomyManager::getSectorWithMostBudget(const std::map<std::string, std::shared_ptr<Sector>>& sectors)
{
	auto maxIter = std::max_element(sectors.begin(), sectors.end(), [](const auto& lhs, const auto& rhs) {
		return lhs.second->getCPBudget() < rhs.second->getCPBudget();
	});

	return maxIter->second;
}

void V3::EconomyManager::buildBuilding(const std::shared_ptr<Building>& building,
	 const std::shared_ptr<SubState>& subState,
	 const std::shared_ptr<Sector>& sector,
	 const std::map<std::string, Law>& lawsMap,
	 const std::vector<std::shared_ptr<SubState>>& subStates,
	 const std::map<std::string, std::tuple<int, double>>& estimatedPMs,
	 const std::map<std::string, std::map<std::string, double>>& estimatedOwnershipFracs,
	 const double defaultRatio,
	 MarketTracker& market) const
{
	// SUBSTATE MUST SPEND ITS CP OR WE GET INFINITE LOOPS
	// Spend sector CP if possible

	// Pick a packet size!
	const int p =
		 Packet(*building, sector->getCPBudget(), econDefines.getPacketFactor(), *subState, subStates, lawsMap, techMap.getTechs(), stateTraits, buildingGroups)
			  .getSize();

	int cost = building->getConstructionCost() * p;
	subState->spendCPBudget(cost);
	sector->spendCP(cost);
	building->setLevel(building->getLevel() + p);

	// If arable, decrease arable land. This does have a side effect of no arable building being able to use more than 50% of the arable land
	const auto& arables = subState->getHomeState()->getArableResources();
	if (const auto& isArable = std::ranges::find(arables, building->getBuildingGroup()); isArable != arables.end())
	{
		subState->setResource("bg_agriculture", subState->getResource("bg_agriculture") - p);
	}

	// Track Demand
	market.integrateBuilding(*building,
		 p,
		 defaultRatio,
		 estimatedPMs,
		 PMGroups,
		 PMs,
		 buildingGroups,
		 estimatedOwnershipFracs,
		 lawsMap,
		 techMap.getTechs(),
		 stateTraits,
		 popTypeLoader.getPopTypes(),
		 buildings,
		 subState);
}

void V3::EconomyManager::removeSubStateIfFinished(std::vector<std::shared_ptr<SubState>>& subStates,
	 const std::vector<std::shared_ptr<SubState>>::iterator& subState,
	 const std::map<std::string, Law>& lawsMap) const
{
	if (!subState->get()->hasValidBuildings(buildingGroups, lawsMap, techMap.getTechs(), stateTraits))
	{
		if (subStates.size() >= 2)
		{
			// Carry over budget to current highest budgeted state.
			subStates[0]->spendCPBudget(-subState->get()->getCPBudget());
		}
		subStates.erase(subState);
	}
}

std::map<std::string, int> V3::EconomyManager::apportionInvestors(const int levels,
	 const std::map<std::string, double>& investorWeights,
	 std::map<std::string, double>& investorIOUs) const
{
	int apportionedLevels = 0;
	std::priority_queue<std::tuple<double, std::string>> hamiltonQueue;
	std::map<std::string, int> investorLevels;

	// First apply IOUs so that minority owners still get buildings now and then.
	for (const auto& [investor, IOU]: investorIOUs)
	{
		if (!investorWeights.contains(investor))
		{
			continue;
		}

		if (IOU > 1)
		{
			investorLevels[investor] += 1;
			apportionedLevels += 1;
			investorIOUs[investor] -= 1;
		}

		if (apportionedLevels == levels)
		{
			break;
		}
	}
	const int repaidLevels = apportionedLevels;

	for (const auto& [investor, weight]: investorWeights)
	{
		const double weightedLevels = weight * (levels - repaidLevels);
		double apportionment;
		const double IOU = std::modf(weightedLevels, &apportionment);
		apportionedLevels += static_cast<int>(apportionment);
		investorLevels[investor] += static_cast<int>(apportionment);
		hamiltonQueue.emplace(IOU, investor);
	}
	while (apportionedLevels < levels)
	{
		const auto& [_, investor] = hamiltonQueue.top();
		investorLevels[investor] += 1;
		apportionedLevels += 1;
		hamiltonQueue.pop();
	}
	while (!hamiltonQueue.empty())
	{
		const auto& [IOU, investor] = hamiltonQueue.top();

		const auto& it = investorIOUs.find(investor);
		if (it == investorIOUs.end())
		{
			investorIOUs.emplace(investor, IOU);
		}
		else
		{
			it->second += IOU;
		}
		hamiltonQueue.pop();
	}
	return investorLevels;
}

std::map<std::string, std::map<std::string, double>> V3::EconomyManager::estimateInvestorBuildings(const Country& country) const
{
	std::map<std::string, std::map<std::string, double>> buildingInvestorEstimates;

	for (const auto& building: ownershipLoader.getBuildingSectorMap() | std::views::keys)
	{
		double totalWeight = 0;
		std::map<std::string, double> investorWeights;
		const auto& ownershipMap = ownershipLoader.getOwnershipsFromBuilding(building);

		for (const auto& [type, data]: ownershipMap)
		{
			if (data.recognized && country.getProcessedData().type != "recognized")
			{
				continue;
			}

			totalWeight += data.weight;
			if (const auto& findIter = type.find("building"); findIter != std::string::npos)
				investorWeights[type] = data.weight;
		}
		for (const auto& type: investorWeights | std::views::keys)
		{
			investorWeights[type] /= totalWeight;
		}
		for (const auto& [type, weight]: investorWeights)
		{
			const auto& data = ownershipMap.at(type);
			buildingInvestorEstimates[building][type] = weight - weight * (data.colonialFrac + data.financialCenterFrac);
		}
	}
	return buildingInvestorEstimates;
}

std::map<std::string, double> V3::EconomyManager::calcInvestorFractions(const std::map<std::string, OwnershipData>& ownershipMap, const Country& country)
{
	std::map<std::string, double> investorWeights;

	double totalWeight = 0;
	for (const auto& [type, investorData]: ownershipMap)
	{
		if (investorData.recognized && country.getProcessedData().type != "recognized")
		{
			continue;
		}

		investorWeights[type] = investorData.weight;
		totalWeight += investorData.weight;
	}

	// Turn weights into fractions
	for (const auto& type: investorWeights | std::views::keys)
	{
		investorWeights[type] /= totalWeight;
	}

	return investorWeights;
}

void V3::EconomyManager::loadTerrainModifierMatrices(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Loading Terrain Modifier Matrices.";

	TerrainModifierLoader terrainModifierLoader;
	terrainModifierLoader.loadTerrainModifiers(filePath + "configurables/economy/terrain_econ_modifiers.txt");

	stateTerrainModifiers = terrainModifierLoader.getTerrainStateModifiers();
	buildingTerrainModifiers = terrainModifierLoader.getTerrainBuildingModifiers();

	Log(LogLevel::Info) << "<> Loaded " << stateTerrainModifiers.size() << " state and " << buildingTerrainModifiers.size() << " building terrain modifiers.";
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
	buildingMapper.loadBuildingMappings(filePath + "configurables/economy/buildings_map.txt");
}

void V3::EconomyManager::loadPMMappings(const std::string& filePath)
{
	PMMapper.loadRules(filePath + "configurables/economy/production_method_rules.txt");
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

void V3::EconomyManager::loadOwnerships(const std::string& filePath)
{
	ownershipLoader.loadOwnership(filePath + "configurables/economy/ownership.txt");
}

void V3::EconomyManager::loadTechMap(const commonItems::ModFilesystem& modFS)
{
	techMap.loadTechs(modFS);
}

void V3::EconomyManager::loadDemand(const commonItems::ModFilesystem& modFS)
{
	demand.loadGoods(modFS);
	demand.loadPopNeeds(modFS);
	demand.loadBuyPackages(modFS);
}

void V3::EconomyManager::loadPopTypes(const commonItems::ModFilesystem& modFS)
{
	popTypeLoader.loadPopTypes(modFS);
}
