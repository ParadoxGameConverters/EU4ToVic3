#include "VanillaBuildingLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"
#include "VanillaBuildingEntries.h"

void V3::VanillaBuildingLoader::loadVanillaBuildings(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/history/buildings/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(fileName);
	}
	clearRegisteredKeywords();
}

void V3::VanillaBuildingLoader::registerKeys()
{
	registerKeyword("BUILDINGS", [this](std::istream& theStream) {
		stateParser.parseStream(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	stateParser.registerRegex(R"([s,S]:\w+)", [this](const std::string& stateName, std::istream& theStream) {
		const auto activeState = stateName.substr(2, stateName.size());
		const auto entries = VanillaBuildingEntries(theStream).getBuildingElements();
		buildingElements[activeState] = entries;
	});
	stateParser.registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
