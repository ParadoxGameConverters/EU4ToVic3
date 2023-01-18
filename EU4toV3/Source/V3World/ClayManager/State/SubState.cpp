#include "SubState.h"
#include "EconomyManager/Building/Building.h"
#include "EconomyManager/Building/BuildingGroup.h"
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

namespace
{
std::vector<std::string> sortMap(const std::map<std::string, int>& theMap)
{
	std::vector<std::string> sorted;

	std::vector<std::pair<std::string, int>> pairs;
	for (const auto& theElement: theMap)
		pairs.emplace_back(theElement);

	std::ranges::sort(pairs.begin(), pairs.end(), [=](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
		return a.second > b.second;
	});

	for (const auto& tag: pairs | std::views::keys)
		sorted.emplace_back(tag);

	return sorted;
}
} // namespace


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

	double theWeight = 0.0;
	// Weight due to terrain
	for (const auto& [terrain, frequency]: terrainFrequency)
	{
		if (!buildingTerrainModifiers.contains(terrain))
			continue;
		if (!buildingTerrainModifiers.at(terrain).contains(building))
			continue;

		theWeight += frequency * buildingTerrainModifiers.at(terrain).at(building);
	}
	return theWeight;
}

double V3::SubState::calcBuildingEU4Weight(const std::string& v3BuildingName, const mappers::BuildingMapper& buildingMapper) const
{
	// Weight from having EU4 buildings that map to VIC3 buildings in SubState weighted data.
	// For a substate made from 5 full chunks each containing a university, weight for building_university will be 5.

	double totalWeight = 0;
	for (const auto& [weightedData, dataWeight]: weightedSourceProvinceData)
		for (const auto& eu4building: weightedData.buildings)
			if (buildingMapper.getVic3Buildings(eu4building).contains(v3BuildingName))
				totalWeight += dataWeight;
	return totalWeight;
}

double V3::SubState::calcBuildingTraitWeight(const Building& building, const std::map<std::string, StateModifier>& traitMap, double traitStrength) const
{
	// Weight from having state traits that boost the building or building_group of building
	// Goods outputs will be done later in PM pass

	// Note: I have no idea what this function returns. -Z.

	double totalWeight = 0;
	const auto& buildingGroupName = building.getBuildingGroup();
	const auto& subStateTraits = getHomeState()->getTraits();
	for (const auto& trait: subStateTraits)
	{
		if (!traitMap.contains(trait))
			continue;
		const auto& modifier = traitMap.at(trait);

		// take building if present, otherwise take building group. Don't stack both.
		if (modifier.getBuildingModifiersMap().contains(building.getName()))
			totalWeight += modifier.getBuildingModifiersMap().at(building.getName()) * traitStrength;
		else if (modifier.getBuildingGroupModifiersMap().contains(buildingGroupName))
			totalWeight += modifier.getBuildingGroupModifiersMap().at(buildingGroupName) * traitStrength;
	}
	return totalWeight;
}

double V3::SubState::calcBuildingInvestmentWeight(const Building& building) const
{
	// Weight modifier due to amount already built of given Building
	// Simple percentage
	if (buildings.contains(building.getName()) && originalCPBudget > 0)
	{
		return std::max(1 - building.getConstructionCost() * buildings.at(building.getName()) / static_cast<double>(originalCPBudget), 0.0);
	}
	return 1;
}

double V3::SubState::calcBuildingIndustrialWeight(const Building& building, const BuildingGroups& buildingGroups) const
{
	const auto civLevelFactor = owner->getProcessedData().civLevel / 100;
	auto buildingGroup = buildingGroups.getBuildingGroupMap().at(building.getBuildingGroup());
	if (buildingGroup->getCategory())
	{
		if (*buildingGroup->getCategory() == "urban")
			return civLevelFactor;
		else
			return 1;
	}

	while (buildingGroup->getParentName())
	{
		buildingGroup = buildingGroups.getBuildingGroupMap().at(*buildingGroup->getParentName());
		if (buildingGroup->getCategory())
		{
			if (*buildingGroup->getCategory() == "urban")
				return civLevelFactor;
			else
				return 1;
		}
	}

	return 0.0;
}

bool V3::SubState::isBuildingValid(const Building& building, const BuildingGroups& buildingGroups) const
{
	// Government Admin is a special case, we're not building it
	if (building.getName() == "building_government_administration")
	{
		return false;
	}
	// TODO(Gawquon): Update when ports go off hardcode
	if (building.getName() == "building_port")
	{
		return false;
	}
	// We can only build what we have the tech for
	if (!getOwner()->hasAnyOfTech(building.getUnlockingTechs()))
	{
		return false;
	}
	if (!building.isBuildable())
	{
		return false;
	}
	if (building.getConstructionCost() > CPBudget)
	{
		return false;
	}
	if (!hasRGO(building))
	{
		return false;
	}
	if (!hasCapacity(building, buildingGroups))
	{
		return false;
	}

	return true;
}

