#ifndef V3_SUBSTATE_H
#define V3_SUBSTATE_H
#include "ClayManager/ClayMapTypedefs.h"
#include "EconomyManager/Building/Building.h"
#include "Loaders/LawLoader/Law.h"
#include "Loaders/TechLoader/Tech.h"
#include "Mappers/BuildingMapper/BuildingMapper.h"
#include "PopManager/Demographic.h"
#include "PopManager/Pops/SubStatePops.h"
#include "SourceProvinceData.h"
#include "StateModifier.h"
#include <optional>

/* A Substate is a cross-section across a set of chunks where all relevant chunk provinces fall within a geographical V3 state.
 * This means, if 5 eu4 provinces generated 5 vic3 chunks, those portions of those chunks that fall within a STATE_ALABAMA and belong
 * to a single owner would merge into a single TAG's STATE_ALABAMA substate, (bound by its single owner - TAG).
 * A substate can be as big as a geographical state, or as small as a single province within it.
 */

namespace EU4
{
class CultureLoader;
class ReligionLoader;
} // namespace EU4
namespace mappers
{
class CultureMapper;
class ReligionMapper;
class SlaveCultureMapper;
} // namespace mappers
namespace V3
{
class Country;
class Chunk;
class State;
class ClayManager;
class SubState
{
  public:
	SubState() = default;
	SubState(std::shared_ptr<State> theHomeState, ProvinceMap theProvinces);

	void setHomeState(const std::shared_ptr<State>& theState) { homeState = theState; }
	void setOwner(const std::shared_ptr<Country>& theOwner) { owner = theOwner; }
	void setCapital() { capital = true; }
	void addHistoricalCapitals(const std::set<std::string>& theCapitals) { eu4capitals = theCapitals; }
	void setProvinces(const ProvinceMap& theProvinces);
	void setSubStateTypes(const std::set<std::string>& theTypes) { subStateTypes = theTypes; }
	void addClaim(const std::string& tag) { claims.emplace(tag); }
	void setClaims(const std::set<std::string>& theClaims) { claims = theClaims; }

	void setSourceOwnerTag(const std::string& sourceTag) { sourceOwnerTag = sourceTag; }
	void setWeight(double theWeight) { weight = theWeight; }
	void setSourceProvinceData(const std::vector<std::pair<SourceProvinceData, double>>& theData) { weightedSourceProvinceData = theData; }

	void setMarketCapital() { marketCapital = true; }
	void setVanillaSubState() { vanillaSubState = true; }
	void setLandshare(const double theLandshare) { landshare = theLandshare; }
	void setResource(const std::string& theResource, const int theAmount) { resources[theResource] = theAmount; }
	void setDemographics(const std::vector<Demographic>& demos) { demographics = demos; }
	void setSubStatePops(const SubStatePops& thePops) { subStatePops = thePops; }
	void addPop(const Pop& pop) { subStatePops.addPop(pop); }
	void addPops(const std::vector<Pop>& pops) { subStatePops.addPops(pops); }
	void setVanillaPopCount(const int popCount) { vanillaPopCount = popCount; }

	void setIndustryWeight(const double theIndustryWeight) { industryWeight = theIndustryWeight; }
	void setCPBudget(const int theCPBudget) { CPBudget = theCPBudget; }
	void setOriginalCPBudget(const int theCPBudget) { originalCPBudget = theCPBudget; }
	void spendCPBudget(const int theCPExpense) { CPBudget -= theCPExpense; }
	void addBuilding(const std::shared_ptr<Building>& building) { buildings.push_back(building); }
	void setVanillaBuildingElements(const std::vector<std::string>& elements) { vanillaBuildingElements = elements; }
	void calculateInfrastructure(const StateModifiers& theStateModifiers, const std::map<std::string, Tech>& techMap);

	void gatherPossibleBuildings(const std::map<std::string, Building>& templateBuildings,
		 const BuildingGroups& buildingGroups,
		 const std::map<std::string, std::map<std::string, double>>& buildingTerrainModifiers,
		 const mappers::BuildingMapper& buildingMapper,
		 const std::map<std::string, Law>& lawsMap,
		 const std::map<std::string, Tech>& techMap,
		 const std::map<std::string, StateModifier>& traitMap,
		 double traitStrength);
	void weightBuildings(const BuildingGroups& buildingGroups,
		 const std::map<std::string, std::map<std::string, double>>& buildingTerrainModifiers,
		 const mappers::BuildingMapper& buildingMapper,
		 const std::map<std::string, Law>& lawsMap,
		 const std::map<std::string, Tech>& techMap,
		 const std::map<std::string, StateModifier>& traitMap,
		 double traitStrength);

