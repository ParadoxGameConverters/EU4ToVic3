#ifndef V3_SUBSTATE_H
#define V3_SUBSTATE_H
#include "ClayManager/ClayMapTypedefs.h"
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
	void setProvinces(const ProvinceMap& theProvinces);
	void setSubStateType(const std::string& theType) { subStateType = theType; }

	void setSourceOwnerTag(const std::string& sourceTag) { sourceOwnerTag = sourceTag; }
	void setWeight(double theWeight) { weight = theWeight; }
	void setSourceProvinceData(const std::vector<std::pair<SourceProvinceData, double>>& theData) { weightedSourceProvinceData = theData; }

	void setLandshare(const double theLandshare) { landshare = theLandshare; }
	void setResource(const std::string& theResource, const int theAmount) { resources[theResource] = theAmount; }
	void setDemographics(const std::vector<Demographic>& demos) { demographics = demos; }
	void setSubStatePops(const SubStatePops& thePops) { subStatePops = thePops; }
	void addPop(const Pop& pop) { subStatePops.addPop(pop); }

	void convertDemographics(const ClayManager& clayManager,
		 const mappers::CultureMapper& cultureMapper,
		 const mappers::ReligionMapper& religionMapper,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader);

	void generatePops(int totalAmount);

	[[nodiscard]] const auto& getHomeState() const { return homeState; }
	[[nodiscard]] const auto& getOwner() const { return owner; }
	[[nodiscard]] const auto& getProvinces() const { return provinces; }
	[[nodiscard]] const auto& getSubStateType() const { return subStateType; }

	[[nodiscard]] const auto& getSourceOwnerTag() const { return sourceOwnerTag; }
	[[nodiscard]] const auto& getWeight() const { return weight; }
	[[nodiscard]] const auto& getSourceProvinceData() const { return weightedSourceProvinceData; }

	[[nodiscard]] const auto& getLandshare() const { return landshare; }
	[[nodiscard]] const auto& getInfrastructure() const { return infrastructure; }
	[[nodiscard]] const auto& getResource(const std::string& theResource) { return resources[theResource]; }
	[[nodiscard]] const auto& getTerrainFrequency(const std::string& theTerrain) { return terrainFrequency[theTerrain]; }
	[[nodiscard]] const auto& getDemographics() const { return demographics; }
	[[nodiscard]] const auto& getSubStatePops() const { return subStatePops; }

	[[nodiscard]] auto isIncorporated() const { return incorporated; }
	[[nodiscard]] auto isMarketCapital() const { return marketCapital; }
	[[nodiscard]] bool isCoastal() const;

	[[nodiscard]] std::optional<std::string> getOwnerTag() const;
	[[nodiscard]] const std::string& getHomeStateName() const;

  private:
	void calculateTerrainFrequency();
	void calculateInfrastructure(const std::map<std::string, std::shared_ptr<StateModifier>>& theStateModifiers);

	std::shared_ptr<State> homeState; // home state
	std::shared_ptr<Country> owner;
	ProvinceMap provinces; // V3 province codes
	std::string subStateType;

	std::optional<std::string> sourceOwnerTag; // we may or may not have this, depending on where substate is imported from.
	std::optional<double> weight;					 // imported scaled sourceProvinceWeight from whatever source province(s) point generally here.
	std::vector<std::pair<SourceProvinceData, double>> weightedSourceProvinceData;

	bool incorporated = true;
	bool marketCapital = false;
	double landshare = 0.0;									// % of State's resources that are substate's
	double infrastructure = 0.0;							// limits the amount of industry a substate can support
	std::map<std::string, int> resources;				// potential for a given resource based on landshare
	std::map<std::string, double> terrainFrequency; // Normalized vector (math-wise) of terrain in substate as %
	std::vector<Demographic> demographics;
	SubStatePops subStatePops;
};
} // namespace V3

#endif // V3_SUBSTATE_H
