#ifndef V3_NATIONAL_BUDGET_LOADER_H
#define V3_NATIONAL_BUDGET_LOADER_H
#include "Parser.h"
#include "SectorLoader.h"

namespace V3
{
class NationalBudgetLoader: commonItems::parser
{
  public:
	NationalBudgetLoader() = default;
	void loadNationalBudget(const std::string& filePath);
	void buildBuildingSectorMap();

	[[nodiscard]] const auto& getSectorBlueprints() const { return sectorBlueprints; }
	[[nodiscard]] const auto& getBuildingSectorMap() const { return buildingSectorMap; }
	[[nodiscard]] std::optional<std::string> getSectorName(const std::string& buildingName) const;

  private:
	void registerKeys();

	std::vector<SectorLoader> sectorBlueprints;
	std::map<std::string, std::string> buildingSectorMap; // Building Name -> Name of Sector Building is in
};
} // namespace V3

#endif // V3_NATIONAL_BUDGET_LOADER_H