#include "ProductionMethodGroupLoader.h"
#include "CommonFunctions.h"
#include "EconomyManager/Building/ProductionMethods/ProductionMethodGroup.h"
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
		auto newPMGroup = std::make_unique<V3::ProductionMethodGroup>();
		newPMGroup->loadProductionMethodGroup(theStream);
		newPMGroup->setName(PMGroupName);
		PMGroups.emplace(PMGroupName, std::move(newPMGroup));
	});
}