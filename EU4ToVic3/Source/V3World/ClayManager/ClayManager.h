#ifndef CLAY_MANAGER_H
#define CLAY_MANAGER_H
#include "ClayMapTypedefs.h"
#include "CoastalMapper/CoastalMapper.h"
#include "Loaders/VanillaBuildingLoader/VanillaBuildingLoader.h"
#include "ModLoader/ModFilesystem.h"
#include "VNColonialMapper/VNColonialMapper.h"
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
class Region;
class VanillaStateEntry;
class PoliticalManager;
class PopManager;
class SubStatePops;
class ClayManager
{
  public:
	ClayManager() = default;

	void initializeVanillaStates(const commonItems::ModFilesystem& modFS);
	void loadTerrainsIntoProvinces(const commonItems::ModFilesystem& modFS);
	void initializeSuperRegions(const commonItems::ModFilesystem& modFS);
	void loadStatesIntoSuperRegions();
	void generateChunks(const mappers::ProvinceMapper& provinceMapper, const EU4::ProvinceManager& provinceManager);
	void unDisputeChunkOwnership(const SourceOwners& sourceCountries);
	void splitChunksIntoSubStates();
	void assignSubStateOwnership(const std::map<std::string, std::shared_ptr<Country>>& countries, const mappers::CountryMapper& countryMapper);
	void injectVanillaSubStates(const commonItems::ModFilesystem& modFS, const PoliticalManager& politicalManager, const PopManager& popManager, bool vn);
	void shoveRemainingProvincesIntoSubStates();
	void squashAllSubStates(const PoliticalManager& politicalManager);
	void redistributeResourcesAndLandshares();
	void addSubState(const std::shared_ptr<SubState>& subState) { substates.emplace_back(subState); }
	void loadAdjacencies(const std::string& filePath);

	[[nodiscard]] const auto& getStates() const { return states; }
	[[nodiscard]] const auto& getSuperRegions() const { return superRegions; }
	[[nodiscard]] const auto& getChunks() const { return chunks; }
	[[nodiscard]] const auto& getSubStates() const { return substates; }

	[[nodiscard]] bool regionIsValid(const std::string& region) const;
	[[nodiscard]] bool stateIsInRegion(const std::string& state, const std::string& region) const;
	[[nodiscard]] std::optional<std::string> getHistoricalCapitalState(const std::string& eu4tag) const;
	[[nodiscard]] std::shared_ptr<SuperRegion> getParentSuperRegion(const std::string& regionName) const;
	[[nodiscard]] std::optional<std::string> getParentRegionName(const std::string& stateName) const;
	[[nodiscard]] std::set<std::string> getStateNamesForRegion(const std::string& regionName) const;

	void loadVNColonialRules(const std::string& fileName) { vnColonialMapper.loadMappingRules(fileName); }
	[[nodiscard]] const auto& getVNColonialMapper() const { return vnColonialMapper; }
	[[nodiscard]] std::optional<std::string> getProvinceOwnerTag(const std::string& provinceID) const;
	[[nodiscard]] std::set<std::string> getStateProvinceIDs(const std::string& stateName) const;

	void filterInvalidClaims(const PoliticalManager& politicalManager) const;

  private:
	[[nodiscard]] std::vector<std::shared_ptr<SubState>> chunkToSubStatesTransferFunction(const std::shared_ptr<Chunk>& chunk) const;
	[[nodiscard]] StateToProvinceMap sortChunkProvincesIntoStates(const std::shared_ptr<Chunk>& chunk) const;
	[[nodiscard]] std::vector<std::shared_ptr<SubState>> buildSubStates(const StateToProvinceMap& stateProvinceMap) const;
	[[nodiscard]] bool importVanillaSubStates(const std::string& stateName,
		 const VanillaStateEntry& entry,
		 const ProvinceMap& unassignedProvinces,
		 const PoliticalManager& politicalManager,
		 const PopManager& popManager,
		 bool vn);
	void makeSubStateFromProvinces(const std::string& stateName, const ProvinceMap& unassignedProvinces);
	[[nodiscard]] SubStatePops prepareInjectedSubStatePops(const std::shared_ptr<SubState>& subState, double subStateRatio, const PopManager& popManager) const;
	[[nodiscard]] std::shared_ptr<SubState> squashSubStates(const std::vector<std::shared_ptr<SubState>>& subStates) const;

	std::map<std::string, std::shared_ptr<State>> states;					// geographical entities
	std::map<std::string, std::shared_ptr<SuperRegion>> superRegions; // geographical entities
	std::map<std::string, std::shared_ptr<State>> provincesToStates;	// handy map for quickly referencing states;
	std::map<int, std::string> stateIDs;										// handy!

	std::vector<std::shared_ptr<Chunk>> chunks;		  // political entities
	std::vector<std::shared_ptr<SubState>> substates; // political entities

	mappers::VNColonialMapper vnColonialMapper;
	mappers::CoastalMapper coastalMapper;
	VanillaBuildingLoader vanillaBuildingLoader;
};
} // namespace V3
#endif // CLAY_MANAGER_H