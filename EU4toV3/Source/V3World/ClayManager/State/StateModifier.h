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

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const bool hasMalaria() const { return malaria >= 1; }
	[[nodiscard]] const bool hasSevereMalaria() const { return malaria >= 2; }
	[[nodiscard]] const auto& getInfrastructureBonus() const { return infrastructure; }
	[[nodiscard]] const auto& getPortBonus() const { return port; }
	[[nodiscard]] const auto& getNavalBaseBonus() const { return navalBase; }
	[[nodiscard]] const std::optional<double> getThroughputModifier(const std::string& buildingGroup, std::shared_ptr<BuildingGroups> bgs) const;
	[[nodiscard]] const std::optional<double> getGoodsOutputModifier(const std::string& good) const;

  private:
	void registerKeys();

	std::string name; // state_trait_natural_harbors

	int malaria = 0; // 0 = none, 1 = Malaria, 2 = Severe Malaria
	int infrastructure = 0;
	int port = 0;
	int navalBase = 0;
	std::map<std::string, double> throughputModifiers;	 // +25.0% Agriculture Throughput
	std::map<std::string, double> goodsOutputModifiers; // +25.0% Building Goods Hardwood output
};
} // namespace V3

#endif // STATE_MODIFIER_H
