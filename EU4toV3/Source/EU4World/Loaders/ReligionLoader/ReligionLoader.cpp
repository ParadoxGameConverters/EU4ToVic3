#include "ReligionLoader.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "ReligionParser.h"
#include <ranges>

void EU4::ReligionLoader::loadReligions(const std::string& EU4Path, const Mods& mods)
{
	registerKeys();

	for (const auto& filename: commonItems::GetAllFilesInFolder(EU4Path + "/common/religions/"))
		parseFile(EU4Path + "/common/religions/" + filename);

	for (const auto& modPath: mods | std::views::values)
		for (const auto& filename: commonItems::GetAllFilesInFolder(modPath + "/common/religions/"))
			parseFile(modPath + "/common/religions/" + filename);

	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> Loaded " << theReligions.size() << " religions.";
}

void EU4::ReligionLoader::loadReligions(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::ReligionLoader::registerKeys()
{
	registerRegex(commonItems::stringRegex, [this](const std::string& unused, std::istream& theStream) {
		ReligionParser newGroup(theStream);
		const auto fetchedReligions = newGroup.takeReligions();
		theReligions.insert(fetchedReligions.begin(), fetchedReligions.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
