#include "Chunk.h"
#include "ProvinceManager/EU4Province.h"
#include "SubState.h"
#include <numeric>
#include <ranges>

void V3::Chunk::addSourceProvinceData(const std::shared_ptr<EU4::Province>& sourceProvince, double weight)
{
	SourceProvinceData data;
	data.owner = sourceProvince->getOwnerTag();
	data.weight = sourceProvince->getProvinceWeight();
	data.investmentFactor = sourceProvince->getInvestmentFactor();
	data.popRatios = sourceProvince->getProvinceHistory().getPopRatios();
	data.buildings = sourceProvince->getBuildings();
	data.cores = sourceProvince->getCores();
	data.territorialCore = sourceProvince->isTerritorialCore();
	data.sea = sourceProvince->isSea();

	weightedSourceProvinceData.push_back(std::pair(data, weight));
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

bool V3::Chunk::isSea() const
{
	return std::ranges::any_of(weightedSourceProvinceData.begin(), weightedSourceProvinceData.end(), [](const auto& sourceProvince) {
		return sourceProvince.first.sea;
	});
}

double V3::Chunk::getTotalSourceProvinceWeight()
{
	const double totalWeight =
		 std::accumulate(weightedSourceProvinceData.begin(), weightedSourceProvinceData.end(), 0.0, [](double sum, const auto& sourceData) {
			 return sum + sourceData.first.weight;
		 });

	// This is important. We're averaging the total weight of our source provinces - right here. If by any chance in the future we want to add them together
	// or use some other function, alter this.

	return totalWeight / static_cast<double>(weightedSourceProvinceData.size());
}
