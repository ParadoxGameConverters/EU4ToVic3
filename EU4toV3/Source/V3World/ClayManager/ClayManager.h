#ifndef CLAY_MANAGER_H
#define CLAY_MANAGER_H
#include "ClayMapTypedefs.h"
#include <string>
#include <vector>

namespace EU4
{
class ProvinceManager;
}
namespace mappers
{
class CountryMapper;
class ProvinceMapper;
} // namespace mappers
namespace V3
{
class Country;
class Chunk;
class SubState;
class State;
class SuperRegion;
class ClayManager
{
  public:
	ClayManager() = default;

	void initializeVanillaStates(const std::string& v3Path);
	void loadTerrainsIntoProvinces(const std::string& v3Path);
	void initializeSuperRegions(const std::string& v3Path);
	void loadStatesIntoSuperRegions();
	void generateChunks(const mappers::ProvinceMapper& provinceMapper, const EU4::ProvinceManager& provinceManager);
	void unDisputeChunkOwnership(const SourceOwners& sourceCountries);
	void splitChunksIntoSubStates();

	void assignSubStateOwnership(const std::map<std::string, std::shared_ptr<Country>>& countries, const mappers::CountryMapper& countryMapper);

	[[nodiscard]] const auto& getStates() const { return states; }
	[[nodiscard]] const auto& getSuperRegions() const { return superRegions; }
	[[nodiscard]] const auto& getChunks() const { return chunks; }
	[[nodiscard]] const auto& getSubStates() const { return substates; }

	[[nodiscard]] bool regionIsValid(const std::string& region) const;
	[[nodiscard]] bool stateIsInRegion(const std::string& state, const std::string& region) const;

  private:
	[[nodiscard]] std::vector<std::shared_ptr<SubState>> chunkToSubStatesTransferFunction(const std::shared_ptr<Chunk>& chunk) const;
	[[nodiscard]] StateToProvinceMap sortChunkProvincesIntoStates(const std::shared_ptr<Chunk>& chunk) const;
	[[nodiscard]] std::vector<std::shared_ptr<SubState>> buildSubStates(const StateToProvinceMap& stateProvinceMap) const;

	std::map<std::string, std::shared_ptr<State>> states;					// geographical entities
	std::map<std::string, std::shared_ptr<SuperRegion>> superRegions; // geographical entities
	std::map<std::string, std::shared_ptr<State>> provincesToStates;	// handy map for quickly referencing states;

	std::vector<std::shared_ptr<Chunk>> chunks;		  // political entities
	std::vector<std::shared_ptr<SubState>> substates; // political entities
};
} // namespace V3
#endif // CLAY_MANAGER_H