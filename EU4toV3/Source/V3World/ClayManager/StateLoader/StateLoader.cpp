#include "StateLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "StringUtils.h"
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
	registerTerrainKeys();
	parseFile(v3Path + "/map_data/province_terrains.txt");
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

void V3::StateLoader::registerTerrainKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& provinceName, std::istream& theStream) {
		const auto theTerrain = commonItems::getString(theStream);
		auto theProvinceName = commonItems::remQuotes(provinceName);
		std::transform(theProvinceName.begin(), theProvinceName.end(), theProvinceName.begin(), ::toupper);
		if (theProvinceName.starts_with("X") && theProvinceName.size() == 7)
			theProvinceName = "x" + theProvinceName.substr(1, theProvinceName.length() - 1);
		else
			Log(LogLevel::Warning) << "Encountered terrain for province " << theProvinceName << " in unknown format!";
		for (const auto& state: states | std::views::values)
			if (state->containsProvince(theProvinceName))
			{
				state->setProvinceTerrain(theProvinceName, theTerrain);
				if (theTerrain == "ocean")
					state->setProvinceSea(theProvinceName);
				else if (theTerrain == "lakes")
					state->setProvinceLake(theProvinceName);
			}
	});
}
