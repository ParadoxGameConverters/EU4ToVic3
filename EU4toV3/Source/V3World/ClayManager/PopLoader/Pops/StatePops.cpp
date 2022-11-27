#include "StatePops.h"
#include <numeric>

int V3::StatePops::getPopCount() const
{
	return std::accumulate(subStatePops.begin(), subStatePops.end(), 0, [](int sum, const auto& subStatePop) {
		return sum + subStatePop.getPopCount();
	});
}
