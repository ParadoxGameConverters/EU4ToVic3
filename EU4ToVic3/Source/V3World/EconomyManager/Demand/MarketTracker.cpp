#include "MarketTracker.h"
#include "ClayManager/State/State.h"
#include "ClayManager/State/SubState.h"
#include "EconomyManager/Building/BuildingGroup.h"
#include "EconomyManager/Building/BuildingResources.h"

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
	std::vector<std::pair<std::string, int>> mhr;
	for (const auto& pair: manorHouseEmployment)
	{
		mhr.push_back(pair);
	}
	marketJobs = MarketJobs(mhr);
}

void V3::MarketTracker::resetMarket()
{
	market.clearMarket();
	marketCulture.clear();
}

void V3::MarketTracker::loadPeasants(const Country& country,
	 const double defaultRatio,
	 const std::map<std::string, Law>& lawsMap,
	 const std::map<std::string, ProductionMethodGroup>& PMGroups,
	 const std::map<std::string, ProductionMethod>& PMs,
	 const std::map<std::string, PopType>& popTypes,
	 const std::map<std::string, Building>& buildings)
{
	const double womenJobRate = Market::calcAddedWorkingPopPercent(country.getProcessedData().laws, lawsMap);

	// For each state check peasant PM
	for (const auto& subState: country.getSubStates())
	{
		const std::string& subsistenceBuildingName = subState->getHomeState()->getSubsistenceBuilding();
		auto subsistenceEmployment = getSubsistenceEmployment(subsistenceBuildingName, country.getProcessedData().laws, PMGroups, PMs, buildings);
		const double levels =
			 marketJobs.createSubsistence(subsistenceEmployment, defaultRatio, womenJobRate, subState->getResource("bg_agriculture"), popTypes, subState);

		// TODO Send levels of subsistenceBuilding to Market
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

	BuildingResources buildingResources;
	buildingResources.evaluateResources(building.getPMGroups(), estimatedPMs, PMs, PMGroups);

	// Collect any/all building or inherited building_group throughput modifiers
	double throughputMod = 1;
	for (const auto& trait: subState->getHomeState()->getTraits())
	{
		if (const auto& traitIter = stateTraits.find(trait); traitIter != stateTraits.end())
		{
			throughputMod += traitIter->second.calcBuildingModifiers(building, buildingGroups);
			continue;
		}
		Log(LogLevel::Warning) << "Trait: " << trait << "has no definition.";
	}

	// Evaluate the economy of scale cap
	// There is an economy of scale penalty for nationalized buildings. For now we're ignoring it.
	// There are scenarios other than subsistence building that do not use economy of scale, but none that are currently relevant.
	int eosCap = subState->getOwner()->getThroughputMax(techMap);
	if (buildingGroups.getBuildingGroupMap().at(building.getBuildingGroup())->isSubsistence())
	{
		eosCap = 0;
	}

	//// Update the market
	const int level = building.getLevel();
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
			effectiveLevelsAdded = p * (throughputMod + 1) + (eosCap * level + 2 * level * p - std::pow(level, 2) - std::pow(p, 2)) / 100.0;
		}
		market.sell(good, effectiveLevelsAdded * amount);
	}
	for (const auto& [good, amount]: buildingResources.getOutputs())
	{
		// Collect any/all good specific output modifiers
		double outputMod = 1;
		for (const auto& trait: subState->getHomeState()->getTraits())
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
			effectiveLevelsAdded = p * (throughputMod + outputMod + 1) + (eosCap * level + 2 * level * p - std::pow(level, 2) - std::pow(p, 2)) / 100.0;
		}
		market.buyForBuilding(good, effectiveLevelsAdded * amount);
	}

	const double lostSubsistence = marketJobs.createJobs(buildingResources.getJobs(),
		 getSubsistenceEmployment(subState->getHomeState()->getSubsistenceBuilding(), subState->getOwner()->getProcessedData().laws, PMGroups, PMs, buildings),
		 p,
		 defaultRatio,
		 Market::calcAddedWorkingPopPercent(subState->getOwner()->getProcessedData().laws, lawsMap),
		 estOwnershipFractions.at(building.getName()),
		 {},
		 popTypes,
		 subState);

	// TODO remove lost subsistence from market
}

