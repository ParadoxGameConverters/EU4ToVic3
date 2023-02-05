#include "PopLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "StateEntries.h"

void V3::PopLoader::loadPops(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/history/pops/"))
	{
		if (getExtension(fileName) != "txt")
			continue;
		parseFile(fileName);
	}
	clearRegisteredKeywords();
}

void V3::PopLoader::registerKeys()
{
	registerKeyword("POPS", [this](std::istream& theStream) {
		StateEntries stateEntries;
		stateEntries.loadStates(theStream);
		auto grabbedPops = stateEntries.getStatePops();
		grabbedPops.merge(statePops); // merging this way so we override previous entries.
		statePops.swap(grabbedPops);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
