#include "ProductionMethodLoader.h"
#include "CommonFunctions.h"
#include "ModLoader/ModFilesystem.h"
#include <CommonRegexes.h>

void V3::ProductionMethodLoader::loadPMs(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/production_methods/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(fileName);
	}
	clearRegisteredKeywords();
}

void V3::ProductionMethodLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& PMName, std::istream& theStream) {
		ProductionMethod newPM;
		newPM.loadProductionMethod(theStream);
		newPM.setName(PMName);
		PMs.emplace(PMName, newPM);
	});
}