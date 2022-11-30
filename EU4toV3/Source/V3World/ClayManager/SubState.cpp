#include "SubState.h"
#include "Log.h"
#include "V3World/PoliticalManager/CountryDefinitionLoader/Country.h"

V3::SubState::SubState(std::shared_ptr<State> theHomeState, std::shared_ptr<EU4::Country> theSourceOwner, ProvinceMap theProvinces):
	 state(std::move(theHomeState)), sourceOwner(std::move(theSourceOwner)), provinces(std::move(theProvinces))
{
}

std::optional<std::string> V3::SubState::getOwnerTag() const
{
	if (owner)
		return owner->getTag();
	return std::nullopt;
}

std::optional<std::string> V3::SubState::getSourceOwnerTag() const
{
	if (sourceOwner)
		return sourceOwner->getTag();
	return std::nullopt;
}

const std::string& V3::SubState::getHomeStateName() const
{
	if (state)
	{
		return state->getName();
	}
	else
	{
		static const std::string empty = "";
		Log(LogLevel::Warning) << "Attempted to access the name of a nullptr state from a substate. Returning empty name.";
		return empty;
	}
}