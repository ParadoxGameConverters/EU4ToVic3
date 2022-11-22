#include "ProvinceTypeCounter.h"

V3::ProvinceTypeCounter::ProvinceTypeCounter(std::map<std::string, std::shared_ptr<Province>> provinces): every(provinces.size()), prime(0), impassable(0)
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
