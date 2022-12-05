#ifndef STATE_MODIFIER_H
#define STATE_MODIFIER_H
#include "Parser.h"
#include <Loaders/BuildingLoader/BuildingGroups.h>

namespace V3
{
class StateModifier: commonItems::parser
{
  public:
	StateModifier() = default;
	void loadStateModifier(std::istream& theStream);
	void setName(const std::string& theName) { name = theName; }

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getInfrastructureBonus() const { return infrastructure; }
	[[nodiscard]] const auto& getPortBonus() const { return port; }
	[[nodiscard]] const auto& getNavalBaseBonus() const { return navalBase; }
	[[nodiscard]] const std::optional<double> getBuildingGroupModifier(const std::string& buildingGroup, std::shared_ptr<BuildingGroups> bgs) const;
	[[nodiscard]] const std::optional<double> getBuildingModifier(const std::string& building) const;
	[[nodiscard]] const std::optional<double> getGoodsModifier(const std::string& good) const;

  private:
	void registerKeys();

	std::string name; // state_trait_natural_harbors

	int infrastructure = 0;
	double infrastructureModifier = 0.0;
	int port = 0;
	int navalBase = 0;
	std::map<std::string, double> buildingGroupModifiers; // building_group to throughput modifier
	std::map<std::string, double> buildingModifiers;		// building to throughput modifier
	std::map<std::string, double> goodsModifiers;			// good to goods throughtput modifier
};
} // namespace V3

#endif // STATE_MODIFIER_H