bool V3::SubState::hasCapacity(const Building& building, const BuildingGroups& buildingGroups) const
{
	// If no cap on building, return true
	if (const auto& isCapped = buildingGroups.tryGetIsCapped(building.getBuildingGroup()); isCapped && !isCapped.value())
	{
		return true;
	}

	return getRGOCapacity(building, buildingGroups) > 0;
}

int V3::SubState::getRGOCapacity(const Building& building, const BuildingGroups& buildingGroups) const
{
	/*
	 NOTE: arable_land is stored in resources as bg_agriculture = #
	 all arable_resources have bg_agriculture as a parent group eventually, even if not directly
	 resources is stored in SubState, already calced from State and landshare


	 arable_land = 120
	 arable_resources = { "bg_wheat_farms" "bg_livestock_ranches" "bg_cotton_plantations" }
	 capped_resources = {
		  bg_sulfur_mining = 20
		  bg_logging = 2
		  bg_fishing = 6
	 }
	 */

	// A building uses the capacity of their building group if that capacity exists
	// otherwise they use the capacity of the parent of their building group
	// if their building group's parent doesn't have capacity keep checking up the chain until a capacity is found or no more parents exist.

	auto buildingGroupName = building.getBuildingGroup();
	auto buildingGroup = buildingGroups.getBuildingGroupMap().at(buildingGroupName);
	if (resources.contains(buildingGroupName))
		return resources.at(buildingGroupName);

	while (buildingGroup->getParentName())
	{
		buildingGroupName = *buildingGroup->getParentName();
		buildingGroup = buildingGroups.getBuildingGroupMap().at(buildingGroupName);
		if (resources.contains(buildingGroupName))
			return resources.at(buildingGroupName);
	}

	return 0;
}

bool V3::SubState::hasRGO(const Building& building) const
{
	if (const auto& arables = homeState->getArableResources(); std::ranges::find(arables, building.getBuildingGroup()) != arables.end())
		return true;

	return homeState->getCappedResources().contains(building.getBuildingGroup());
}

double V3::SubState::calcBuildingWeight(const Building& building,
	 const std::map<std::string, std::map<std::string, double>>& buildingTerrainModifiers,
	 const mappers::BuildingMapper& buildingMapper,
	 const std::map<std::string, StateModifier>& traitMap,
	 const double traitStrength) const
{
	const double terrainWeight = calcBuildingTerrainWeight(building.getName(), buildingTerrainModifiers);
	const double EU4BuildingWeight = calcBuildingEU4Weight(building.getName(), buildingMapper);
	const double traitWeight = calcBuildingTraitWeight(building, traitMap, traitStrength);
	const double investmentWeight = calcBuildingInvestmentWeight(building);

	return (terrainWeight + EU4BuildingWeight + traitWeight) * investmentWeight;
}

void V3::SubState::calculateInfrastructure(const StateModifiers& theStateModifiers, const std::map<std::string, Tech>& techMap)
{
	const double popInfra = getPopInfrastructure(techMap);
	auto [stateModBonus, stateModMultipliers] = getStateInfrastructureModifiers(theStateModifiers);

	// Principal = Base + isCoastal(substate lvl) + State modifier bonus + (Pop * tech)_capped
	const double infraBase = 3 + 2 * isCoastal() + stateModBonus + popInfra;

	// Multipliers are additive, market capital + incorporation status + state modifier multipliers
	double multipliers = 0.25 * marketCapital + -0.25 * !isIncorporated() + stateModMultipliers;
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

void V3::SubState::generatePops(int totalAmount, const int slaveAmount)
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
		if (totalAmount - slaveAmount >= 0)
		{
			auto pop = Pop(demo.culture, demo.religion, "", static_cast<int>(round(static_cast<double>(totalAmount - slaveAmount) * demoSum / demoTotal)));
			subStatePops.addPop(pop);
			if (slaveAmount > 0)
			{
				auto slavePop = Pop(demo.culture, demo.religion, "slaves", static_cast<int>(round(static_cast<double>(slaveAmount) * demoSum / demoTotal)));
				subStatePops.addPop(slavePop);
			}
		}
		else
		{
			Log(LogLevel::Warning) << "Substate in " << getHomeStateName() << " wants " << slaveAmount << " slaves but only gets " << totalAmount
										  << " pops! Bailing!";
		}
	}
}

std::optional<std::string> V3::SubState::getPrimaryCulture() const
{
	if (subStatePops.getPopCount() == 0)
		return std::nullopt;

	std::map<std::string, int> census;
	for (const auto& pop: subStatePops.getPops())
	{
		if (pop.getCulture().empty())
			continue;
		census[pop.getCulture()] += pop.getSize();
	}
	auto sorted = sortMap(census);
	return *sorted.begin();
}

std::set<std::string> V3::SubState::getProvinceIDs() const
{
	std::set<std::string> IDs;
	for (const auto& province: provinces | std::views::keys)
		IDs.emplace(province);
	return IDs;
}
