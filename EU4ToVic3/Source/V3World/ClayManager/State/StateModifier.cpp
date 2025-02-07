#include "StateModifier.h"
#include "CommonRegexes.h"
#include "EconomyManager/Building/BuildingGroups.h"
#include "ParserHelpers.h"
#include <numeric>
#include <ranges>

#include "Log.h"

void V3::StateModifier::loadStateModifier(std::istream& theStream)
{
	registerKeys();
	modifierUnwrapper.parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::StateModifier::registerKeys()
{
	modifierUnwrapper.registerKeyword("modifier", [this](std::istream& theStream) {
		parseStream(theStream);
	});
	modifierUnwrapper.IgnoreUnregisteredItems();

	registerRegex("state_building_[a-zA-Z_]+_max_level_add", [this](const std::string& modifier, std::istream& theStream) {
		const std::regex pattern("state_building_([a-zA-Z_]+)_max_level_add");
		std::smatch building;
		if (std::regex_search(modifier, building, pattern)) // state_building_port_max_level_add -> port
		{
			maxBuildingLevels["building_" + building[1].str()] = commonItems::getInt(theStream);
		}
		else
		{
			Log(LogLevel::Error) << "Found a max level modifier, but could not parse it: " << modifier;
		}
	});
	registerKeyword("state_infrastructure_add", [this](std::istream& theStream) {
		infrastructure = commonItems::getInt(theStream);
	});
	registerKeyword("state_infrastructure_mult", [this](std::istream& theStream) {
		infrastructureMult = commonItems::getDouble(theStream);
	});
	registerRegex(R"(building_output_\w+_mult)", [this](const std::string& modifierName, std::istream& theStream) {
		// Goods based throughput modifiers
		const auto& id = modifierName.substr(0, modifierName.size() - 5); // name before _mult
		goodsModifiers.emplace(id, commonItems::getDouble(theStream));
	});
	registerRegex(R"(building_group_\w+_mult)", [this](const std::string& modifierName, std::istream& theStream) {
		// Building Group based throughput modifiers
		const auto& id = modifierName.substr(15, modifierName.size() - 31); // name between building_group_ and _throughput_mult
		buildingGroupModifiers.emplace(id, commonItems::getDouble(theStream));
	});
	registerRegex(R"(building_\w+_mult)", [this](const std::string& modifierName, std::istream& theStream) {
		// Building based throughput modifiers
		const auto& id = modifierName.substr(0, modifierName.size() - 16); // name before _throughput_mult
		buildingModifiers.emplace(id, commonItems::getDouble(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<int> V3::StateModifier::getMaxBuildingBonus(const std::string& building) const
{
	if (maxBuildingLevels.contains(building))
	{
		return maxBuildingLevels.at(building);
	}
	return std::nullopt;
}

double V3::StateModifier::getAllBonuses(const std::map<std::string, double>& modifierMap)
{
	auto modifiers = std::views::values(modifierMap);

	return std::accumulate(modifiers.begin(), modifiers.end(), 0.0);
}

double V3::StateModifier::getBuildingGroupModifier(const std::string& buildingGroup, const BuildingGroups& bgs) const
{
	double modifierTotal = 0;
	for (const auto& [groupName, modifier]: buildingGroupModifiers)
	{
		std::optional currentGroup = buildingGroup;

		do
		{
			if (groupName == currentGroup.value())
			{
				modifierTotal += modifier;
				break;
			}
			currentGroup = bgs.tryGetParentName(currentGroup);
		} while (currentGroup);
	}
	return modifierTotal;
}

std::optional<double> V3::StateModifier::getBuildingModifier(const std::string& building) const
{
	if (const auto& possibleModifier = buildingModifiers.find(building); possibleModifier != buildingModifiers.end())
	{
		return possibleModifier->second;
	}
	return std::nullopt;
}

std::optional<double> V3::StateModifier::getGoodsModifier(const std::string& good) const
{
	if (const auto& possibleModifier = goodsModifiers.find(good); possibleModifier != goodsModifiers.end())
	{
		return possibleModifier->second;
	}
	return std::nullopt;
}

double V3::StateModifier::calcBuildingModifiers(const Building& building, const BuildingGroups& buildingGroups) const
{
	const auto& modifierIter = buildingModifiers.find(building.getName());
	double modifierTotal = modifierIter == buildingModifiers.end() ? 0 : modifierIter->second;
	modifierTotal += getBuildingGroupModifier(building.getBuildingGroup(), buildingGroups);

	return modifierTotal;
}