	void convertDemographics(const ClayManager& clayManager,
		 mappers::CultureMapper& cultureMapper,
		 const mappers::ReligionMapper& religionMapper,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader);

	void generatePops(int totalAmount, int slaveAmount);

	[[nodiscard]] const auto& getHomeState() const { return homeState; }
	[[nodiscard]] const auto& getOwner() const { return owner; }
	[[nodiscard]] auto isCapital() const { return capital; }
	[[nodiscard]] const auto& getHistoricalCapitals() const { return eu4capitals; }
	[[nodiscard]] std::set<std::string> getProvinceIDs() const;
	[[nodiscard]] const auto& getProvinces() const { return provinces; }
	[[nodiscard]] const auto& getSubStateTypes() const { return subStateTypes; }
	[[nodiscard]] const auto& getClaims() const { return claims; }

	[[nodiscard]] const auto& getSourceOwnerTag() const { return sourceOwnerTag; }
	[[nodiscard]] const auto& getWeight() const { return weight; }
	[[nodiscard]] const auto& getSourceProvinceData() const { return weightedSourceProvinceData; }

	[[nodiscard]] const auto& getLandshare() const { return landshare; }
	[[nodiscard]] const auto& getInfrastructure() const { return infrastructure; }
	[[nodiscard]] int getResource(const std::string& theResource) const;
	[[nodiscard]] double getTerrainFrequency(const std::string& theTerrain) const;
	[[nodiscard]] double getOverPopulation() const;
	[[nodiscard]] const auto& getTerrainFrequencies() { return terrainFrequency; }
	[[nodiscard]] const auto& getDemographics() const { return demographics; }
	[[nodiscard]] const auto& getSubStatePops() const { return subStatePops; }
	[[nodiscard]] const auto& getVanillaPopCount() const { return vanillaPopCount; }
	[[nodiscard]] std::optional<std::string> getPrimaryCulture() const;

	[[nodiscard]] const auto& getIndustryWeight() const { return industryWeight; }
	[[nodiscard]] const auto& getCPBudget() const { return CPBudget; }
	[[nodiscard]] const auto& getBuildings() const { return buildings; }
	[[nodiscard]] const auto& getVanillaBuildingElements() const { return vanillaBuildingElements; }
	[[nodiscard]] double calcBuildingWeight(const Building& building,
		 const BuildingGroups& buildingGroups,
		 const std::map<std::string, std::map<std::string, double>>& buildingTerrainModifiers,
		 const mappers::BuildingMapper& buildingMapper,
		 const std::map<std::string, Law>& lawsMap,
		 const std::map<std::string, Tech>& techMap,
		 const std::map<std::string, StateModifier>& traitMap,
		 double traitStrength) const;
	[[nodiscard]] bool hasValidBuildings(const BuildingGroups& buildingGroups,
		 const std::map<std::string, Law>& lawsMap,
		 const std::map<std::string, Tech>& techMap,
		 const std::map<std::string, StateModifier>& traitMap) const;
	[[nodiscard]] int getBuildingCapacity(const Building& building,
		 const BuildingGroups& buildingGroups,
		 const std::map<std::string, Law>& lawsMap,
		 const std::map<std::string, Tech>& techMap,
		 const std::map<std::string, StateModifier>& traitMap) const;
	[[nodiscard]] double getTotalDev() const;

	[[nodiscard]] auto isVanillaSubState() const { return vanillaSubState; }
	[[nodiscard]] auto isIncorporated() const { return subStateTypes.contains("incorporated"); }
	[[nodiscard]] auto isTreatyPort() const { return subStateTypes.contains("treaty_port"); }
	[[nodiscard]] auto isMarketCapital() const { return marketCapital; }
	[[nodiscard]] bool isCoastal() const;

	[[nodiscard]] std::optional<std::string> getOwnerTag() const;
	[[nodiscard]] const std::string& getHomeStateName() const;

	[[nodiscard]] bool isStagedForMinorities() const { return stageForMinorities; }
	void setStageForMinorities(const bool stage) { stageForMinorities = stage; }

