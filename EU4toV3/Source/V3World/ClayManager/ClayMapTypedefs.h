#ifndef CLAY_MAP_TYPEDEFS_H
#define CLAY_MAP_TYPEDEFS_H

#include "StateLoader/Province.h"
#include <map>

namespace EU4
{
class Country;
}
namespace V3
{
using ProvinceMap = std::map<std::string, std::shared_ptr<Province>>;					 // v3 province name->v3 province
using StateToProvinceMap = std::map<std::string, ProvinceMap>;								 // state name -> v3 provinces
using EU4TagToStateToProvinceMap = std::map<std::string, StateToProvinceMap>;			 // eu4 tag -> states and their provinces.
using ChunkToEU4TagToStateToProvinceMap = std::map<int, EU4TagToStateToProvinceMap>; // chunk -> whatever's generated from its provinces.
using SourceOwners = std::map<std::string, std::shared_ptr<EU4::Country>>;				 // eu4tag, eu4country
} // namespace V3

#endif // CLAY_MAP_TYPEDEFS_H