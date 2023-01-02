#ifndef V3_NATIONAL_BUDGET_LOADER_H
#define V3_NATIONAL_BUDGET_LOADER_H
#include "Parser.h"
#include "SectorLoader.h"

namespace V3
{
// A Sector of industry is a group of vic3 buildings with similar characteristics and distribution

class NationalBudgetLoader: commonItems::parser
{
  public:
	NationalBudgetLoader() = default;
	void loadNationalBudget(const std::string& filePath);

	[[nodiscard]] const auto& getSectorBlueprints() const { return sectorBlueprints; }

  private:
	void registerKeys();

	std::vector<std::shared_ptr<SectorLoader>> sectorBlueprints;
};
} // namespace V3

#endif // V3_NATIONAL_BUDGET_LOADER_H