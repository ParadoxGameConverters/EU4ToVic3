#include "SubState.h"
#include "Log.h"
#include "V3World/PoliticalManager/CountryDefinitionLoader/Country.h"

V3::SubState::SubState(std::shared_ptr<State> theHomeState, std::shared_ptr<EU4::Country> theSourceOwner, const ProvinceMap& theProvinces):
	 state(theHomeState), owner(nullptr), sourceOwner(theSourceOwner), provinces(theProvinces), resources()
{
}

const std::string& V3::SubState::getOwnerTag() const
{
	if (owner)
	{
		return owner->getTag();
	}
	else
	{
		static const std::string empty = "";
		Log(LogLevel::Warning) << "Attempted to access the TAG of a nullptr V3 country from a substate. Returning empty TAG.";
		return empty;
	}
}

const std::string& V3::SubState::getSourceOwnerTag() const
{
	if (sourceOwner)
	{

		return sourceOwner->getTag();
	}
	else
	{
		static const std::string empty = "";
		Log(LogLevel::Warning) << "Attempted to access the TAG of a nullptr EU4 country from a substate. Returning empty TAG.";
		return empty;
	}
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