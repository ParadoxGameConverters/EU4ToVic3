#include "VanillaPopulationHistoryLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "Loaders/VanillaCountryHistoryLoader/VanillaCountryHistoryEntries.h"
#include "Log.h"
#include "ParserHelpers.h"

void V3::VanillaPopulationHistoryLoader::loadVanillaPopulationHistories(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/history/population/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(fileName);
	}
	clearRegisteredKeywords();
}

void V3::VanillaPopulationHistoryLoader::registerKeys()
{
	registerKeyword("POPULATION", [this](std::istream& theStream) {
		const auto entries = VanillaCountryHistoryEntries(theStream);
		// Append. They don't necessarily overwrite eachother.
		for (const auto& [tag, elements]: entries.getCountryHistoryElements())
			populationHistoryElements[tag].insert(populationHistoryElements[tag].end(), elements.begin(), elements.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
