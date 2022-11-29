#ifndef V3_SUBSTATE_H
#define V3_SUBSTATE_H
#include "ClayMapTypedefs.h"
#include "PoliticalManager/CountryDefinitionLoader/Country.h"
#include "PopManager/Demographic.h"
#include "StateLoader/State.h"
#include <EU4World/CountryManager/EU4Country.h>

/* A Substate is a cross-section across a set of chunks where all relevant chunk provinces fall within a geographical V3 state.
 * This means, if 5 eu4 provinces generated 5 vic3 chunks, those portions of those chunks that fall within a STATE_ALABAMA and belong
 * to a single owner would merge into a single TAG's STATE_ALABAMA substate, (bound by its single owner - TAG).
 * A substate can be as big as a geographical state, or as small as a single province within it.
 */

namespace V3
{
class Country;
class SubState
{
  public:
	SubState() = default;
	SubState(std::shared_ptr<State> theHomeState, std::shared_ptr<EU4::Country> theSourceOwner, ProvinceMap theProvinces);
	void setProvinces(const ProvinceMap& theProvinces) { provinces = theProvinces; }
	void setOwner(const std::shared_ptr<Country>& theOwner) { owner = theOwner; }
	void setLandshare(const double theLandshare) { landshare = theLandshare; }
	void setResource(const std::string& theResource, int theAmount) { resources[theResource] = theAmount; }
	void setChunk(const std::shared_ptr<Chunk>& theChunk) { chunk = theChunk; }
	void setDemographics(const std::vector<Demographic>& demos) { demographics = demos; }

	[[nodiscard]] const auto& getProvinces() const { return provinces; }
	[[nodiscard]] const auto& getOwner() const { return owner; }
	[[nodiscard]] const auto& getSourceOwner() const { return sourceOwner; }
	[[nodiscard]] const auto& getHomeState() const { return state; }
	[[nodiscard]] const auto& getChunk() const { return chunk; }
	[[nodiscard]] auto getLandshare() const { return landshare; }
	[[nodiscard]] const auto& getResource(const std::string& theResource) { return resources[theResource]; }
	[[nodiscard]] const auto& getDemographics() { return demographics; }

	[[nodiscard]] std::optional<std::string> getOwnerTag() const;
	[[nodiscard]] std::optional<std::string> getSourceOwnerTag() const;
	[[nodiscard]] const std::string& getHomeStateName() const;

  private:
	std::shared_ptr<State> state; // home state
	std::shared_ptr<EU4::Country> sourceOwner;
	std::shared_ptr<Country> owner;
	ProvinceMap provinces; // V3 province codes
	std::shared_ptr<Chunk> chunk;

	double landshare = 0; // % of State's resources that are substate's
	std::map<std::string, int> resources;
	std::vector<Demographic> demographics;
};
} // namespace V3

#endif // V3_SUBSTATE_H
