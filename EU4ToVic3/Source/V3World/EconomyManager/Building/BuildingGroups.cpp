#include "BuildingGroups.h"
#include "BuildingGroup.h"
#include <ranges>

void V3::BuildingGroups::setInfrastructureCosts()
{
	for (const auto& buildingGroup: std::views::values(buildingGroups))
	{
		auto parentGroupName = buildingGroup->getParentName();
		while (!buildingGroup->getInfrastructureCost() && parentGroupName)
		{
			buildingGroup->setInfrastructureCost(tryGetInfraCost(parentGroupName));
			parentGroupName = tryGetParentName(parentGroupName);
		}
	}
}

void V3::BuildingGroups::setResourceCaps()
{
	for (const auto& buildingGroup: std::views::values(buildingGroups))
	{
		auto parentGroupName = buildingGroup->getParentName();
		while (!buildingGroup->possibleIsResourceCapped() && parentGroupName)
		{
			buildingGroup->setResourceCap(tryGetIsCapped(parentGroupName));
			parentGroupName = tryGetParentName(parentGroupName);
		}
	}
}

void V3::BuildingGroups::addBuildingGroup(const std::shared_ptr<BuildingGroup>& theBuildingGroup)
{
	buildingGroups[theBuildingGroup->getName()] = theBuildingGroup;
}

std::optional<std::string> V3::BuildingGroups::getAncestralCategory(const std::optional<std::string>& theGroupName) const
{
	if (!theGroupName)
	{
		return std::nullopt;
	}
	auto name = theGroupName.value();
	while (!name.empty())
	{
		const auto& possibleGroup = buildingGroups.find(name);
		if (possibleGroup == buildingGroups.end())
		{
			break;
		}
		const auto cat = possibleGroup->second->getCategory();
		if (cat)
		{
			return *cat;
		}
		const auto& parentName = possibleGroup->second->getParentName();
		if (!parentName)
		{
			break;
		}
		name = parentName.value();
	}

	return std::nullopt;
}

std::optional<std::string> V3::BuildingGroups::getAncestralGroup(const std::string& theGroupName) const
{
	auto name = theGroupName;
	while (!name.empty())
	{
		const auto& possibleGroup = buildingGroups.find(name);
		if (possibleGroup == buildingGroups.end())
		{
			return std::nullopt;
		}
		const auto& parentName = possibleGroup->second->getParentName();
		if (!parentName)
		{
			return possibleGroup->first;
		}
		name = parentName.value();
	}

	return std::nullopt;
}

bool V3::BuildingGroups::usesArableLand(const std::optional<std::string>& theGroupName) const
{
	if (!theGroupName)
	{
		return false;
	}
	auto name = theGroupName.value();
	while (!name.empty())
	{
		const auto& possibleGroup = buildingGroups.find(name);
		if (possibleGroup == buildingGroups.end())
		{
			break;
		}
		if (possibleGroup->second->usesArableLand())
		{
			return true;
		}
		const auto& parentName = possibleGroup->second->getParentName();
		if (!parentName)
		{
			break;
		}
		name = parentName.value();
	}

	return false;
}

std::optional<std::string> V3::BuildingGroups::tryGetParentName(const std::optional<std::string>& theGroupName) const
{
	if (!theGroupName)
		return std::nullopt;

	const auto& possibleGroup = buildingGroups.find(theGroupName.value());
	if (possibleGroup != buildingGroups.end())
	{
		return possibleGroup->second->getParentName();
	}

	return std::nullopt;
}

std::optional<double> V3::BuildingGroups::tryGetInfraCost(const std::optional<std::string>& theGroupName) const
{
	if (!theGroupName)
		return std::nullopt;

	const auto& possibleGroup = buildingGroups.find(theGroupName.value());
	if (possibleGroup != buildingGroups.end())
	{
		return possibleGroup->second->getInfrastructureCost();
	}

	return std::nullopt;
}

std::optional<bool> V3::BuildingGroups::tryGetIsCapped(const std::optional<std::string>& theGroupName) const
{
	if (!theGroupName)
		return std::nullopt;

	auto possibleGroup = buildingGroups.find(theGroupName.value());
	if (possibleGroup != buildingGroups.end())
	{
		if (possibleGroup->second->possibleIsResourceCapped() && !*possibleGroup->second->possibleIsResourceCapped())
			return false;
		if (possibleGroup->second->possibleIsResourceCapped() && *possibleGroup->second->possibleIsResourceCapped())
			return true;
		while (possibleGroup->second->getParentName())
			possibleGroup = buildingGroups.find(*possibleGroup->second->getParentName());
		{
			if (possibleGroup->second->possibleIsResourceCapped() && !*possibleGroup->second->possibleIsResourceCapped())
				return false;
			if (possibleGroup->second->possibleIsResourceCapped() && *possibleGroup->second->possibleIsResourceCapped())
				return true;
		}
		return possibleGroup->second->possibleIsResourceCapped();
	}

	return std::nullopt;
}

int V3::BuildingGroups::getUrbanization(const std::string& theGroupName) const
{
	auto name = theGroupName;
	while (!name.empty())
	{
		const auto& possibleGroup = buildingGroups.find(name);
		if (possibleGroup == buildingGroups.end())
		{
			break;
		}
		if (const auto& urbanization = possibleGroup->second->getUrbanization(); urbanization > 0)
		{
			return urbanization;
		}
		const auto& parentName = possibleGroup->second->getParentName();
		if (!parentName)
		{
			break;
		}
		name = parentName.value();
	}

	return 0;
}
