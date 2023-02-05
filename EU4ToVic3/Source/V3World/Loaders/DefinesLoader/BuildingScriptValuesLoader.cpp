#include "BuildingScriptValuesLoader.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

void V3::BuildingScriptValuesLoader::loadBuildingScriptValues(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/script_values"))
	{
		if (fileName.find("building_values") == std::string::npos)
			continue;
		parseFile(fileName);
	}
	clearRegisteredKeywords();
}

void V3::BuildingScriptValuesLoader::registerKeys()
{
	registerRegex(R"(construction_cost_\w+)", [this](const std::string& costTier, std::istream& theStream) {
		buildingCostConstants.emplace(costTier, commonItems::getInt(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}