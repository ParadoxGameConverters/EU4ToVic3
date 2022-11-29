#include "StatePops.h"
#include <algorithm>
#include <map>
#include <numeric>

int V3::StatePops::getPopCount() const
{
	return std::accumulate(subStatePops.begin(), subStatePops.end(), 0, [](int sum, const auto& subStatePop) {
		return sum + subStatePop.getPopCount();
	});
}

std::optional<std::string> V3::StatePops::getDominantCulture() const
{
	std::map<std::string, int> cultureCounts;

	for (const auto& subStatePop: subStatePops)
		for (const auto& pop: subStatePop.getPops())
		{
			if (cultureCounts.contains(pop.getCulture()))
				cultureCounts.at(pop.getCulture()) += pop.getSize();
			else
				cultureCounts.emplace(pop.getCulture(), pop.getSize());
		}

	if (cultureCounts.empty())
		return std::nullopt;

	const auto highest = std::max_element(std::begin(cultureCounts), std::end(cultureCounts), [](const auto& p1, const auto& p2) {
		return p1.second < p2.second;
	});

	return highest->first;
}
