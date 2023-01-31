#include "ProductionMethodMapper.h"
#include "ClayManager/State/SubState.h"
#include "CommonRegexes.h"
#include "EconomyManager/Building/Building.h"
#include "EconomyManager/Building/ProductionMethods/ProductionMethod.h"
#include "EconomyManager/Building/ProductionMethods/ProductionMethodGroup.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "PoliticalManager/Country/Country.h"
#include "ProductionMethodMapping.h"
#include <numeric>
#include <ranges>

void mappers::ProductionMethodMapper::loadRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Loading PM rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> Loaded PM rules for " << buildingToRules.size() << " buildings.";
}


void mappers::ProductionMethodMapper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& buildingName, std::istream& theStream) {
		ProductionMethodMapping PMMapping;
		PMMapping.loadRules(theStream);

		// Gov Admins are special, leave them out
		if (buildingName != "building_government_administration")
			buildingToRules.emplace(buildingName, PMMapping.getRules());
	});
}

void mappers::ProductionMethodMapper::applyRules(const V3::Country& country,
	 const std::map<std::string, V3::ProductionMethod>& PMs,
	 const std::map<std::string, V3::ProductionMethodGroup>& PMGroups) const
{
	for (const auto& buildingType: getCountryBuildingTypes(country))
	{
		if (!buildingToRules.contains(buildingType))
			continue;

		auto builtBuildings = getBuiltBuildings(country, buildingType); // All instances of the current building type in the country
		for (const auto& rule: buildingToRules.at(buildingType))
		{
			auto selectedBuildings = pickBuildings(builtBuildings, rule.percent);
			const std::string selectedPM = pickPM(country, rule.pm, PMs, PMGroups); // select PM

			// For each building in the vector, add the PMs to it
			for (const auto& selectedBuilding: selectedBuildings)
				selectedBuilding->addPM(selectedPM);
		}
	}
}

std::set<std::string> mappers::ProductionMethodMapper::getCountryBuildingTypes(const V3::Country& country)
{
	std::set<std::string> countryBuildingTypes;
	for (const auto& subState: country.getSubStates())
	{
		for (const auto& building: subState->getBuildings())
			countryBuildingTypes.emplace(building->getName());
	}

	return countryBuildingTypes;
}

std::vector<std::shared_ptr<V3::Building>> mappers::ProductionMethodMapper::getBuiltBuildings(const V3::Country& country, const std::string& buildingType)
{
	std::vector<std::shared_ptr<V3::Building>> builtBuildings;

	const auto& findBuilding = [buildingType](const std::shared_ptr<V3::Building>& subStateBuilding) {
		return subStateBuilding->getName() == buildingType;
	};

	for (const auto& subState: country.getSubStates())
	{
		if (const auto& builtBuilding = std::ranges::find_if(subState->getBuildings(), findBuilding); builtBuilding != subState->getBuildings().end())
			builtBuildings.push_back(*builtBuilding);
	}

	return builtBuildings;
}

std::vector<std::shared_ptr<V3::Building>> mappers::ProductionMethodMapper::pickBuildings(const std::vector<std::shared_ptr<V3::Building>>& buildings,
	 const double percent)
{
	if (percent >= 1)
	{
		return buildings;
	}

	auto sumBuildingLevels = [](const int sum, const std::shared_ptr<V3::Building>& building) {
		return sum + building->getLevel();
	};

	const int target = static_cast<int>(std::accumulate(buildings.begin(), buildings.end(), 0, sumBuildingLevels) * percent);

	// Select all buildings with level <= target
	std::vector<std::shared_ptr<V3::Building>> selectedBuildings;
	std::ranges::copy_if(buildings, std::back_inserter(selectedBuildings), [target](const std::shared_ptr<V3::Building>& building) {
		return building->getLevel() <= target;
	});

	if (std::accumulate(selectedBuildings.begin(), selectedBuildings.end(), 0, sumBuildingLevels) > target)
	{
		selectedBuildings = subSetSum(selectedBuildings, target);
	}

	// Now return either selectedBuildings, or a single building if the sum of selectedBuildings
	// is further from target than the smallest building larger than target.
	std::shared_ptr<V3::Building> smallestLargerBuilding = nullptr;
	int smallestLargerBuildingLevel = INT_MAX;

	for (const auto& building: buildings)
	{
		if (building->getLevel() > target && building->getLevel() < smallestLargerBuildingLevel)
		{
			smallestLargerBuilding = building;
			smallestLargerBuildingLevel = building->getLevel();
		}
	}

	if (smallestLargerBuilding)
	{
		if (smallestLargerBuildingLevel - target < target - std::accumulate(selectedBuildings.begin(), selectedBuildings.end(), 0, sumBuildingLevels))
		{
			selectedBuildings.clear();
			selectedBuildings.push_back(smallestLargerBuilding);
		}
	}

	return selectedBuildings;
}

std::string mappers::ProductionMethodMapper::pickPM(const V3::Country& country,
	 const std::string& targetName,
	 const std::map<std::string, V3::ProductionMethod>& PMs,
	 const std::map<std::string, V3::ProductionMethodGroup>& PMGroups)
{
	// NOTE(Gawquon): This works for most PMs, but is not guaranteed to work for ownership PMs.
	// This is just a basic version that will support every use-case we currently care about.
	for (const auto& PMGroup: PMGroups | std::views::values)
	{
		if (std::ranges::find(PMGroup.getPMs(), targetName) != PMGroup.getPMs().end())
		{
			// Validate every PM in group
			for (const auto& PM: PMGroup.getPMs())
			{
				if (!PMs.contains(PM))
				{
					Log(LogLevel::Error) << "Unknown PM: " << PM << ".";
					return "";
				}
			}

			// Walk the group
			std::string pick = PMGroup.getPMs()[0];
			for (auto PM: PMGroup.getPMs())
			{
				if (!country.hasAnyOfTech(PMs.at(PM).getUnlockingTechs()))
					return pick;
				if (PM == targetName)
					return PM;
				pick = PM;
			}
		}
	}

	Log(LogLevel::Error) << "Could not find a PM group for PM: " << targetName << ".";

	return "";
}

////////////////////////////////////////// Subset-sum fxns
std::vector<std::shared_ptr<V3::Building>> mappers::ProductionMethodMapper::subSetSum(const std::vector<std::shared_ptr<V3::Building>>& subSet, int targetVal)
{
	return subSet;
}
