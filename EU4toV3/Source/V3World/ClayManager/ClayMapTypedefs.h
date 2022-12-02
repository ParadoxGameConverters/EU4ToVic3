#ifndef CLAY_MAP_TYPEDEFS_H
#define CLAY_MAP_TYPEDEFS_H
#include <map>
#include <memory>
#include <string>

namespace EU4
{
class Country;
}
namespace V3
{
class Province;
using ProvinceMap = std::map<std::string, std::shared_ptr<Province>>;					 // v3 province name->v3 province
using StateToProvinceMap = std::map<std::string, ProvinceMap>;								 // state name -> v3 provinces
using SourceOwners = std::map<std::string, std::shared_ptr<EU4::Country>>;				 // eu4tag, eu4country
} // namespace V3

#endif // CLAY_MAP_TYPEDEFS_H