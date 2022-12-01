#include "Chunk.h"
#include "ProvinceManager/EU4Province.h"
#include "SubState.h"
#include <ranges>

void V3::Chunk::importDemographics()
{
	// NOTE: At this stage we can only import EU4 popRatios into our scaled demographics.
	// We cannot do actual culture/religion conversion since chunks are region-less entities.
	//
	// Conversion of pops along with subsequent shaping will happen on substate level
	// as substates are regionally bound.

	double totalSourceDevelopmentWeight = 0;
	for (const auto& sourceProvince: sourceProvinces | std::views::values)
		totalSourceDevelopmentWeight += sourceProvince->getProvinceWeight();

	// Now, for every source province, import scaled demographics according to dev weight.
	// More dev in a mapping = more impact of province's culture.
	for (const auto& sourceProvince: sourceProvinces | std::views::values)
	{
		const auto provinceWeightRatio = sourceProvince->getProvinceWeight() / totalSourceDevelopmentWeight;
		auto popRatios = sourceProvince->getProvinceHistory().getPopRatios();
		if (popRatios.empty())
			continue;
		createDemographics(popRatios, provinceWeightRatio);
	}

	copyDemographicsToSubStates();
}

void V3::Chunk::createDemographics(const std::vector<EU4::PopRatio>& popRatios, double provinceWeightRatio)
{
	for (const auto& popRatio: popRatios)
	{
		Demographic demographic;
		demographic.culture = popRatio.getCulture();
		demographic.religion = popRatio.getReligion();
		demographic.upperRatio = popRatio.getUpperRatio() * provinceWeightRatio;
		demographic.middleRatio = popRatio.getMiddleRatio() * provinceWeightRatio;
		demographic.lowerRatio = popRatio.getLowerRatio() * provinceWeightRatio;

		demographics.push_back(demographic);
	}
}

void V3::Chunk::copyDemographicsToSubStates() const
{
	// This might be counterintuitive, but a demographic is not related to population size. It's a descriptor.
	// Here we're saying "original provinces had half culture A and half culture B, thus all substates will have
	// the same ratio. How *many* of the pops that turns out to be - that's up to the substate size in regards to
	// its home state size, various factors, all relating to some imported popcount.

	for (const auto& substate: substates)
		substate->setDemographics(demographics);
}
