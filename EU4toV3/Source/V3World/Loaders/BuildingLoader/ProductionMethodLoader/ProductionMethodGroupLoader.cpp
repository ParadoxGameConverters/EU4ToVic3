#include "ProductionMethodGroupLoader.h"
#include "CommonFunctions.h"
#include <CommonRegexes.h>

void V3::ProductionMethodGroupLoader::loadPMGroups(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/production_method_groups/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(fileName);
	}
	clearRegisteredKeywords();
}

void V3::ProductionMethodGroupLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& PMGroupName, std::istream& theStream) {
		ProductionMethodGroup newPMGroup;
		newPMGroup.loadProductionMethodGroup(theStream);
		newPMGroup.setName(PMGroupName);
		PMGroups.emplace(PMGroupName, newPMGroup);
	});
}