void V3::MarketTracker::logDebugMarket(const Country& country) const
{
	Log(LogLevel::Debug) << country.getName("english") << "'s Market:\n"
								<< market.marketAsTable().str() << "Jobs Estimate:\n"
								<< breakdownAsTable(country.getJobBreakdown()).str() << "Culture Split:\n"
								<< breakdownAsTable(marketCulture).str();
}

std::stringstream V3::MarketTracker::breakdownAsTable(const std::map<std::string, double>& breakdown)
{
	std::stringstream out;
	int nameLength = 0;
	int percentLength = 0;
	for (const auto& [good, amt]: breakdown)
	{
		nameLength = std::max(nameLength, static_cast<int>(good.length()));
		percentLength = std::max(percentLength, static_cast<int>(std::to_string(amt).length()));
	}

	out << std::setprecision(1);
	out << std::endl;
	out << std::left << std::setw(nameLength + 2) << "Good" << std::setw(percentLength + 2) << "Price" << std::endl;
	out << std::setfill('-') << std::setw(nameLength + 2) << "" << std::setw(percentLength + 2) << "" << std::endl;
	out << std::setfill(' ');

	for (const auto& pair: breakdown)
	{
		out << std::left << std::setw(nameLength + 2) << pair.first << std::setw(percentLength + 2) << pair.second * 100 << "%" << std::endl;
	}
	return out;
}

int V3::MarketTracker::getPMAllowedByLaws(const std::string& PMGroup,
	 const std::set<std::string>& laws,
	 const std::map<std::string, ProductionMethodGroup>& PMGroups,
	 const std::map<std::string, ProductionMethod>& PMs) const
{
	const auto& groupPMs = PMGroups.at(PMGroup).getPMs();

	// Validate every PM in group
	for (const auto& PM: groupPMs)
	{
		if (!PMs.contains(PM))
		{
			Log(LogLevel::Error) << "Unknown PM: " << PM << ".";
			return 0;
		}
	}

	// Walk the group
	int pick = 0;
	for (const auto& PM: groupPMs)
	{
		const auto& thePM = PMs.at(PM);
		if (hasUnlockingLaws(laws, thePM.getUnlockingLaws()) && !hasBlockingLaws(laws, thePM.getBlockingLaws()))
			return std::max(pick - 1, 0);
		++pick;
	}
	return std::max(pick - 1, 0);
}

bool V3::MarketTracker::hasUnlockingLaws(const std::set<std::string>& laws, const std::set<std::string>& targetLaws) const
{
	if (targetLaws.empty())
	{
		return true;
	}

	return std::ranges::any_of(targetLaws, [laws](const std::string& targetLaw) {
		return laws.contains(targetLaw);
	});
}

bool V3::MarketTracker::hasBlockingLaws(const std::set<std::string>& laws, const std::set<std::string>& targetLaws) const
{
	if (targetLaws.empty())
	{
		return false;
	}

	return std::ranges::any_of(targetLaws, [laws](const std::string& targetLaw) {
		return laws.contains(targetLaw);
	});
}

std::map<std::string, int> V3::MarketTracker::getSubsistenceEmployment(const std::string& buildingName,
	 const std::set<std::string>& laws,
	 const std::map<std::string, ProductionMethodGroup>& PMGroups,
	 const std::map<std::string, ProductionMethod>& PMs,
	 const std::map<std::string, Building>& buildings) const
{
	const auto& subsistenceBuildingIter = buildings.find(buildingName);
	if (subsistenceBuildingIter == buildings.end())
	{
		Log(LogLevel::Warning) << "Subsistence Building: " << buildingName << " has no definition.";
	}

	std::map<std::string, int> subsistenceEmployment;

	const auto& subsistenceBuilding = subsistenceBuildingIter->second;
	for (const auto& PMGroup: subsistenceBuilding.getPMGroups())
	{
		const auto& PMName = PMGroups.at(PMGroup).getPMs()[getPMAllowedByLaws(PMGroup, laws, PMGroups, PMs)];
		const auto& thePM = PMs.at(PMName);
		for (const auto& [job, amount]: thePM.getEmployment())
		{
			subsistenceEmployment[job] += amount;
		}
	}

	return subsistenceEmployment;
}
