#include "MarketJobs.h"

#include "ClayManager/State/SubState.h"

#include <numeric>

namespace
{
// For a given popType, what % of that popType have jobs?
double calculateWorkerDependencyRatio(const V3::PopType& popType, const double defaultRatio, const double womenJobRate)
{
	return popType.getDependentRatio().value_or(defaultRatio) + womenJobRate;
}
// How many people do x number of jobs support?
double getWorkersPlusDependents(const double amountOfJobs, const V3::PopType& popType, const double defaultRatio, const double womenJobRate)
{
	return amountOfJobs / calculateWorkerDependencyRatio(popType, defaultRatio, womenJobRate);
}
// Returns the amount of jobs with no unemployed available.
double hireFromUnemployed(const double amount, const std::shared_ptr<V3::SubState>& subState)
{
	const double unemployed = subState->getJob("unemployed");
	if (unemployed > amount)
	{
		subState->addJob("unemployed", -amount);
		return 0;
	}
	subState->setJob("unemployed", 0);
	return amount - unemployed;
}

} // namespace


V3::MarketJobs::MarketJobs(const std::vector<std::pair<std::string, int>>& manorHouseRoster): manorHouseRoster(manorHouseRoster)
{
}

// Returns levels of displaced subsistence building.
double V3::MarketJobs::createJobs(const std::map<std::string, double>& rgoUnitEmployment,
	 const std::map<std::string, double>& subsistenceUnitEmployment,
	 const int levels,
	 const double defaultRatio,
	 const double womenJobRate,
	 const std::map<std::string, double>& estimatedOwnerships,
	 const std::map<std::string, std::map<std::string, int>>& ownershipEmployments,
	 const std::map<std::string, PopType>& popTypes,
	 const std::shared_ptr<SubState>& subState)
{
	auto unitEmployment = rgoUnitEmployment;
	for (const auto& [ownershipBuildingName, fraction]: estimatedOwnerships)
	{
		if (!ownershipEmployments.contains(ownershipBuildingName))
		{
			if (ownershipEmploymentsErrorFlag.find(ownershipBuildingName) == ownershipEmploymentsErrorFlag.end())
			{
				Log(LogLevel::Error) << ownershipBuildingName << ": has no known employment data, economy generation will be broken.";
				ownershipEmploymentsErrorFlag.emplace(ownershipBuildingName);
			}
			continue;
		}

		for (const auto& [job, amount]: ownershipEmployments.at(ownershipBuildingName)) // Account for the owner of the buildings.
		{
			unitEmployment[job] += amount * fraction;
		}
	}
	for (const auto& [job, amount]: unitEmployment) // Track Dependents
	{
		unitEmployment[job] = getWorkersPlusDependents(amount, popTypes.at(job), defaultRatio, womenJobRate);
	}

	double totalPlaced = 0;
	for (const auto& [job, amount]: unitEmployment)
	{
		subState->addJob(job, amount * levels);
		totalPlaced += amount * levels;
	}
	return hireFromWorseJobs(totalPlaced, defaultRatio, womenJobRate, popTypes, subsistenceUnitEmployment, subState);
}

// post: The given subState's job estimate is initialized with the 0 buildings version of local employment.
// Returns number of subsistence building levels filled.
double V3::MarketJobs::createSubsistence(const std::map<std::string, double>& subsistenceUnitEmployment,
	 const double defaultRatio,
	 const double womenJobRate,
	 const int arableLand,
	 const std::map<std::string, PopType>& popTypes,
	 const std::shared_ptr<V3::SubState>& subState)
{
	std::map<std::string, double> unitEmployment;

	for (const auto& [job, amount]: subsistenceUnitEmployment)
	{
		unitEmployment[job] += amount;
	}
	for (const auto& [job, amount]: manorHouseRoster) // Peasants create manor house jobs.
	{
		unitEmployment[job] += amount;
	}
	for (const auto& [job, amount]: unitEmployment) // Track Dependents
	{
		unitEmployment[job] = getWorkersPlusDependents(amount, popTypes.at(job), defaultRatio, womenJobRate);
	}

	double unitEmploymentPop = std::accumulate(unitEmployment.begin(), unitEmployment.end(), 0.0, [](double sum, const auto& pair) {
		return sum + pair.second;
	});
	unitEmploymentPop = std::max(unitEmploymentPop, 1.0);
	const double unemployedPerUnit = subState->getSubStatePops().getPopCount() / unitEmploymentPop;
	const double levels = std::min(static_cast<double>(arableLand), unemployedPerUnit);

	for (const auto& [job, amount]: unitEmployment)
	{
		subState->addJob(job, amount * levels);
		subState->addJob("unemployed", amount * (unemployedPerUnit - levels));
	}

	return levels;
}

