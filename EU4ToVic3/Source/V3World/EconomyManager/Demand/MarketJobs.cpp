#include "MarketJobs.h"

#include <numeric>


V3::MarketJobs::MarketJobs(const std::map<std::string, double>& jobsList): jobCounts(jobsList)
{
	population = std::accumulate(jobCounts.begin(), jobCounts.end(), 0, [](int sum, const auto& pair) {
		return sum + static_cast<int>(pair.second);
	});
}

// Returns a map of each job as a percentage of all jobs in the market.
std::map<std::string, double> V3::MarketJobs::getJobBreakdown() const
{
	std::map<std::string, double> jobBreakdown;

	const auto thePop = population < 1 ? 1 : population;
	for (const auto& [job, amount]: jobCounts)
	{
		jobBreakdown[job] = amount / thePop;
	}

	return jobBreakdown;
}

void V3::MarketJobs::createJobs(const PopType& popType, double amount, const double defaultRatio, const double womenJobRate)
{
	amount /= (popType.getDependentRatio().value_or(defaultRatio) + womenJobRate);
	const double shortage = hireFromWorseJobs(amount);
	jobCounts[popType.getType()] += amount - shortage;
}

double V3::MarketJobs::hireFromWorseJobs(double amount)
{
	amount = hireFromUnemployed(amount);
	amount = hireFromPeasants(amount);
	return amount;
}

double V3::MarketJobs::hireFromUnemployed(double amount)
{
	const double unemployed = jobCounts.at("unemployed");
	if (unemployed > amount)
	{
		jobCounts["unemployed"] -= amount;
		return 0;
	}
	amount -= unemployed;
	jobCounts["unemployed"] = 0;
	return amount;
}

double V3::MarketJobs::hireFromPeasants(double amount)
{
	const double peasants = jobCounts.at("peasants");
	if (peasants > amount)
	{
		jobCounts["peasants"] -= amount;
		return 0;
	}
	amount -= peasants;
	jobCounts["peasants"] = 0;
	return amount;
}