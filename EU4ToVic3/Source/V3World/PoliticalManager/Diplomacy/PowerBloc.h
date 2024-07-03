#ifndef POWERBLOC_H
#define POWERBLOC_H
#include "Date.h"

namespace V3
{
struct PowerBloc
{
	PowerBloc() = default;
	explicit PowerBloc(std::string _owner,
		 std::set<std::string> _members,
		 std::string _identity,
		 std::string _name,
		 std::string _principle,
		 const std::optional<commonItems::Color>& _color,
		 const date& _start_date):
		 identity(std::move(_identity)),
		 owner(std::move(_owner)), name(std::move(_name)), principle(std::move(_principle)), color(_color), members(std::move(_members)), start_date(_start_date)
	{
	}
	std::string identity;
	std::string owner;
	std::string name;
	std::string principle;
	std::optional<commonItems::Color> color;
	std::set<std::string> members;
	date start_date;
	std::map<std::string, std::string> localizations; // language, league_name. Always exported to <owner>_BLOC to avoid collisions.
};
} // namespace V3

#endif // POWERBLOC_H