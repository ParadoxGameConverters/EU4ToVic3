#include "Substate.h"
#include "V3World/PoliticalManager/CountryDefinitionLoader/Country.h"

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