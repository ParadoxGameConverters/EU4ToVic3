#ifndef V3_PACKET_H
#define V3_PACKET_H
#include "Building/Building.h"
#include "ClayManager/State/SubState.h"

/* A Packet is the limiting factor which determines how many levels of a building a substate can buy in one iteration.
 * This prevents each substate from being too dominated by one type of industry. The result is simply the minimum of 4 independent factors
 *
 * 1. Sector Limit (How many buildings does the country as a whole need of this type, as defined by a configuration file?)
 * 2. Budgetary (How many levels can the substate afford by spending all of its points?)
 * 3. Capacity (Is this building type limited by resource availability or current laws?)
 * 4. Clustering (How much do we want industries to concentrate in fewer states? Set by configuration file.)
 */

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