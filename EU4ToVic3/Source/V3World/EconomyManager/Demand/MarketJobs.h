#ifndef V3_MARKET_JOBS_H
#define V3_MARKET_JOBS_H

#include "PopManager/Pops/PopType.h"

namespace V3
{
class MarketJobs
{
  public:
	MarketJobs() = default;
	explicit MarketJobs(const std::map<std::string, double>& jobsList);

	[[nodsicard]] const auto& getPop() const { return population; }
	[[nodsicard]] const auto& getJobCounts() const { return jobCounts; }
	[[nodsicard]] std::map<std::string, double> getJobBreakdown() const;

	void createJobs(const PopType& popType, double amount, double defaultRatio, double womenJobRate);

  private:
	int population = 0;
	std::map<std::string, double> jobCounts;

	double hireFromWorseJobs(double amount);	// Returns the amount of jobs with no workers available.
	double hireFromUnemployed(double amount); // Returns the amount of jobs with no unemployed available.
	double hireFromPeasants(double amount);	// Returns the amount of jobs with no peasants available.
};
} // namespace V3

#endif // V3_MARKET_JOBS_H