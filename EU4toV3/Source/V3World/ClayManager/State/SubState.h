#ifndef V3_SUBSTATE_H
#define V3_SUBSTATE_H
#include "ClayManager/ClayMapTypedefs.h"
#include "PopManager/Demographic.h"
#include "PopManager/Pops/SubStatePops.h"
#include "SourceProvinceData.h"
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
} // namespace mappers
namespace V3
{
class Country;
class Chunk;
class State;
class StateModifier;
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
	void setUnincorporated() { subStateTypes.emplace("unincorporated"); }
	void setTreatyPort() { subStateTypes.emplace("treaty_port"); }
	void setVanillaSubState() { vanillaSubState = true; }
	void setLandshare(const double theLandshare) { landshare = theLandshare; }
	void setResource(const std::string& theResource, const int theAmount) { resources[theResource] = theAmount; }
	void setDemographics(const std::vector<Demographic>& demos) { demographics = demos; }
	void setSubStatePops(const SubStatePops& thePops) { subStatePops = thePops; }
	void addPop(const Pop& pop) { subStatePops.addPop(pop); }

	void setIndustryWeight(const double theIndustryWeight) { industryWeight = theIndustryWeight; }
	void setCPBudget(const int theCPBudget) { CPBudget = theCPBudget; }
	void setBuildingLevel(const std::string& building, const int level) { buildings[building] = level; }
	void setVanillaBuildingElements(const std::vector<std::string>& elements) { vanillaBuildingElements = elements; }

	void convertDemographics(const ClayManager& clayManager,
		 mappers::CultureMapper& cultureMapper,
		 const mappers::ReligionMapper& religionMapper,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader);

	void generatePops(int totalAmount);

	void calculateInfrastructure(const StateModifiers& theStateModifiers);

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
	[[nodiscard]] const auto& getResource(const std::string& theResource) { return resources[theResource]; }
	[[nodiscard]] double getTerrainFrequency(const std::string& theTerrain) const;
	[[nodiscard]] const auto& getTerrainFrequencies() { return terrainFrequency; }
	[[nodiscard]] const auto& getDemographics() const { return demographics; }
	[[nodiscard]] const auto& getSubStatePops() const { return subStatePops; }
	[[nodiscard]] std::optional<std::string> getPrimaryCulture() const;

	[[nodiscard]] const auto& getIndustryWeight() const { return industryWeight; }
	[[nodiscard]] const auto& getCPBudget() const { return CPBudget; }
	[[nodiscard]] std::optional<int> getBuildingLevel(const std::string& building) const;
	[[nodiscard]] const auto& getBuildings() const { return buildings; }
	[[nodiscard]] const auto& getVanillaBuildingElements() const { return vanillaBuildingElements; }

	[[nodiscard]] auto isVanillaSubState() const { return vanillaSubState; }
	[[nodiscard]] auto isIncorporated() const { return !subStateTypes.contains("unincorporated"); }
	[[nodiscard]] auto isTreatyPort() const { return !subStateTypes.contains("treaty_port"); }
	[[nodiscard]] auto isMarketCapital() const { return marketCapital; }
	[[nodiscard]] bool isCoastal() const;

	[[nodiscard]] std::optional<std::string> getOwnerTag() const;
	[[nodiscard]] const std::string& getHomeStateName() const;

  private:
	void calculateTerrainFrequency();
	[[nodiscard]] double getPopInfrastructure() const;
	[[nodiscard]] std::pair<int, double> getStateInfrastructureModifiers(const StateModifiers& theStateModifiers) const;

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
	std::map<std::string, int> resources;				// potential for a given resource based on landshare
	std::map<std::string, double> terrainFrequency; // Normalized vector (math-wise) of terrain in substate as %
	std::vector<Demographic> demographics;
	SubStatePops subStatePops;

	double industryWeight = 0;				  // Share of owner's industry a substate should get, not normalized
	int CPBudget = 0;							  // Construction Points for a substate to spend on it's development
	std::map<std::string, int> buildings; // building -> level

	std::vector<std::string> vanillaBuildingElements; // vanilla buildings for this substate, ready for direct dump.
	std::set<std::string> claims;
};
} // namespace V3

#endif // V3_SUBSTATE_H
