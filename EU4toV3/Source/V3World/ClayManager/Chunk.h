#ifndef V3_CHUNK_H
#define V3_CHUNK_H
#include "Country/Country.h"
#include "StateLoader/Province.h"
#include "StateLoader/State.h"
#include <EU4World/CountryManager/EU4Country.h>
#include <map>
#include <memory>
#include <string>

/* A Chunk is a collection of provinces defined by province_mappings.txt where a particular eu4 province maps to.
 * Its defining attribute is the single owner these provinces belong to. From Chunks we can generate substates, when
 * we overlay this political map over a geographical map.
 *
 * A chunk can be as small as a single V3 province, or as large as multiple V3 states.*/

namespace V3
{
struct Chunk
{
	std::map<int, std::shared_ptr<EU4::Province>> sourceProvinces; // EU4 provinces
	std::map<std::string, std::shared_ptr<Province>> provinces;		// V3 provinces
	std::shared_ptr<EU4::Country> sourceOwner;
	std::shared_ptr<Country> owner;
	std::map<std::string, std::shared_ptr<State>> states;
};
} // namespace V3

#endif // V3_CHUNK_H
