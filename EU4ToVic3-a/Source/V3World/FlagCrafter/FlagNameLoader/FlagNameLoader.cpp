#include "FlagNameLoader.h"
#include "CommonRegexes.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"

void V3::FlagNameLoader::loadKnownFlags(const std::string& folderPath)
{
	registerKeys();
	for (const auto& filename: commonItems::GetAllFilesInFolder(folderPath))
		parseFile(folderPath + "/" + filename);
	clearRegisteredKeywords();
}

void V3::FlagNameLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& flagName, std::istream& theStream) {
		knownFlags.emplace(flagName);
		commonItems::ignoreItem(flagName, theStream);
	});
}
