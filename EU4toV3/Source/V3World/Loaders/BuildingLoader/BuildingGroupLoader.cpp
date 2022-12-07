#include "BuildingGroupLoader.h"
#include "BuildingGroup.h"
#include "BuildingGroups.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"

V3::BuildingGroupLoader::BuildingGroupLoader(): buildingGroups(std::make_shared<BuildingGroups>())
{
}

void V3::BuildingGroupLoader::loadBuildingGroups(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/building_groups/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(fileName);
	}
	clearRegisteredKeywords();
	buildingGroups->setInfrastructureCosts();
}

void V3::BuildingGroupLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& buildingGroupName, std::istream& theStream) {
		auto newBuildingGroup = std::make_unique<BuildingGroup>();
		newBuildingGroup->loadBuildingGroup(theStream);
		newBuildingGroup->setName(buildingGroupName);
		buildingGroups->addBuildingGroup(std::move(newBuildingGroup));
	});
}