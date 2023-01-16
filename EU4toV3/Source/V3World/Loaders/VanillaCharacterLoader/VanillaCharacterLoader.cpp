#include "VanillaCharacterLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"
#include "VanillaCharacterEntries.h"

void V3::VanillaCharacterLoader::loadVanillaCharacters(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/history/characters/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(fileName);
	}
	clearRegisteredKeywords();
}

void V3::VanillaCharacterLoader::registerKeys()
{
	registerKeyword("CHARACTERS", [this](std::istream& theStream) {
		const auto entries = VanillaCharacterEntries(theStream);
		// Append. They don't necessarily overwrite eachother.
		for (const auto& [tag, elements]: entries.getCharacterElements())
			characterElements[tag].insert(characterElements[tag].end(), elements.begin(), elements.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
