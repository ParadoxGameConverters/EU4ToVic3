#include "VanillaDiplomaticPlayLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"
#include "VanillaDiplomaticPlayEntries.h"

void V3::VanillaDiplomaticPlayLoader::loadVanillaDiplomaticPlays(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/history/diplomatic_plays/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(fileName);
	}
	clearRegisteredKeywords();
}

void V3::VanillaDiplomaticPlayLoader::registerKeys()
{
	registerKeyword("DIPLOMATIC_PLAYS", [this](std::istream& theStream) {
		const auto entries = VanillaDiplomaticPlayEntries(theStream);
		// Append. They don't necessarily overwrite eachother.
		for (const auto& [tag, elements]: entries.getDiploPlayElements())
			diploPlayElements[tag].insert(diploPlayElements[tag].end(), elements.begin(), elements.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
