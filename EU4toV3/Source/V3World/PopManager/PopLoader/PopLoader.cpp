#include "PopLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "StateEntries.h"

void V3::PopLoader::loadPops(const std::string& v3Path)
{
	registerKeys();
	for (const auto& fileName: commonItems::GetAllFilesInFolder(v3Path + "/common/history/pops/"))
	{
		if (getExtension(fileName) != "txt")
			continue;
		parseFile(v3Path + "/common/history/pops/" + fileName);
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
