#include "MarketTracker.h"
#include "ClayManager/State/State.h"
#include "ClayManager/State/SubState.h"
#include "EconomyManager/Building/BuildingGroup.h"
#include <cmath>
#include <iomanip>

V3::MarketTracker::MarketTracker(const std::map<std::string, Good>& possibleGoods,
	 const std::set<std::string>& manorHousePMGroups,
	 const std::map<std::string, ProductionMethodGroup>& PMGroups,
	 const std::map<std::string, ProductionMethod>& PMs)
{
	market.loadGoods(possibleGoods);
	std::map<std::string, int> manorHouseEmployment;
	for (const auto& pmg: manorHousePMGroups)
	{
		for (const auto& [job, amount]: PMs.at(PMGroups.at(pmg).getPMs()[0]).getEmployment())
		{
			manorHouseEmployment[job] = amount;
		}
	}
	std::vector<std::pair<std::string, int>> manorHouseRoster;
	for (const auto& pair: manorHouseEmployment)
	{
		manorHouseRoster.push_back(pair);
	}
	marketJobs = MarketJobs(manorHouseRoster);
}

void V3::MarketTracker::resetMarket()
{
	market.clearMarket();
	marketCulture.clear();
}

void V3::MarketTracker::loadPeasants(const Country& country,
	 const double defaultRatio,
	 const std::map<std::string, Law>& lawsMap,
	 const std::map<std::string, std::tuple<int, double>>& estimatedPMs,
	 const std::map<std::string, ProductionMethod>& PMs,
	 const std::map<std::string, ProductionMethodGroup>& PMGroups,
	 const std::map<std::string, PopType>& popTypes,
	 const std::map<std::string, Building>& buildings,
	 const V3::BuildingGroups& buildingGroups,
	 const std::map<std::string, V3::StateModifier>& stateTraits)
{
	const double womenJobRate = Market::calcAddedWorkingPopFraction(country.getProcessedData().laws, lawsMap);

	// For each state check peasant PM
	for (const auto& subState: country.getSubStates())
	{
		const auto& traits = subState->getHomeState()->getTraits();
		BuildingResources subsistenceResources;
		const Building& subsistenceBuilding = buildings.at(subState->getHomeState()->getSubsistenceBuilding());
		subsistenceResources.evaluateResources(subsistenceBuilding.getPMGroups(), estimatedPMs, PMs, PMGroups);

		const double subsistenceModifier = calcThroughputStateModifier(traits, subsistenceBuilding, buildingGroups, stateTraits);
		const double levels =
			 marketJobs.createSubsistence(subsistenceResources.getJobs(), defaultRatio, womenJobRate, subState->getResource("bg_agriculture"), popTypes, subState);
		updateMarketGoods(levels, levels, 0, subsistenceModifier, subsistenceResources, traits, stateTraits);
	}
}

void V3::MarketTracker::updatePopNeeds(const V3::Country& country,
	 const Vic3DefinesLoader& defines,
	 const DemandLoader& demandDefines,
	 const std::set<std::string>& laws,
	 const std::map<std::string, PopType>& popTypes,
	 const std::map<std::string, mappers::CultureDef>& cultures,
	 const std::map<std::string, mappers::ReligionDef>& religions,
	 const std::map<std::string, Law>& lawsMap)
{
	market.calcPopOrders(country.getPopCount(), country.getJobBreakdown(), marketCulture, defines, demandDefines, popTypes, cultures, religions, laws, lawsMap);
}

