#include "StateModifier.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void V3::StateModifier::loadStateModifier(std::istream& theStream)
{
	registerKeys();
	modiferUnwrapper.parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::StateModifier::registerKeys()
{
	modiferUnwrapper.registerKeyword("modifier", [this](std::istream& theStream) {
		parseStream(theStream);
	});
	modiferUnwrapper.IgnoreUnregisteredItems();

	registerKeyword("state_building_port_max_level_add", [this](std::istream& theStream) {
		port = commonItems::getInt(theStream);
	});
	registerKeyword("state_building_naval_base_max_level_add", [this](std::istream& theStream) {
		navalBase = commonItems::getInt(theStream);
	});
	registerKeyword("state_infrastructure_add", [this](std::istream& theStream) {
		infrastructure = commonItems::getInt(theStream);
	});
	registerKeyword("state_infrastructure_mult", [this](std::istream& theStream) {
		infrastructureModifier = commonItems::getDouble(theStream);
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

const std::optional<double> V3::StateModifier::getBuildingGroupModifier(const std::string& buildingGroup, std::shared_ptr<BuildingGroups> bgs) const
{
	std::string theBuildingGroup = buildingGroup;
	do
	{
		if (const auto& possibleModifier = buildingGroupModifiers.find(theBuildingGroup); possibleModifier != buildingGroupModifiers.end())
		{
			return possibleModifier->second;
		}
		theBuildingGroup = bgs->safeGetParentName(theBuildingGroup);
	} while (bgs->getParentName(theBuildingGroup));
	return std::nullopt;
}

const std::optional<double> V3::StateModifier::getBuildingModifier(const std::string& building) const
{
	if (const auto& possibleModifier = buildingModifiers.find(building); possibleModifier != buildingModifiers.end())
	{
		return possibleModifier->second;
	}
	return std::nullopt;
}

const std::optional<double> V3::StateModifier::getGoodsModifier(const std::string& good) const
{
	if (const auto& possibleModifier = goodsModifiers.find(good); possibleModifier != goodsModifiers.end())
	{
		return possibleModifier->second;
	}
	return std::nullopt;
}