#include "SubStatePops.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <numeric>

int V3::SubStatePops::getPopCount() const
{
	return std::accumulate(pops.begin(), pops.end(), 0, [](int sum, const auto& pop) {
		return sum + pop.getSize();
	});
}

int V3::SubStatePops::getSlavePopCount() const
{
	return std::accumulate(pops.begin(), pops.end(), 0, [](int sum, const auto& pop) {
		if (pop.getType() == "slaves")
			return sum + pop.getSize();
		else
			return sum;
	});
}

std::optional<std::string> V3::SubStatePops::getDominantCulture() const
{
	std::map<std::string, int> cultureCounts;

	for (const auto& pop: pops)
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

std::optional<std::string> V3::SubStatePops::getDominantReligion() const
{
	std::map<std::string, int> religionCounts;

	for (const auto& pop: pops)
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

void V3::SubStatePops::multiplyPops(double factor)
{
	std::vector<Pop> replacementPops;
	for (const auto& pop: pops)
	{
		auto newPop = pop;
		newPop.setSize(static_cast<int>(std::round(pop.getSize() * factor)));
		replacementPops.push_back(newPop);
	}
	pops.swap(replacementPops);
}
