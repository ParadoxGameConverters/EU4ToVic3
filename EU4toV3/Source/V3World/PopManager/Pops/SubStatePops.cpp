#include "SubStatePops.h"
#include <numeric>

int V3::SubStatePops::getPopCount() const
{
	return std::accumulate(pops.begin(), pops.end(), 0, [](int sum, const auto& pop) {
		return sum + pop.getSize();
	});
}