void V3::MarketTracker::integrateBuilding(const Building& building,
	 const int p,
	 const double defaultRatio,
	 const std::map<std::string, std::tuple<int, double>>& estimatedPMs,
	 const std::map<std::string, ProductionMethodGroup>& PMGroups,
	 const std::map<std::string, ProductionMethod>& PMs,
	 const BuildingGroups& buildingGroups,
	 const std::map<std::string, std::map<std::string, double>>& estOwnershipFractions,
	 const std::map<std::string, Law>& lawsMap,
	 const std::map<std::string, Tech>& techMap,
	 const std::map<std::string, StateModifier>& stateTraits,
	 const std::map<std::string, PopType>& popTypes,
	 const std::map<std::string, Building>& buildings,
	 const std::shared_ptr<SubState>& subState)
{
	const auto& traits = subState->getHomeState()->getTraits();
	BuildingResources buildingResources;
	buildingResources.evaluateResources(building.getPMGroups(), estimatedPMs, PMs, PMGroups);

	// Collect any/all building or inherited building_group throughput modifiers
	const double throughputMod = calcThroughputStateModifier(traits, building, buildingGroups, stateTraits);

	// Evaluate the economy of scale cap
	// There is an economy of scale penalty for nationalized buildings. For now we're ignoring it.
	// There are scenarios other than subsistence building that do not use economy of scale, but none that are currently relevant.
	const int eosCap = calcEconomyOfScaleCap(*subState->getOwner(), building, buildingGroups, techMap);

	//// Update the market.
	updateMarketGoods(building.getLevel(), p, eosCap, throughputMod, buildingResources, traits, stateTraits);

	// Ownership fraction filtering.
	std::map<std::string, double> estOwnershipFraction;
	if (estOwnershipFractions.contains(building.getName()))
	{
		estOwnershipFraction = estOwnershipFractions.at(building.getName());
	}

	// Set up Subsistence
	BuildingResources subsistenceResources;
	const Building& subsistenceBuilding = buildings.at(subState->getHomeState()->getSubsistenceBuilding());
	subsistenceResources.evaluateResources(subsistenceBuilding.getPMGroups(), estimatedPMs, PMs, PMGroups);
	const double addedWorkingPopFraction = Market::calcAddedWorkingPopFraction(subState->getOwner()->getProcessedData().laws, lawsMap);

	// Track Jobs changed.
	double lostSubsistence = marketJobs.createJobs(buildingResources.getJobs(),
		 subsistenceResources.getJobs(),
		 p,
		 defaultRatio,
		 addedWorkingPopFraction,
		 estOwnershipFraction,
		 {{
				"building_financial_district",
				{{"capitalists", 50}, {"shopkeepers", 100}, {"clerks", 100}},
		  },
			  {"building_manor_house", {{"aristocrats", 50}, {"laborers", 100}}}}, // TODO(Gawquon): Load in
		 popTypes,
		 subState);

	// Track outputs and Jobs from new Urban Centers.
	if (const auto& urbanization = buildingGroups.getUrbanization(building.getBuildingGroup()); urbanization > 0)
	{
		const double urbanFrac = urbanization / 100.0;
		subState->addUrbanCenters(urbanFrac * p);

		BuildingResources urbanResources;
		if (!buildings.contains("building_urban_center"))
		{
			Log(LogLevel::Error) << "No building definition of Urban Center.";
		}
		const Building& urbanBuilding = buildings.at("building_urban_center");
		const double urbanThroughputMod = calcThroughputStateModifier(traits, urbanBuilding, buildingGroups, stateTraits);
		urbanResources.evaluateResources(urbanBuilding.getPMGroups(), estimatedPMs, PMs, PMGroups);

		const double urbanLevel = subState->getUrbanCenters() + urbanFrac * p;
		updateMarketGoods(urbanLevel, urbanFrac * p, eosCap, urbanThroughputMod, urbanResources, traits, stateTraits);

		lostSubsistence += marketJobs.createJobs(urbanResources.getJobs(),
			 subsistenceResources.getJobs(),
			 p * urbanFrac,
			 defaultRatio,
			 addedWorkingPopFraction,
			 {},
			 {},
			 popTypes,
			 subState);
	}

	// Track Subsistence outputs.
	const double subsistenceModifier = calcThroughputStateModifier(traits, subsistenceBuilding, buildingGroups, stateTraits);
	updateMarketGoods(-lostSubsistence, -lostSubsistence, 0, subsistenceModifier, subsistenceResources, traits, stateTraits);
}

void V3::MarketTracker::logDebugMarket(const Country& country) const
{
	Log(LogLevel::Debug) << "\n"
								<< country.getName("english") << "'s Market:\n"
								<< market.marketAsTable().str() << "\nJobs Estimate:" << breakdownAsTable(country.getJobBreakdown(), country.getPopCount(), false).str()
								<< "\nCulture Split:" << breakdownAsTable(marketCulture, 0).str();
}

