#include "StateModifier.h"

void V3::StateModifier::loadStateModifier(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

const std::optional<double> V3::StateModifier::getThroughputModifier(const std::string& buildingGroup, std::shared_ptr<BuildingGroups> bgs) const
{
	std::string theBuildingGroup = buildingGroup;
	do
	{
		if (const auto& possibleModifier = throughputModifiers.find(theBuildingGroup); possibleModifier != throughputModifiers.end())
		{
			return possibleModifier->second;
		}
		theBuildingGroup = bgs->safeGetParentName(theBuildingGroup);
	} while (!bgs->getParentName(theBuildingGroup));
	return std::nullopt;
}

const std::optional<double> V3::StateModifier::getGoodsOutputModifier(const std::string& good) const
{
	if (const auto& possibleModifier = goodsOutputModifiers.find(good); possibleModifier != goodsOutputModifiers.end())
	{
		return possibleModifier->second;
	}
	return std::nullopt;
}

void V3::StateModifier::registerKeys()
{
}
