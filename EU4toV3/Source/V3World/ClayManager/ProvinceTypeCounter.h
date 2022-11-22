#ifndef V3_PROVINCE_TYPE_COUNTER_H
#define V3_PROVINCE_TYPE_COUNTER_H
#include "StateLoader/Province.h"
#include <map>
#include <ranges>


/* A ProvinceTypeCounter measures the count of each type of province in a collection.
 * It also provides conveinent syntactic labels for the resulting n-tuple of counts.
 *
 * It is primarily used to calculate landshare in substates.*/

namespace V3
{
struct ProvinceTypeCounter
{
	ProvinceTypeCounter(std::map<std::string, std::shared_ptr<Province>> provinces);

	int every;
	int prime;
	int impassable;
};
} // namespace V3

#endif // V3_PROVINCE_TYPE_COUNTER_H