#ifndef V3_BUILDING_GROUP_H
#define V3_BUILDING_GROUP_H
#include "Parser.h"

namespace V3
{
class BuildingGroup: commonItems::parser
{
  public:
	BuildingGroup() = default;
	void loadBuildingGroup(std::istream& theStream);
	void setName(const std::string& theName) { name = theName; }
	void setInfrastructureCost(const std::optional<double> theCost) { infrastructureCost = theCost; }
	void setResourceCap(const std::optional<bool> isCapped) { resourceCapped = isCapped; }

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getParentName() const { return parent; }
	[[nodiscard]] const auto& getCategory() const { return category; }
	[[nodiscard]] const auto& getInfrastructureCost() const { return infrastructureCost; }
	[[nodiscard]] const auto& possibleIsResourceCapped() const { return resourceCapped; }
	[[nodiscard]] const auto& usesArableLand() const { return arableCapped; }
	[[nodiscard]] const auto& isSubsistence() const { return subsistence; }
	[[nodiscard]] const auto& getUrbanization() const { return urbanization; }

  private:
	void registerKeys();
	std::string name;
	std::optional<std::string> parent;
	std::optional<std::string> category;
	std::optional<double> infrastructureCost;
	std::optional<bool> resourceCapped;
	std::optional<bool> arableCapped;
	bool subsistence;
	int urbanization = 0;
};
} // namespace V3

#endif // V3_BUILDING_GROUP_H
