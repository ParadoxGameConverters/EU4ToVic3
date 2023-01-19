#ifndef V3_BUILDING_H
#define V3_BUILDING_H
#include "Parser.h"

namespace V3
{
class Building: commonItems::parser
{
  public:
	Building() = default;
	void loadBuilding(std::istream& theStream, const std::map<std::string, int>& costTiers);
	void setName(const std::string& theName) { name = theName; }

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getBuildingGroup() const { return buildingGroup; }
	[[nodiscard]] const auto& getPMGroups() const { return PMGroups; }
	[[nodiscard]] const auto& getConstructionCost() const { return constructionCost; }
	[[nodiscard]] const auto& getUnlockingTechs() const { return unlockingTechs; }
	[[nodiscard]] const auto& isBuildable() const { return buildable; }

  private:
	void registerKeys(const std::map<std::string, int>& costTiers);

	std::string name;
	std::string buildingGroup;
	std::vector<std::string> PMGroups;
	int constructionCost = 50;
	std::vector<std::string> unlockingTechs;
	bool buildable = true;
};
} // namespace V3

#endif // V3_BUILDING_H