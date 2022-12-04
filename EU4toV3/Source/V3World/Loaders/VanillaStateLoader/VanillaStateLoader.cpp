#include "VanillaStateLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"
#include "VanillaStateEntries.h"

void V3::VanillaStateLoader::loadVanillaStates(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/history/states/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(fileName);
	}
	clearRegisteredKeywords();
}

void V3::VanillaStateLoader::registerKeys()
{
	registerKeyword("STATES", [this](std::istream& theStream) {
		auto entries = VanillaStateEntries(theStream).getStates();
		entries.merge(stateEntries); // merging backwards to override older entries.
		stateEntries.swap(entries);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
