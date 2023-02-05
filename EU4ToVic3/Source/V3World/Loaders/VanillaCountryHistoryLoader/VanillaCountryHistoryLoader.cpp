#include "VanillaCountryHistoryLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"
#include "VanillaCountryHistoryEntries.h"

void V3::VanillaCountryHistoryLoader::loadVanillaCountryHistories(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/history/countries/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(fileName);
	}
	clearRegisteredKeywords();
}

void V3::VanillaCountryHistoryLoader::registerKeys()
{
	registerKeyword("COUNTRIES", [this](std::istream& theStream) {
		const auto entries = VanillaCountryHistoryEntries(theStream);
		// Append. They don't necessarily overwrite eachother.
		for (const auto& [tag, elements]: entries.getCountryHistoryElements())
			countryHistoryElements[tag].insert(countryHistoryElements[tag].end(), elements.begin(), elements.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
