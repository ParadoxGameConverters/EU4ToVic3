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
	void setInfrastructureCost(int theCost) { infrastructureCost = theCost; }

	[[nodiscard]] const auto& getName() { return name; }
	[[nodiscard]] const auto& getParent() { return parent; }
	[[nodiscard]] const auto& getInfrastructureCost() { return infrastructureCost; }


  private:
	void registerKeys();
	std::string name;
	std::string parent = "";
	int infrastructureCost = 0;
};
} // namespace V3

#endif // V3_BUILDING_GROUP_H
