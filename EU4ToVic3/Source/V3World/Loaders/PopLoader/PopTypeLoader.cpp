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
