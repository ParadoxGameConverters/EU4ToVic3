#include "SubState.h"
#include "Log.h"
#include "Mappers/CultureMapper/CultureMapper.h"
#include "Mappers/ReligionMapper/ReligionMapper.h"
#include "PoliticalManager/Country/Country.h"
#include "Province.h"
#include "ProvinceManager/PopRatio.h"
#include "State.h"
#include "StateModifier.h"
#include <cmath>
#include <numeric>
#include <ranges>

#include "EconomyManager/Building/Building.h"


V3::SubState::SubState(std::shared_ptr<State> theHomeState, ProvinceMap theProvinces): homeState(std::move(theHomeState)), provinces(std::move(theProvinces))
{
	calculateTerrainFrequency();
}

void V3::SubState::setProvinces(const ProvinceMap& theProvinces)
{
	provinces = theProvinces;
	calculateTerrainFrequency();
}

double V3::SubState::getTerrainFrequency(const std::string& theTerrain) const
{
	if (const auto terrain = terrainFrequency.find(theTerrain); terrain != terrainFrequency.end())
	{
		return terrain->second;
	}
	return 0;
}

std::optional<int> V3::SubState::getBuildingLevel(const std::string& building) const
{
	if (buildings.contains(building))
		return buildings.at(building);
	return std::nullopt;
}

bool V3::SubState::isCoastal() const
{
	if (const auto& coastalFreq = terrainFrequency.find("coastal"); coastalFreq != terrainFrequency.end())
	{
		return coastalFreq->second > 0;
	}
	return false;
}

std::optional<std::string> V3::SubState::getOwnerTag() const
{
	if (owner)
		return owner->getTag();
	return std::nullopt;
}

const std::string& V3::SubState::getHomeStateName() const
{
	if (homeState)
	{
		return homeState->getName();
	}
	else
	{
		static const std::string empty;
		Log(LogLevel::Warning) << "Attempted to access the name of a nullptr state from a substate. Returning empty name.";
		return empty;
	}
}

void V3::SubState::calculateTerrainFrequency()
{
	for (const auto& province: std::views::values(provinces))
	{
		if (province->isCoastal())
		{
			// Create in effect coastal_mountains, coastal_plains, etc. terrain.
			terrainFrequency["coastal"] += 0.5;
			terrainFrequency[province->getTerrain()] += 0.5;
		}
		else
		{
			terrainFrequency[province->getTerrain()] += 1;
		}
	}

	const auto& frequencies = std::views::values(terrainFrequency);
	const double total = std::reduce(frequencies.begin(), frequencies.end());

	for (const auto& [terrain, count]: terrainFrequency)
	{
		terrainFrequency[terrain] = count / total;
	}
}

double V3::SubState::getPopInfrastructure(const std::map<std::string, Tech>& techMap) const
{
	// INDIVIDUALS_PER_POP_INFRASTRUCTURE = 10000
	const double popInfra = subStatePops.getPopCount() * owner->getTechInfraMult(techMap) / 10000.0;
	if (const int cap = owner->getTechInfraCap(techMap); popInfra > cap)
	{
		return cap;
	}
	return popInfra;
}

std::pair<int, double> V3::SubState::getStateInfrastructureModifiers(const StateModifiers& theStateModifiers) const
{
	int bonus = 0;
	double mult = 0;
	for (const auto& stateModifier: getHomeState()->getTraits())
	{
		if (!theStateModifiers.contains(stateModifier))
		{
			// should never happen
			continue;
		}
		bonus += theStateModifiers.at(stateModifier)->getInfrastructureBonus();
		mult += theStateModifiers.at(stateModifier)->getInfrastructureMult();
	}
	return std::make_pair(bonus, mult);
}

double V3::SubState::calcBuildingTerrainWeight(const std::string& building,
	 const std::map<std::string, std::map<std::string, double>>& buildingTerrainModifiers) const
{
	// Weight due to terrain
	return 0.0;
}

double V3::SubState::calcBuildingEU4Weight(const std::string& building, const mappers::BuildingMapper& buildingMapper) const
{
	// Weight from having EU4 buildings that map to VIC3 buildings in SubState weighted data
	return 0.0;
}

double V3::SubState::calcBuildingTraitWeight(const std::shared_ptr<V3::Building>& building,
	 const std::map<std::string, std::shared_ptr<StateModifier>>& traitMap,
	 const double traitStrength) const
{
	// Weight from having state traits that boost the building or building_group of building
	// Goods outputs will be done later in PM pass
	return 0.0;
}