// Debugging function
std::stringstream V3::MarketTracker::breakdownAsTable(const std::map<std::string, double>& breakdown, const int popCount, const bool asPercent)
{
	std::stringstream out;
	int nameLength = 0;
	int percentLength = 0;
	for (const auto& [good, amt]: breakdown)
	{
		nameLength = std::max(nameLength, static_cast<int>(good.length()));
		percentLength = std::max(percentLength, static_cast<int>(std::to_string(amt).length()));
	}

	out << std::setprecision(3);
	out << std::endl;
	out << std::setfill('-') << std::setw(nameLength + 2) << "" << std::setw(percentLength + 2) << "" << std::endl;
	out << std::setfill(' ');

	const double mult = asPercent ? 1 : popCount;

	for (const auto& pair: breakdown)
	{
		out << std::left << std::setw(nameLength + 2) << pair.first << std::setw(percentLength + 2) << pair.second * mult << std::endl;
	}
	return out;
}

void V3::MarketTracker::updateMarketGoods(const double level,
	 const double p,
	 const int eosCap,
	 const double throughputMod,
	 const BuildingResources& buildingResources,
	 const std::vector<std::string>& traits,
	 const std::map<std::string, StateModifier>& stateTraits)
{
	for (const auto& [good, amount]: buildingResources.getInputs())
	{
		double effectiveLevelsAdded;
		if (level <= eosCap)
		{
			effectiveLevelsAdded = p * ((2 * level - p) / 100.0 + 1 + throughputMod);
		}
		else if (level - p >= eosCap) // We're already past the economy of scale cap.
		{
			effectiveLevelsAdded = p * (eosCap / 100.0 + 1 + throughputMod);
		}
		else // The new level of buildings just jumped over the economy of scale cap.
		{
			effectiveLevelsAdded = p * (throughputMod + 1) + (eosCap * level + 2 * level * p - pow(level, 2) - pow(p, 2)) / 100.0;
		}
		market.buyForBuilding(good, effectiveLevelsAdded * amount);
	}

	for (const auto& [good, amount]: buildingResources.getOutputs())
	{
		// Collect any/all good specific output modifiers
		double outputMod = 0;
		for (const auto& trait: traits)
		{
			if (const auto& traitIter = stateTraits.find(trait); traitIter != stateTraits.end())
			{
				outputMod += traitIter->second.getGoodsModifier(good).value_or(0);
				continue;
			}
			Log(LogLevel::Warning) << "Trait: " << trait << "has no definition.";
		}

		double effectiveLevelsAdded;
		if (level <= eosCap)
		{
			effectiveLevelsAdded = p * ((2 * level - p) / 100.0 + 1 + throughputMod + outputMod);
		}
		else if (level - p >= eosCap) // We're already past the economy of scale cap.
		{
			effectiveLevelsAdded = p * (eosCap / 100.0 + 1 + throughputMod + outputMod);
		}
		else // The new level of buildings just jumped over the economy of scale cap.
		{
			effectiveLevelsAdded = p * (throughputMod + outputMod + 1) + (eosCap * level + 2 * level * p - pow(level, 2) - pow(p, 2)) / 100.0;
		}
		market.sell(good, effectiveLevelsAdded * amount);
	}
}

double V3::MarketTracker::calcThroughputStateModifier(const std::vector<std::string>& traits,
	 const Building& building,
	 const BuildingGroups& buildingGroups,
	 const std::map<std::string, StateModifier>& stateTraits)
{
	double throughputMod = 0;
	for (const auto& trait: traits)
	{
		if (const auto& traitIter = stateTraits.find(trait); traitIter != stateTraits.end())
		{
			throughputMod += traitIter->second.calcBuildingModifiers(building, buildingGroups);
			continue;
		}
		Log(LogLevel::Warning) << "Trait: " << trait << "has no definition.";
	}

	return throughputMod;
}

int V3::MarketTracker::calcEconomyOfScaleCap(const Country& country,
	 const Building& building,
	 const BuildingGroups& buildingGroups,
	 const std::map<std::string, Tech>& techMap)
{
	if (buildingGroups.getBuildingGroupMap().at(building.getBuildingGroup())->isSubsistence())
	{
		return 0;
	}
	return country.getThroughputMax(techMap);
}
