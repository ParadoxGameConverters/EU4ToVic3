#include "StateLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include <ranges>

void V3::StateLoader::loadStates(const std::string& v3Path)
{
	registerKeys();
	for (const auto& fileName: commonItems::GetAllFilesInFolder(v3Path + "/map_data/state_regions/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(v3Path + "/map_data/state_regions/" + fileName);
	}
	clearRegisteredKeywords();
}

void V3::StateLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& stateName, std::istream& theStream) {
		const auto newState = std::make_shared<State>();
		newState->loadState(theStream);
		newState->setStateName(stateName);
		states.emplace(stateName, newState);
	});
}