	[[nodiscard]] static bool greaterBudget(const std::shared_ptr<SubState>& lhs, const std::shared_ptr<SubState>& rhs);

	void setIncorporated(bool status);
	void setTreatyPort();
	void vacateTreatyPortPops();

  private:
	void calculateTerrainFrequency();
	[[nodiscard]] double getPopInfrastructure(const std::map<std::string, Tech>& techMap) const;
	[[nodiscard]] std::pair<int, double> getStateInfrastructureModifiers(const StateModifiers& theStateModifiers) const;

	void sortBuildingsByWeight();

	[[nodiscard]] double calcBuildingTerrainWeight(const std::string& building,
		 const std::map<std::string, std::map<std::string, double>>& buildingTerrainModifiers) const;
	[[nodiscard]] double calcBuildingEU4Weight(const std::string& v3BuildingName, const mappers::BuildingMapper& buildingMapper) const;
	[[nodiscard]] double calcBuildingTraitWeight(const Building& building, const std::map<std::string, StateModifier>& traitMap, double traitStrength) const;
	[[nodiscard]] double calcBuildingInvestmentWeight(const Building& building) const;
	[[nodiscard]] double calcBuildingIndustrialWeight(const Building& building, const BuildingGroups& buildingGroups) const;
	[[nodiscard]] double calcBuildingIncorporationWeight(const Building& building, const BuildingGroups& buildingGroups) const;
	[[nodiscard]] double calcBuildingOverPopulationWeight(const Building& building, const BuildingGroups& buildingGroups) const;
	[[nodiscard]] bool isBuildingValid(const Building& building,
		 const BuildingGroups& buildingGroups,
		 const std::map<std::string, Law>& lawsMap,
		 const std::map<std::string, Tech>& techMap,
		 const std::map<std::string, StateModifier>& traitMap) const;
	[[nodiscard]] bool hasCapacity(const Building& building,
		 const BuildingGroups& buildingGroups,
		 const std::map<std::string, Law>& lawsMap,
		 const std::map<std::string, Tech>& techMap,
		 const std::map<std::string, StateModifier>& traitMap) const;
	[[nodiscard]] int getRGOCapacity(const Building& building, const BuildingGroups& buildingGroups) const;
	[[nodiscard]] int getGovCapacity(const std::string& building, // barracks, ports things that have max capacities based on laws/tech/traits
		 const std::map<std::string, Law>& lawsMap,
		 const std::map<std::string, Tech>& techMap,
		 const std::map<std::string, StateModifier>& traitMap) const;
	[[nodiscard]] bool hasRGO(const Building& building, const BuildingGroups& buildingGroups) const;

	std::shared_ptr<State> homeState; // home state
	std::shared_ptr<Country> owner;
	bool capital = false;
	std::set<std::string> eu4capitals;	 // these eu4 tags had a capital in roughly this spot.
	ProvinceMap provinces;					 // V3 province codes
	std::set<std::string> subStateTypes; // unincorporated, treat_port...

	std::optional<std::string> sourceOwnerTag; // we may or may not have this, depending on where substate is imported from.
	std::optional<double> weight;					 // imported scaled sourceProvinceWeight from whatever source province(s) point generally here.
	std::vector<std::pair<SourceProvinceData, double>> weightedSourceProvinceData;

	bool vanillaSubState = false; // Is this substate imported from vanilla?
	bool marketCapital = false;
	double landshare = 0.0;									// % of State's resources that are substate's
	double infrastructure = 0.0;							// limits the amount of industry a substate can support
	std::map<std::string, int> resources;				// potential for a given resource based on landshare. {building_group -> #}
	std::map<std::string, double> terrainFrequency; // Normalized vector (math-wise) of terrain in substate as %
	std::vector<Demographic> demographics;
	SubStatePops subStatePops;
	int vanillaPopCount = 0; // What pop of substate would be without adjustments

	double industryWeight = 0;								  // Share of owner's industry a substate should get, not normalized
	int CPBudget = 0;											  // Construction Points for a substate to spend on its development
	int originalCPBudget = 0;								  // Used in Building Weight calculations
	std::vector<std::shared_ptr<Building>> buildings; // buildings available to build in the subState

	std::vector<std::string> vanillaBuildingElements; // vanilla buildings for this substate, ready for direct dump.
	std::set<std::string> claims;

	bool stageForMinorities = false; // runtime flag for pop processing
};
} // namespace V3

#endif // V3_SUBSTATE_H
