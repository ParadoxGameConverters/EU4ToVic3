#ifndef V3_MARKET_JOBS_H
#define V3_MARKET_JOBS_H

#include "PopManager/Pops/PopType.h"

namespace V3
{
class MarketJobs
{
  public:
	MarketJobs() = default;
	explicit MarketJobs(const std::vector<std::pair<std::string, int>>& manorHouseRoster);

	[[nodsicard]] const auto& getPop() const { return population; }
	[[nodsicard]] const auto& getJobCounts() const { return jobCounts; }
	[[nodsicard]] std::map<std::string, double> getJobBreakdown() const;

	void createJobs(const PopType& popType, double amount, double defaultRatio, double womenJobRate, int peasantsPerLevel);
	void createJobs(const std::map<std::string, int>& unitEmployment,
		 double defaultRatio,
		 double womenJobRate,
		 int peasantsPerLevel,
		 const std::map<std::string, double>& estimatedOwnerships,
		 const std::map<std::string, int>& ownershipEmployments,
		 const std::map<std::string, PopType>& popTypes);
	double createPeasants(const std::map<std::string, int>& subsistenceUnitEmployment,
		double defaultRatio,
		double womenJobRate,
		int arableLand,
		int subStatePop,
		const std::map<std::string, PopType>& popTypes);
	void clearJobs() { jobCounts.clear(); }
	void loadInitialJobs(const std::map<std::string, double>& jobsList);

  private:
	int population = 0;
	std::map<std::string, double> jobCounts;

	double hireFromWorseJobs(double amount, int peasantsPerLevel);			 // Returns the amount of jobs with no workers available.
	double hireFromUnemployed(double amount);										 // Returns the amount of jobs with no unemployed available.
	double hireFromPeasants(double amount, int peasantsPerLevel);			 // Returns the amount of jobs with no peasants available.
	void downsizeManorHouses(double peasantAmount, int peasantsPerLevel); // Removes employment from Manor Houses based on # of peasants who got real jobs.


	std::vector<std::pair<std::string, int>> manorHouseRoster;
};
} // namespace V3

#endif // V3_MARKET_JOBS_H