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
	/*
	 * An adaptation of David Pisinger's O(nr) Subset-sum algorithm.
	 * Paper: http://hjemmesider.diku.dk/~pisinger/95-6.ps
	 * StackExchange generic implementation: https://stackoverflow.com/questions/9962568/fast-solution-to-subset-sum-algorithm-by-pisinger
	 *
	 *
	 * s_index < breakIndex
	 * t_index >= breakIndex
	 * breakIndex is the index at which the sum of everything to the left first exceeds the target number.
	 * The first index of the 2D array is the index of the subSet, the second index is the currentSum.
	 *
	 * mu = currentSum, the row index
	 * b = the breakIndex, the col index
	 *
	 * For a subSet with a max value of 2 and a target of 15 the table looks something like this
	 * mu | b-1 | b | b+1
	 * 13 | -1  | 0 | 0
	 * 14 | 2   | 0 | 0
	 * 15 | -1  | 0 | 0
	 * 16 | 0   | 0 | 0
	 * 17 | 0   | 0 | 0
	 * 18 | 0   | 0 | 0
	 */

	const auto maxItem = *std::ranges::max_element(subSet, [](const std::shared_ptr<V3::Building>& lhs, const std::shared_ptr<V3::Building>& rhs) {
		return lhs->getLevel() < rhs->getLevel();
	});
	const auto maxVal = maxItem->getLevel();
	const auto [breakIndex, breakSum] = findBreakIndexAndSum(subSet, targetVal);

	// The 2D array of the possible balanced state-space
	// The table on paper is not 0 indexed on its second index. We need to transform all sum based indices to be centered around the breakSum
	// This means we'll be using negative indices to represent possible sums < the breakSum
	std::vector stateSpace(subSet.size() - breakIndex + 1, std::vector<int>(static_cast<size_t>(2 * maxVal)));

	// Pisinger balsub 1-4
	initializeBreakSolution(stateSpace, maxVal, breakIndex, breakSum, targetVal);

	// Pisinger balsub 5-9
	completeBalancedOperations(stateSpace, maxVal, breakIndex, subSet);

	// First check for exact solutions, then close solutions under & over
	std::vector<std::shared_ptr<V3::Building>> result;

	const auto finalState = stateSpace[subSet.size() - breakIndex].begin() + (maxVal - 1);
	for (int offBy = 0; offBy <= maxVal - 1; ++offBy)
	{
		if (finalState[-offBy] >= 0)
		{
			result = traceSteps(stateSpace, -offBy, maxVal, breakIndex, subSet);
			break;
		}
		if (finalState[offBy] >= 0)
		{
			result = traceSteps(stateSpace, -offBy, maxVal, breakIndex, subSet);
			break;
		}
	}
	return result;
}
std::pair<int, int> mappers::ProductionMethodMapper::findBreakIndexAndSum(const std::vector<std::shared_ptr<V3::Building>>& subSet, const int targetNumber)
{
	// Find the first index that exceeds targetNumber when added to the selection
	int sum = 0;
	for (int i = 0; i < static_cast<int>(subSet.size()); ++i)
	{
		if (sum + subSet[i]->getLevel() > targetNumber)
			return {i, sum};
		sum += subSet[i]->getLevel();
	}
	return {-1, -1};
}

void mappers::ProductionMethodMapper::initializeBreakSolution(std::vector<std::vector<int>>& stateSpace,
	 const int maxVal,
	 const int breakIndex,
	 const int breakSum,
	 const int targetVal)
{
	// Transform Pisinger's algorithm which centers on targetNumber, to be centered around 0 to play nicely with vector index math
	const auto initialState = stateSpace[0].begin() + (maxVal - 1);
	for (int currentSum = -maxVal + 1; currentSum <= 0; ++currentSum)
	{
		initialState[currentSum] = -1;
	}
	for (int currentSum = 1; currentSum <= maxVal; ++currentSum)
	{
		initialState[currentSum] = 0;
	}
	initialState[breakSum - targetVal] = breakIndex;
}

void mappers::ProductionMethodMapper::completeBalancedOperations(std::vector<std::vector<int>>& stateSpace,
	 const int maxVal,
	 const int breakIndex,
	 const std::vector<std::shared_ptr<V3::Building>>& subSet)
{
	// Consider the t_index indices in increasing order
	for (int t_index = breakIndex; t_index < static_cast<int>(subSet.size()); ++t_index)
	{
		const auto prevState = stateSpace[t_index - breakIndex].begin() + (maxVal - 1);
		const auto currentState = stateSpace[t_index - breakIndex + 1].begin() + (maxVal - 1);

		for (int currentSum = -maxVal + 1; currentSum <= maxVal; ++currentSum)
		{
			currentState[currentSum] = prevState[currentSum];
		}
		for (int currentSum = -maxVal + 1; currentSum <= 0; ++currentSum)
		{
			const int nextSum = currentSum + subSet[t_index]->getLevel();
			currentState[nextSum] = std::max(currentState[nextSum], prevState[currentSum]);
		}
		for (int currentSum = subSet[t_index]->getLevel(); currentSum >= 1; --currentSum)
		{
			// Now consider the s_indices in decreasing order
			for (int s_index = currentState[currentSum] - 1; s_index >= prevState[currentSum]; --s_index)
			{
				const int nextSum = currentSum - subSet[s_index]->getLevel();
				currentState[nextSum] = std::max(currentState[nextSum], s_index);
			}
		}
	}
}

std::vector<std::shared_ptr<V3::Building>> mappers::ProductionMethodMapper::traceSteps(const std::vector<std::vector<int>>& theStates,
	 const int offBy,
	 const int maxVal,
	 const int breakIndex,
	 const std::vector<std::shared_ptr<V3::Building>>& subSet)
{
	// Now we use the state knowledge to trace the solution from the beginning
	std::vector mask(subSet.size(), false);
	int sum = offBy;

	// Start with the break solution. e.g. {1,1,1,0,0,0,0}
	for (int i = 0; i < breakIndex; ++i)
	{
		mask[i] = true;
	}

	for (int t_index = static_cast<int>(subSet.size()) - 1; t_index >= breakIndex; --t_index)
	{
		auto currentState = theStates[t_index - breakIndex + 1].begin() + (maxVal - 1);
		auto prevState = theStates[t_index - breakIndex].begin() + (maxVal - 1);

		// Then read the state data to flip s_index indices. e.g. {1,1->0,1,0,0,0,0}
		for (int s_index = currentState[sum]; s_index >= 0; s_index = currentState[sum])
		{
			const int currentSum = sum + subSet[s_index]->getLevel();
			if (currentSum > maxVal || s_index >= currentState[currentSum])
			{
				break;
			}
			sum = currentSum;
			mask[s_index] = false;
		}

		// Then read the state data to flip t_index indices. e.g. {1,0,1,0,0->1,0,0->1}
		const int currentSum = sum - subSet[t_index]->getLevel();
		if (currentSum >= -maxVal + 1 && prevState[currentSum] >= currentState[sum])
		{
			sum = currentSum;
			mask[t_index] = true;
		}
	}

	std::vector<std::shared_ptr<V3::Building>> output;
	for (int i = 0; i < static_cast<int>(subSet.size()); i++)
	{
		if (mask[i])
			output.push_back(subSet[i]);
	}
	return output;
}
