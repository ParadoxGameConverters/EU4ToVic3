#include "Packet.h"
#include <cmath>
#include <numeric>

V3::Packet::Packet(const Building& building,
	 const int sectorBudget,
	 const double clusterFactor,
	 const SubState& subState,
	 const std::vector<std::shared_ptr<SubState>>& subStates,
	 const std::map<std::string, Law>& lawsMap,
	 const std::map<std::string, Tech>& techMap,
	 const std::map<std::string, StateModifier>& traitMap,
	 const BuildingGroups& buildingGroups)
{
	const int sectorPacket = sectorBudget / building.getConstructionCost();
	const int subStatePacket = subState.getCPBudget() / building.getConstructionCost();
	const int capacityPacket = subState.getBuildingCapacity(building, buildingGroups, lawsMap, techMap, traitMap) - building.getLevel();
	const int clusterPacket = getClusterPacket(building.getConstructionCost(), clusterFactor, subStates);

	size = std::max(std::min({sectorPacket, subStatePacket, capacityPacket, clusterPacket}), 1);
}

int V3::Packet::getClusterPacket(const int baseCost, const double factor, const std::vector<std::shared_ptr<SubState>>& subStates) const
{
	if (subStates.empty())
		return 1;

	const int CPAll = std::accumulate(subStates.begin(), subStates.end(), 0, [](const int sum, const std::shared_ptr<SubState>& subState) {
		return sum + subState->getCPBudget();
	});
	const double CPMean = static_cast<double>(CPAll) / static_cast<double>(subStates.size());

	const int maxCP = subStates[0]->getCPBudget();
	const int minCP = std::max(subStates.back()->getCPBudget(), baseCost);

	// Default, when factor is 0
	int packet = static_cast<int>(CPMean / baseCost);

	if (factor < 0)
	{
		// Trends toward only building 1 building at a time
		packet = static_cast<int>(floor(CPMean * (1.0 + factor) + minCP * -factor) / baseCost);
	}
	if (factor > 0)
	{
		// Trends toward building as many buildings as the substate can get away with at a time
		packet = static_cast<int>(floor(CPMean * (1 - factor) + maxCP * factor) / baseCost);
	}

	return packet;
}
