#ifndef CLAY_MANAGER_H
#define CLAY_MANAGER_H
#include "Chunk.h"
#include "ProvinceManager/ProvinceManager.h"
#include "ProvinceMapper/ProvinceMapper.h"
#include "StateLoader/State.h"
#include "Substate.h"
#include "SuperRegionLoader/V3SuperRegion.h"
#include <map>
#include <string>

namespace V3
{
class ClayManager
{
  public:
	ClayManager() = default;

	void initializeVanillaStates(const std::string& v3Path);
	void loadTerrainsIntoProvinces(const std::string& v3Path);
	void initializeSuperRegions(const std::string& v3Path);
	void loadStatesIntoSuperRegions();
	void generateChunks(const mappers::ProvinceMapper& provinceMapper, const EU4::ProvinceManager& provinceManager);

	[[nodiscard]] const auto& getStates() const { return states; }
	[[nodiscard]] const auto& getSuperRegions() const { return superRegions; }
	[[nodiscard]] const auto& getChunks() const { return chunks; }

  private:
	std::map<std::string, std::shared_ptr<State>> states;					// geographical entities
	std::map<std::string, std::shared_ptr<SuperRegion>> superRegions; // geographical entities
	std::map<std::string, std::shared_ptr<State>> provincesToStates;	// handy map for quickly referencing states;

	std::vector<std::shared_ptr<Chunk>> chunks;		  // political entities
	std::vector<std::shared_ptr<SubState>> substates; // political entities
};
} // namespace V3
#endif // CLAY_MANAGER_H