#ifndef V3_PROVINCE_COUNT_H
#define V3_PROVINCE_COUNT_H
#include "StateLoader/Province.h"
#include <map>
#include <ranges>


/* A ProvinceCount is a counter for a collection of provinces that measures multiple criteria in a single iteration.
 * It also provides conveinent syntactic labels for the resulting n-tuple of counts.
 *
 * It is primarily used to calculate landshare in substates.*/

namespace V3
{
struct ProvinceCount
{
	ProvinceCount(std::map<std::string, std::shared_ptr<Province>> provinces): every(provinces.size()), prime(0), impassable(0)
	{
		for (const auto& province: std::views::values(provinces))
		{
			if (province->isPrime())
			{
				++prime;
			}
			if (province->isImpassable())
			{
				++impassable;
			}
		}
	}

	int every;
	int prime;
	int impassable;
};
} // namespace V3

#endif // V3_PROVINCE_COUNT_H