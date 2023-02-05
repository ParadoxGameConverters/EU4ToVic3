#include "Chunk.h"
#include "ProvinceManager/EU4Province.h"
#include "SubState.h"
#include <algorithm>
#include <numeric>
#include <ranges>

void V3::Chunk::addSourceProvinceData(const EU4::Province& sourceProvince, double weight)
{
	SourceProvinceData data;
	data.owner = sourceProvince.getOwnerTag();
	data.weight = sourceProvince.getProvinceWeight();
	data.investmentFactor = sourceProvince.getInvestmentFactor();
	data.popRatios = sourceProvince.getProvinceHistory().getPopRatios();
	data.buildings = sourceProvince.getBuildings();
	data.cores = sourceProvince.getCores();
	data.territorialCore = sourceProvince.isTerritorialCore();
	data.sea = sourceProvince.isSea();
	data.eu4Capitals = sourceProvince.getCapitals();

	// Immediately apply weight to popratios as further splitting of this core data won't affect demographics.
	for (auto& popRatio: data.popRatios)
	{
		popRatio.multiplyRatios(weight);
	}

	weightedSourceProvinceData.emplace_back(data, weight);
}

std::map<std::string, double> V3::Chunk::calcOwnerWeights() const
{
	std::map<std::string, double> ownerWeights; // ownerTag, total province weight
	for (const auto& [sourceProvinceData, weight]: weightedSourceProvinceData)
	{
		const auto& sourceTag = sourceProvinceData.owner;
		if (sourceTag.empty())
			continue; // not relevant source - wasteland etc.
		if (ownerWeights.contains(sourceTag))
			ownerWeights.at(sourceTag) += weight; // this is RAW province weight - dev + buildings.
		else
			ownerWeights.emplace(sourceTag, weight);
	}
	return ownerWeights;
}

std::set<std::string> V3::Chunk::getKnownCapitals() const
{
	std::set<std::string> capitals;
	for (const auto& wspd: weightedSourceProvinceData | std::views::keys)
		capitals.insert(wspd.eu4Capitals.begin(), wspd.eu4Capitals.end());
	return capitals;
}

bool V3::Chunk::isSea() const
{
	return std::ranges::any_of(weightedSourceProvinceData.begin(), weightedSourceProvinceData.end(), [](const auto& sourceProvince) {
		return sourceProvince.first.sea;
	});
}

double V3::Chunk::getTotalSourceProvinceWeight() const
{
	const double totalWeight =
		 std::accumulate(weightedSourceProvinceData.begin(), weightedSourceProvinceData.end(), 0.0, [](double sum, const auto& sourceData) {
			 return sum + sourceData.first.weight;
		 });

	// This is important. We're averaging the total weight of our source provinces - right here. If by any chance in the future we want to add them together
	// or use some other function, alter this.

	return totalWeight / static_cast<double>(weightedSourceProvinceData.size());
}
