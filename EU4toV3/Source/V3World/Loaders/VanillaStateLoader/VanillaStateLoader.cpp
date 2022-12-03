#include "VanillaStateLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "VanillaStateEntries.h"

void V3::VanillaStateLoader::loadVanillaStates(const std::string& v3Path)
{
	registerKeys();
	for (const auto& fileName: commonItems::GetAllFilesInFolder(v3Path + "/common/history/states/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(v3Path + "/common/history/states/" + fileName);
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
