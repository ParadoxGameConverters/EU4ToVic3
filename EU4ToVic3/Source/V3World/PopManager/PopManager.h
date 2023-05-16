#ifndef POP_MANAGER_H
#define POP_MANAGER_H
#include "Configuration.h"
#include "MinorityPopMapper/MinorityPopMapper.h"
#include "ModLoader/ModFilesystem.h"
#include "Pops/StatePops.h"
#include "SlaveCultureMapper/SlaveCultureMapper.h"
#include <map>
#include <memory>
#include <string>

namespace EU4
{
class CultureLoader;
class ReligionLoader;
} // namespace EU4
namespace mappers
{
class CultureMapper;
class ReligionMapper;
struct CultureDef;
} // namespace mappers
namespace V3
{
class ClayManager;
class SubState;
class State;
class PoliticalManager;
class PopManager
{
  public:
	PopManager() = default;

	void initializeVanillaPops(const commonItems::ModFilesystem& modFS);
	void initializeDWPops(const commonItems::ModFilesystem& modFS);
	void convertDemographics(const ClayManager& clayManager,
		 mappers::CultureMapper& cultureMapper,
		 const mappers::ReligionMapper& religionMapper,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader) const;
	void generatePops(const ClayManager& clayManager, Configuration::POPSHAPES popShapes);
	void applyHomeLands(const ClayManager& clayManager) const;
	void loadMinorityPopRules(const std::string& filePath);
	void loadSlaveCultureRules(const std::string& filePath);
	void injectReligionsIntoVanillaPops(const std::map<std::string, mappers::CultureDef>& cultureDefs);
	void injectReligionsIntoDWPops(const std::map<std::string, mappers::CultureDef>& cultureDefs);
	void liberateSlaves(const PoliticalManager& politicalManager) const;
	void alterSlaveCultures(const PoliticalManager& politicalManager,
		 const ClayManager& clayManager,
		 const std::map<std::string, mappers::CultureDef>& cultureDefs) const;

	[[nodiscard]] std::string getDominantVanillaCulture(const std::string& stateName) const;
	[[nodiscard]] std::string getDominantVanillaReligion(const std::string& stateName) const;
	[[nodiscard]] std::string getDominantDWCulture(const std::string& stateName) const;
	[[nodiscard]] std::string getDominantDWReligion(const std::string& stateName) const;
	[[nodiscard]] std::optional<SubStatePops> getVanillaSubStatePops(const std::string& stateName, const std::string& ownerTag) const;
	[[nodiscard]] std::optional<SubStatePops> getDWSubStatePops(const std::string& stateName, const std::string& ownerTag) const;

  private:
	void generatePopsForShovedSubStates(const std::shared_ptr<State>& state, int unassignedPopCount, int unassignedProvinceCount) const;
	void generatePopsForNormalSubStates(const std::shared_ptr<State>& state, int unassignedPopCount, int unassignedSlavePopCount) const;
	[[nodiscard]] int generatePopCountForShovedSubState(const std::shared_ptr<SubState>& subState, int unassignedPopCount, int unassignedProvinces) const;
	[[nodiscard]] int generatePopCountForNormalSubState(const std::shared_ptr<SubState>& subState, int unassignedPopCount) const;
	void filterVanillaMinorityStatePops();
	[[nodiscard]] std::vector<Pop> generateMinorityPops(const std::string& stateName,
		 double neededPopSizeTotal,
		 const std::optional<std::string>& dominantCulture,
		 const std::optional<std::string>& dominantReligion) const;
	[[nodiscard]] static std::map<std::string, StatePops> injectReligionsIntoPops(const std::map<std::string, mappers::CultureDef>& cultureDefs,
		 const std::map<std::string, StatePops>& pops);
	[[nodiscard]] std::map<std::string, int> getVanillaSuperRegionalPopCounts(const ClayManager& clayManager) const;
	[[nodiscard]] static std::map<std::string, double> getVanillaSuperRegionalWeights(const ClayManager& clayManager);
	[[nodiscard]] std::map<std::string, double> getSuperRegionPopShapingProjections(const ClayManager& clayManager) const;

	std::map<std::string, StatePops> vanillaStatePops;			  // state, StatePops
	std::map<std::string, StatePops> vanillaMinorityStatePops; // state, StatePops
	std::map<std::string, StatePops> dwStatePops;				  // decentralized world - used for shoved and imported states only!

	mappers::MinorityPopMapper minorityPopMapper;
	mappers::SlaveCultureMapper slaveCultureMapper;
};
} // namespace V3
#endif // POP_MANAGER_H
