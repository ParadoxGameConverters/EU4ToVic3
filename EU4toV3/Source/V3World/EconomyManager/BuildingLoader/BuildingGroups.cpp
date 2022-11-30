#include "BuildingGroups.h"
#include "Log.h"
#include <ranges>

void V3::BuildingGroups::registerKeywords()
{
}

void V3::BuildingGroups::setInfrastructureCosts()
{
	for (auto& buildingGroup: std::views::values(buildingGroups))
	{
		std::string& parentGroupName = buildingGroup->parent;
		while (buildingGroup->infrastructureCost == 0 && !parentGroupName.empty())
		{
			buildingGroup->infrastructureCost = getInfrastuctureCost(parentGroupName);
			parentGroupName = getParent(parentGroupName);
		}
	}
}

void V3::BuildingGroups::loadBuildingGroups(std::istream& theStream)
{
}

const std::string& V3::BuildingGroups::getParent(const std::string& theBuildingGroup) const
{
	return getBuildingGroup(theBuildingGroup)->parent;
}

const int V3::BuildingGroups::getInfrastuctureCost(const std::string& theBuildingGroup) const
{
	return getBuildingGroup(theBuildingGroup)->infrastructureCost;
}

const std::shared_ptr<V3::BuildingGroup> V3::BuildingGroups::getBuildingGroup(const std::string& theBuildingGroup) const
{
	const auto& possibleBuildingGroup = buildingGroups.find(theBuildingGroup);
	if (possibleBuildingGroup != buildingGroups.end())
	{
		return possibleBuildingGroup->second;
	}
	else
	{
		Log(LogLevel::Error) << "Key not recognized: " << theBuildingGroup << " is not a recognized building_group. Using an empty group in its place.";
		return std::move(std::make_unique<BuildingGroup>());
	}
}