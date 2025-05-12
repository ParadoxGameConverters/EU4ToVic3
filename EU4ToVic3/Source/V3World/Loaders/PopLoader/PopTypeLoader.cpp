#include "PopTypeLoader.h"

void V3::PopTypeLoader::loadPopTypes(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/pop_types/"))
	{
		if (getExtension(fileName) != "txt")
			continue;
		parseFile(fileName);
	}
	clearRegisteredKeywords();

	// A copy of the default pop type.
	if (popTypes.contains("laborers"))
	{
		PopType unemployed = popTypes.at("laborers");
		unemployed.setType("unemployed");
		popTypes.emplace("unemployed", unemployed);
	}
	else
	{
		PopType unemployed;
		unemployed.setType("unemployed");
		unemployed.setStartingQOL(5);
		popTypes.emplace("unemployed", unemployed);
	}
}

void V3::PopTypeLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& type, std::istream& theStream) {
		PopType popType(theStream);
		popType.setType(type);
		popTypes.emplace(type, popType);
	});
	IgnoreUnregisteredItems();
}
