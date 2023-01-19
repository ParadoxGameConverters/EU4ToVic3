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
	[[nodiscard]] const auto& getInfrastructureCost() const { return infrastructureCost; }
	[[nodiscard]] const auto& possibleIsResourceCapped() const { return resourceCapped; }


  private:
	void registerKeys();
	std::string name;
	std::optional<std::string> parent;
	std::optional<double> infrastructureCost;
	std::optional<bool> resourceCapped;
};
} // namespace V3

#endif // V3_BUILDING_GROUP_H
