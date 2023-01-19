#include "StateLoader.h"
#include "ClayManager/State/State.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"

void V3::StateLoader::loadStates(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/map_data/state_regions/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(fileName);
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
