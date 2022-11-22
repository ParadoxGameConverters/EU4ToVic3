#ifndef V3_SUBSTATE_H
#define V3_SUBSTATE_H
#include "StateLoader/Province.h"
#include "StateLoader/State.h"
#include <EU4World/CountryManager/EU4Country.h>
#include <map>
#include <memory>
#include <string>

/* A Substate is a cross-section across a set of chunks where all relevant chunk provinces fall within a geographical V3 state.
 * This means, if 5 eu4 provinces generated 5 vic3 chunks, those portions of those chunks that fall within a STATE_ALABAMA and belong
 * to a single owner would merge into a single TAG's STATE_ALABAMA substate, (bound by its single owner - TAG).
 * A substate can be as big as a geographical state, or as small as a single province within it.
 */

namespace V3
{
class Country;
struct SubState
{
	std::map<std::string, std::shared_ptr<Province>> provinces; // V3 province codes
	std::string ownerTag;													// V3 TAG
	std::shared_ptr<Country> owner;
	std::string sourceOwnerTag; // EU4 TAG
	std::shared_ptr<EU4::Country> sourceOwner;
	std::shared_ptr<State> state; // home state
	std::string stateName;

	friend std::ostream& operator<<(std::ostream& output, const SubState& substate);
};
} // namespace V3

#endif // V3_SUBSTATE_H
