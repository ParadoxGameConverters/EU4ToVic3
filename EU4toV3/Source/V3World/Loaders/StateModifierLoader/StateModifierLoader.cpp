#include "StateModifierLoader.h"
#include "CommonFunctions.h"
#include "OSCompatibilityLayer.h"

void V3::StateModifierLoader::loadStateModifiers(const std::string& v3Path)
{
	registerKeys();
	for (const auto& fileName: commonItems::GetAllFilesInFolder(v3Path + "/common/state_traits/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(v3Path + "/common/state_traits/" + fileName);
	}
	clearRegisteredKeywords();
}

void V3::StateModifierLoader::registerKeys()
{
}
