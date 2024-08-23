#ifndef V3_PACKET_H
#define V3_PACKET_H
#include "Building/Building.h"
#include "ClayManager/State/SubState.h"

namespace V3
{
class Packet
{
  public:
	Packet(const Building& building,
		 int sectorBudget,
		 double clusterFactor,
		 const SubState& subState,
		 const std::vector<std::shared_ptr<SubState>>& subStates,
		 const std::map<std::string, Law>& lawsMap,
		 const std::map<std::string, Tech>& techMap,
		 const std::map<std::string, StateModifier>& traitMap,
		 const BuildingGroups& buildingGroups);

	[[nodiscard]] const auto& getSize() const { return size; }

  private:
	int getClusterPacket(int baseCost, double factor, const std::vector<std::shared_ptr<SubState>>& subStates) const;

	int size = 1;
};
} // namespace V3

#endif // V3_PACKET_H