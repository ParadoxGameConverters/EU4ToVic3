#ifndef CLAY_MANAGER_H
#define CLAY_MANAGER_H
#include "Chunk.h"
#include "ProvinceManager/ProvinceManager.h"
#include "ProvinceMapper/ProvinceMapper.h"
#include "StateLoader/State.h"
#include "SubState.h"
#include "SuperRegionLoader/V3SuperRegion.h"
#include <map>
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

	[[nodiscard]] bool regionIsValid(const std::string& region) const;
	[[nodiscard]] bool stateIsInRegion(const std::string& state, const std::string& region) const;

  private:
	using ProvinceMap = std::map<std::string, std::shared_ptr<Province>>;			// v3 province name->v3 province
	using StateToProvinceMap = std::map<std::string, ProvinceMap>;						// state name -> v3 provinces
	using EU4TagToStateToProvinceMap = std::map<std::string, StateToProvinceMap>; // eu4 tag -> states and their provinces.
	using SourceOwners = std::map<std::string, std::shared_ptr<EU4::Country>>;		// eu4tag, eu4country

	[[nodiscard]] static std::map<std::string, double> calcChunkOwnerWeights(const std::shared_ptr<Chunk>& chunk);
	[[nodiscard]] std::pair<EU4TagToStateToProvinceMap, SourceOwners> sortChunkProvincesIntoTagStates() const;
	[[nodiscard]] std::vector<std::shared_ptr<SubState>> buildSubStates(const EU4TagToStateToProvinceMap& tagStateProvinces,
		 const SourceOwners& sourceOwners) const;
	[[nodiscard]] static bool isChunkSea(const std::shared_ptr<Chunk>& chunk);

	std::map<std::string, std::shared_ptr<State>> states;					// geographical entities
	std::map<std::string, std::shared_ptr<SuperRegion>> superRegions; // geographical entities
	std::map<std::string, std::shared_ptr<State>> provincesToStates;	// handy map for quickly referencing states;

	std::vector<std::shared_ptr<Chunk>> chunks;		  // political entities
	std::vector<std::shared_ptr<SubState>> substates; // political entities
};
} // namespace V3
#endif // CLAY_MANAGER_H