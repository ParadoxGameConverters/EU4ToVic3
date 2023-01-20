#ifndef V3_SECTOR_H
#define V3_SECTOR_H
#include "Parser.h"

namespace V3
{
// A Sector of industry is a group of vic3 buildings with similar characteristics and distribution
class SectorLoader;
class Country;
class Sector
{
  public:
	Sector(const SectorLoader& sectorRules, const Country& country);
	void calculateBudget(double totalWeight, int countryCPBudget);
	void spendCP(const int CPExpenditure) { CPBudget -= CPExpenditure; }

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getCPBudget() const { return CPBudget; }
	[[nodiscard]] const auto& getBuildings() const { return buildings; }
	[[nodiscard]] bool hasBuilding(const std::string& building) const;
	[[nodiscard]] auto getWeight() const { return weight; }

  private:
	std::string name;
	double weight = 0;
	int CPBudget = 0;
	std::set<std::string> buildings;
};
} // namespace V3

#endif // V3_MATH_SCRIPT_H