double V3::SubState::calcBuildingInfrastructureWeight(const std::shared_ptr<V3::Building>& building) const
{
	// Weight for infrastructure producing buildings when near the infrastructure limit
	// Hardcode to ports and rails until PM pass
	return 0.0;
}

void V3::SubState::calcBuildingWeight(const std::shared_ptr<V3::Building>& building,
	 const std::map<std::string, std::map<std::string, double>>& buildingTerrainModifiers,
	 const mappers::BuildingMapper& buildingMapper,
	 const std::map<std::string, std::shared_ptr<StateModifier>>& traitMap,
	 double traitStrength) const
{
	// sum all sub-functions functions, save to map of buildings -> weight
	// possibly clean this up with some typedefs
}

void V3::SubState::calculateInfrastructure(const StateModifiers& theStateModifiers, const std::map<std::string, Tech>& techMap)
{
	const double popInfra = getPopInfrastructure(techMap);
	auto [stateModBonus, stateModMultipliers] = getStateInfrastructureModifiers(theStateModifiers);

	// Principal = Base + isCoastal(substate lvl) + State modifier bonus + (Pop * tech)_capped
	const double infraBase = 3 + 2 * isCoastal() + stateModBonus + popInfra;

	// Multipliers are additive, market capital + incorporation status + state modifier multipliers
	double multipliers = 0.25 * marketCapital + -0.25 * !incorporated + stateModMultipliers;
	multipliers = std::max(0.0, multipliers + 1);

	infrastructure = infraBase * multipliers;
}

void V3::SubState::convertDemographics(const ClayManager& clayManager,
	 mappers::CultureMapper& cultureMapper,
	 const mappers::ReligionMapper& religionMapper,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader)
{
	// Just a few sanity checks which should never fail.
	if (!homeState || !owner)
		Log(LogLevel::Error) << "Not converting demographics in insane substate!";

	std::vector<Demographic> newDemographics;
	for (const auto& sourceData: weightedSourceProvinceData | std::views::keys)
	{
		for (const auto& popratio: sourceData.popRatios)
		{
			Demographic newDemo;
			// Religion
			auto religionMatch = religionMapper.getV3Religion(popratio.getReligion());
			if (!religionMatch)
				newDemo.religion = "noreligion";
			else
				newDemo.religion = *religionMatch;

			// Culture
			auto cultureMatch = cultureMapper.cultureMatch(clayManager,
				 cultureLoader,
				 religionLoader,
				 popratio.getCulture(),
				 popratio.getReligion(),
				 homeState->getName(),
				 owner->getTag(),
				 popratio.isNeoCulture());
			if (!cultureMatch)
			{
				// This should happen literally never unless a system error in one of the mapping rules.
				newDemo.culture = "noculture";
				if (popratio.isNeoCulture())
					Log(LogLevel::Warning) << "No neoculture match for: " << popratio.getCulture() << "/" << popratio.getReligion() << " in " << homeState->getName()
												  << " for " << owner->getTag();
				else
					Log(LogLevel::Warning) << "No culture match for: " << popratio.getCulture() << "/" << popratio.getReligion() << " in " << homeState->getName()
												  << " for " << owner->getTag();
			}
			else
				newDemo.culture = *cultureMatch;

			newDemo.upperRatio = popratio.getUpperRatio();
			newDemo.middleRatio = popratio.getMiddleRatio();
			newDemo.lowerRatio = popratio.getLowerRatio();

			newDemographics.push_back(newDemo);
		}
	}
	demographics.swap(newDemographics);
}

void V3::SubState::generatePops(int totalAmount)
{
	// At this moment we're not concerned with pop types. HOWEVER, demographics do carry a varying amount of ratios,
	// which are (were?) supposed to apply to those types.
	//
	// For now, we'll sum those ratios together and divide by 3, averaging them out.

	if (demographics.empty())
		return;

	// *technically* demoTotal should always be equal 1.
	const auto demoTotal = std::accumulate(demographics.begin(), demographics.end(), 0.0, [](double sum, const auto& demo) {
		return sum + (demo.upperRatio + demo.middleRatio + demo.lowerRatio) / 3;
	});

	// TODO: plug in poptypes via PopPoints.h. If needed.
	for (const auto& demo: demographics)
	{
		const double demoSum = (demo.upperRatio + demo.middleRatio + demo.lowerRatio) / 3;
		auto pop = Pop(demo.culture, demo.religion, "", static_cast<int>(round(static_cast<double>(totalAmount) * demoSum / demoTotal)));
		subStatePops.addPop(pop);
	}
}
