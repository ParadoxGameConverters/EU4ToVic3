#ifndef CLAY_MAP_TYPEDEFS_H
#define CLAY_MAP_TYPEDEFS_H
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace EU4
{
class Country;
}
namespace V3
{
class SubState;
class Province;
class StateModifier;
using ProvinceMap = std::map<std::string, std::shared_ptr<Province>>;					// v3 province name->v3 province
using StateToProvinceMap = std::map<std::string, ProvinceMap>;								// state name -> v3 provinces
using SourceOwners = std::map<std::string, std::shared_ptr<EU4::Country>>;				// eu4tag, eu4country
using TagSubStates = std::map<std::string, std::vector<std::shared_ptr<SubState>>>; // tag, substate
using StateModifiers = std::map<std::string, std::shared_ptr<StateModifier>>;			// modifier name -> modifier
} // namespace V3

#endif // CLAY_MAP_TYPEDEFS_H