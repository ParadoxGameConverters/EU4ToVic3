#include "Pop.h"
#include <numeric>

V3::Pop::Pop(std::string culture, std::string religion, std::string type, int size):
	 culture(std::move(culture)), religion(std::move(religion)), type(std::move(type)), size(size)
{
}

bool V3::Pop::operator==(const Pop& rhs) const
{
	return culture == rhs.culture && religion == rhs.religion && type == rhs.type;
}

int V3::SubStatePops::getPopCount() const
{
	return std::accumulate(pops.begin(), pops.end(), 0, [](int sum, const auto& pop) {
		return sum + pop.getSize();
	});
}

int V3::StatePops::getPopCount() const
{
	return std::accumulate(subStatePops.begin(), subStatePops.end(), 0, [](int sum, const auto& subStatePop) {
		return sum + subStatePop.getPopCount();
	});
}
