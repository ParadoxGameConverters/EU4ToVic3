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

int V3::StatePops::getSlavePopCount() const
{
	return std::accumulate(subStatePops.begin(), subStatePops.end(), 0, [](int sum, const auto& subStatePop) {
		return sum + subStatePop.getSlavePopCount();
	});
}

std::optional<std::string> V3::StatePops::getDominantCulture() const
{
	std::map<std::string, int> cultureCounts;

	for (const auto& subStatePop: subStatePops)
		for (const auto& pop: subStatePop.getPops())
		{
			if (pop.getCulture().empty())
				continue;
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

std::optional<std::string> V3::StatePops::getDominantReligion() const
{
	std::map<std::string, int> religionCounts;

	for (const auto& subStatePop: subStatePops)
		for (const auto& pop: subStatePop.getPops())
		{
			if (pop.getReligion().empty())
				continue;
			if (religionCounts.contains(pop.getReligion()))
				religionCounts.at(pop.getReligion()) += pop.getSize();
			else
				religionCounts.emplace(pop.getReligion(), pop.getSize());
		}

	if (religionCounts.empty())
		return std::nullopt;

	const auto highest = std::max_element(std::begin(religionCounts), std::end(religionCounts), [](const auto& p1, const auto& p2) {
		return p1.second < p2.second;
	});

	return highest->first;
}