double V3::MarketJobs::hireFromWorseJobs(double amount,
	 const double defaultRatio,
	 const double womenJobRate,
	 const std::map<std::string, PopType>& popTypes,
	 const std::map<std::string, double>& subsistenceUnitEmployment,
	 const std::shared_ptr<V3::SubState>& subState)
{
	amount = hireFromUnemployed(amount, subState);
	return hireFromSubsistence(amount, subsistenceUnitEmployment, defaultRatio, womenJobRate, popTypes, subState);
}

double V3::MarketJobs::hireFromSubsistence(const double amount,
	 const std::map<std::string, double>& subsistenceUnitEmployment,
	 const double defaultRatio,
	 const double womenJobRate,
	 const std::map<std::string, PopType>& popTypes,
	 const std::shared_ptr<V3::SubState>& subState)
{
	// Use peasants as a proxy for subsistence worker presence.
	if (subState->getJob("peasants") == 0)
	{
		// Log(LogLevel::Warning) << "No subsistence workers available.";
		return 0;
	}

	// Accounts for Homesteading, 2.5% of jobs are Farmers per subsistence level.
	std::map<std::string, double> subsistenceCounts;
	double unitSubsistencePop = 0; // Amount of workers + dependents in a subsistence level
	for (const auto& [job, workers]: subsistenceUnitEmployment)
	{
		subsistenceCounts[job] = getWorkersPlusDependents(workers, popTypes.at(job), defaultRatio, womenJobRate);
		unitSubsistencePop += subsistenceCounts[job];
	}

	if (!subsistenceCounts.contains("peasants") || subsistenceCounts.at("peasants") <= 0)
	{
		if (!peasantErrorFlag)
		{
			Log(LogLevel::Error) << "Supposed subsistence building contains no peasants in its production methods. Job predictions will be unreliable.";
			peasantErrorFlag = true;
		}
		return 0;
	}

	const double lostSubsistenceLevels = std::min(amount / unitSubsistencePop, subState->getJob("peasants") / subsistenceCounts.at("peasants"));
	for (const auto& [job, people]: subsistenceCounts)
	{
		subState->addJob(job, -people * lostSubsistenceLevels);
	}
	// When subsistence levels are removed, Manor Houses downsize
	downsizeManorHouses(lostSubsistenceLevels, defaultRatio, womenJobRate, popTypes, subState);

	// if (const double remainder = amount - lostSubsistenceLevels * unitSubsistencePop; remainder > 0)
	//{
	//	Log(LogLevel::Warning) << "Could not find available workers for " << remainder << " jobs."; // Should never happen.
	// }

	return lostSubsistenceLevels;
}

void V3::MarketJobs::downsizeManorHouses(const double lostSubsistenceLevels,
	 const double defaultRatio,
	 const double womenJobRate,
	 const std::map<std::string, V3::PopType>& popTypes,
	 const std::shared_ptr<V3::SubState>& subState)
{
	for (const auto& [job, amount]: manorHouseRoster)
	{
		subState->addJob(job, -getWorkersPlusDependents(amount, popTypes.at(job), defaultRatio, womenJobRate) * lostSubsistenceLevels);
	}
}