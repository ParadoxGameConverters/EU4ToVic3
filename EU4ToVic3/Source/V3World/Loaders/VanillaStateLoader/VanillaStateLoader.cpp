#include "VanillaStateLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"
#include "VanillaStateEntries.h"

void V3::VanillaStateLoader::loadVanillaStates(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("common/history/states"))
	{
		if (fileName.extension() == ".txt")
			parseFile(fileName);
	}
	clearRegisteredKeywords();
}

void V3::VanillaStateLoader::registerKeys()
{
	registerKeyword("STATES", [this](std::istream& theStream) {
		auto entries = VanillaStateEntries(theStream).getStates();
		for (const auto& [stateName, stateEntry]: entries)
			if (!stateEntries.contains(stateName)) // mods have already overridden these later entries.
				stateEntries.emplace(stateName, stateEntry);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
