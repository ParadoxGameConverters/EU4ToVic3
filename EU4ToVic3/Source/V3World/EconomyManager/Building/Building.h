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
	void setLevel(const int theLevel) { level = theLevel; }
	void setWeight(const double theWeight) { weight = theWeight; }
	void addPM(const std::string& pm) { PMs.emplace(pm); }
	void setPMGroups(const std::set<std::string>& pmgs) { PMGroups = pmgs; }

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getBuildingGroup() const { return buildingGroup; }
	[[nodiscard]] const auto& getPMGroups() const { return PMGroups; }
	[[nodiscard]] const auto& getPMs() const { return PMs; }
	[[nodiscard]] const auto& getConstructionCost() const { return constructionCost; }
	[[nodiscard]] const auto& getUnlockingTechs() const { return unlockingTechs; }
	[[nodiscard]] const auto& isBuildable() const { return buildable; }
	[[nodiscard]] const auto& isCappedByGov() const { return governmentCapped; }

	[[nodiscard]] const auto& getLevel() const { return level; }
	[[nodiscard]] const auto& getWeight() const { return weight; }

  private:
	void registerKeys(const std::map<std::string, int>& costTiers);

	// Static per type of building e.g. "building_port"
	std::string name;
	std::string buildingGroup;
	std::set<std::string> PMGroups;
	int constructionCost = 50;
	std::set<std::string> unlockingTechs;
	bool buildable = true;
	bool governmentCapped = false; // barracks, ports, other caps done at the building not building group level by techs/laws/traits.

	// Specific to instance of building in SubState e.g. The port in New York
	std::set<std::string> PMs; // Only Production Methods that are different from default
	int level = 0;
	double weight = 0;
};
} // namespace V3

#endif // V3_BUILDING_H