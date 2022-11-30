#include "BuildingGroupLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"

void V3::BuildingGroupLoader::loadBuildingGroups(const std::string& v3Path)
{
	registerKeys();
	for (const auto& fileName: commonItems::GetAllFilesInFolder(v3Path + "/common/building_groups/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(v3Path + "/common/building_groups/" + fileName);
	}
	clearRegisteredKeywords();
}

void V3::BuildingGroupLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& buildingGroupName, std::istream& theStream) {
		buildingGroups = std::make_shared<BuildingGroups>();
		theBuildingGroups->loadBuildingGroups(theStream);
	});
}
