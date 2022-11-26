#ifndef V3_SUBSTATE_H
#define V3_SUBSTATE_H
#include "ClayMapTypedefs.h"
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
	SubState(std::shared_ptr<State> theHomeState, std::shared_ptr<EU4::Country> theSourceOwner, const ProvinceMap& theProvinces);
	void setProvinces(const ProvinceMap& theProvinces) { provinces = theProvinces; }
	void setOwner(std::shared_ptr<Country> theOwner) { owner = theOwner; }
	void setLandshare(const double theLandshare) { landshare = theLandshare; }
	void setResource(const std::string& theResource, int theAmount) { resources[theResource] = theAmount; }

	[[nodiscard]] const auto& getProvinces() const { return provinces; }
	[[nodiscard]] const auto& getOwner() const { return owner; }
	[[nodiscard]] const auto& getSourceOwner() const { return sourceOwner; }
	[[nodiscard]] const auto& getHomeState() const { return state; }
	[[nodiscard]] const auto& getLandshare() const { return landshare; }
	[[nodiscard]] const auto& getResource(const std::string& theResource) { return resources[theResource]; }

	[[nodiscard]] const std::string& getOwnerTag() const;
	[[nodiscard]] const std::string& getSourceOwnerTag() const;
	[[nodiscard]] const std::string& getHomeStateName() const;

  private:
	ProvinceMap provinces; // V3 province codes
	std::shared_ptr<Country> owner;
	std::shared_ptr<EU4::Country> sourceOwner;
	std::shared_ptr<State> state; // home state

	double landshare = 0; // % of State's resources that are substate's
	std::map<std::string, int> resources;
};
} // namespace V3

#endif // V3_SUBSTATE_H
