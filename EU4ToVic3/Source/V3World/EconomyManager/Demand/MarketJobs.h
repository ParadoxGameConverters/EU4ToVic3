#ifndef V3_MARKET_JOBS_H
#define V3_MARKET_JOBS_H

#include "PopManager/Pops/PopType.h"

namespace V3
{
class MarketJobs
{
  public:
	MarketJobs() = default;
	MarketJobs(const std::map<std::string, double>& jobsList, const std::vector<std::pair<std::string, int>>& manorHouseRoster);

	[[nodsicard]] const auto& getPop() const { return population; }
	[[nodsicard]] const auto& getJobCounts() const { return jobCounts; }
	[[nodsicard]] std::map<std::string, double> getJobBreakdown() const;

	void createJobs(const PopType& popType, double amount, double defaultRatio, double womenJobRate, int peasantsPerLevel);

  private:
	int population = 0;
	std::map<std::string, double> jobCounts;

	double hireFromWorseJobs(double amount, int peasantsPerLevel);			 // Returns the amount of jobs with no workers available.
	double hireFromUnemployed(double amount);										 // Returns the amount of jobs with no unemployed available.
	double hireFromPeasants(double amount, int peasantsPerLevel);			 // Returns the amount of jobs with no peasants available.
	void downsizeManorHouses(double peasantAmount, int peasantsPerLevel); // Removes employment from Manor houses based on # of peasants who got real jobs.

	std::vector<std::pair<std::string, int>> manorHouseRoster;
};
} // namespace V3

#endif // V3_MARKET_JOBS_H