#include "MarketTracker.h"
#include "ClayManager/State/State.h"
#include "ClayManager/State/SubState.h"

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
	marketJobs.clearJobs();
	marketCulture.clear();
}

void V3::MarketTracker::loadPeasants(const Country& country,
	 const std::map<std::string, ProductionMethodGroup>& PMGroups,
	 const std::map<std::string, ProductionMethod>& PMs,
	 const std::map<std::string, Building>& buildings)
{
	// from PM calc Jobs per arable land
	// multiply Unit Jobs per filled arable land
	// load in jobs
	// "unemployed" for all who don't fit on the arable land

	// For each state check peasant PM
	for (const auto& subState: country.getSubStates())
	{
		const std::string& subsistenceBuildingName = subState->getHomeState()->getSubsistenceBuilding();
		auto subsistenceEmployment = getSubsistenceEmployment(subsistenceBuildingName, country.getProcessedData().laws, PMGroups, PMs, buildings);

		if (!subsistenceEmployment.contains("peasants"))
		{
			if (!subsistenceErrors.contains(subsistenceBuildingName))
			{
				Log(LogLevel::Warning) << "Building: " << subsistenceBuildingName << " is marked as subsistence, yet has no peasants.";
				subsistenceErrors.emplace(subsistenceBuildingName);
			}
			continue;
		}

		marketJobs.createPeasants(subsistenceEmployment,
			10000000000000, // TODO load in defines
			10000000000000, // TODO do the law thingy
			subState->getResource("bg_agriculture"),
			subState->getSubStatePops().getPopCount(),
			{});

		// Get the arable land
		// marketJobs.
	}
}

void V3::MarketTracker::updatePopNeeds(const Vic3DefinesLoader& defines,
	 const DemandLoader& demandDefines,
	 const std::set<std::string>& laws,
	 const std::map<std::string, PopType>& popTypes,
	 const std::map<std::string, mappers::CultureDef>& cultures,
	 const std::map<std::string, mappers::ReligionDef>& religions,
	 const std::map<std::string, Law>& lawsMap)
{
	market.calcPopOrders(marketJobs.getPop(), marketJobs.getJobBreakdown(), marketCulture, defines, demandDefines, popTypes, cultures, religions, laws, lawsMap);
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
		for (const auto& [job, amount]: thePM.getEmployment()) // TODO(Gawquon) Note that we still need to add the market goods for subsistence farms
		{
			subsistenceEmployment[job] += amount;
		}
	}

	return subsistenceEmployment;
}
