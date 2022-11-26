#include "Substate.h"
#include "V3World/PoliticalManager/CountryDefinitionLoader/Country.h"

V3::SubState::SubState(std::shared_ptr<State> theHomeState, std::shared_ptr<EU4::Country> theSourceOwner, const ProvinceMap& theProvinces):
	 state(theHomeState), owner(nullptr), sourceOwner(theSourceOwner), provinces(theProvinces), resources()
{
}

const std::string& V3::SubState::getOwnerTag() const
{
	return owner->getTag();
}

const std::string& V3::SubState::getSourceOwnerTag() const
{
	return sourceOwner->getTag();
}

const std::string& V3::SubState::getHomeStateName() const
{
	return state->getName();
}