#ifndef CLAY_MANAGER_H
#define CLAY_MANAGER_H
#include "Chunk.h"
#include "ClayMapTypedefs.h"
#include "ProvinceManager/ProvinceManager.h"
#include "ProvinceMapper/ProvinceMapper.h"
#include "StateLoader/State.h"
#include "SubState.h"
#include "SuperRegionLoader/V3SuperRegion.h"
#include <string>

namespace mappers
{
class CountryMapper;
}
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
	void unDisputeChunkOwnership(const std::map<std::string, std::shared_ptr<EU4::Country>>& sourceCountries);
	void distributeChunksAcrossSubStates();
	void assignSubStateOwnership(const std::map<std::string, std::shared_ptr<Country>>& countries, const mappers::CountryMapper& countryMapper);

	[[nodiscard]] const auto& getStates() const { return states; }
	[[nodiscard]] const auto& getSuperRegions() const { return superRegions; }
	[[nodiscard]] const auto& getChunks() const { return chunks; }
	[[nodiscard]] const auto& getSubStates() const { return substates; }

  private:
	[[nodiscard]] static std::map<std::string, double> calcChunkOwnerWeights(const std::shared_ptr<Chunk>& chunk);
	[[nodiscard]] std::pair<EU4TagToStateToProvinceMap, SourceOwners> sortChunkProvincesIntoTagStates() const;
	[[nodiscard]] std::vector<std::shared_ptr<SubState>> buildSubStates(const EU4TagToStateToProvinceMap& tagStateProvinces,
		 const SourceOwners& sourceOwners) const;

	std::map<std::string, std::shared_ptr<State>> states;					// geographical entities
	std::map<std::string, std::shared_ptr<SuperRegion>> superRegions; // geographical entities
	std::map<std::string, std::shared_ptr<State>> provincesToStates;	// handy map for quickly referencing states;

	std::vector<std::shared_ptr<Chunk>> chunks;		  // political entities
	std::vector<std::shared_ptr<SubState>> substates; // political entities
};
} // namespace V3
#endif // CLAY_MANAGER_H