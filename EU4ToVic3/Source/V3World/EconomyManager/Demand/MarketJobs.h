#ifndef V3_MARKET_JOBS_H
#define V3_MARKET_JOBS_H

#include "PoliticalManager/Country/Country.h"
#include "PopManager/Pops/PopType.h"

namespace V3
{
class MarketJobs
{
  public:
	MarketJobs() = default;
	explicit MarketJobs(const std::vector<std::pair<std::string, int>>& manorHouseRoster);

	double createJobs(const std::map<std::string, double>& rgoUnitEmployment,
		 const std::map<std::string, double>& subsistenceUnitEmployment,
		 int levels,
		 double defaultRatio,
		 double womenJobRate,
		 const std::map<std::string, double>& estimatedOwnerships,
		 const std::map<std::string, std::map<std::string, int>>& ownershipEmployments,
		 const std::map<std::string, PopType>& popTypes,
		 const std::shared_ptr<SubState>& subState);
	double createSubsistence(const std::map<std::string, double>& subsistenceUnitEmployment,
		 double defaultRatio,
		 double womenJobRate,
		 int arableLand,
		 const std::map<std::string, PopType>& popTypes,
		 const std::shared_ptr<SubState>& subState);

  private:
	// Returns the level of subsistence buildings downsized.
	double hireFromWorseJobs(double amount,
		 double defaultRatio,
		 double womenJobRate,
		 const std::map<std::string, PopType>& popTypes,
		 const std::map<std::string, double>& subsistenceUnitEmployment,
		 const std::shared_ptr<SubState>& subState);
	// Returns the level of subsistence buildings downsized.
	double hireFromSubsistence(double amount,
		 const std::map<std::string, double>& subsistenceUnitEmployment,
		 double defaultRatio,
		 double womenJobRate,
		 const std::map<std::string, PopType>& popTypes,
		 const std::shared_ptr<V3::SubState>& subState);
	// Removes employment from Manor Houses based on # of peasants who got real jobs.
	void downsizeManorHouses(double lostSubsistenceLevels,
		 double defaultRatio,
		 double womenJobRate,
		 const std::map<std::string, PopType>& popTypes,
		 const std::shared_ptr<V3::SubState>& subState);

	std::vector<std::pair<std::string, int>> manorHouseRoster;

	inline static bool peasantErrorFlag = false;
	inline static std::set<std::string> ownershipEmploymentsErrorFlag = {};
};
} // namespace V3

#endif // V3_MARKET_JOBS_H