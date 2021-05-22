#include "Religions.h"
#include "Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "ReligionGroup.h"
#include "CommonRegexes.h"

EU4::Religions::Religions()
{
	LOG(LogLevel::Info) << "Parsing EU4 religions";
	registerKeys();

	for (const auto& filename: commonItems::GetAllFilesInFolder(theConfiguration.getEU4Path() + "/common/religions/"))
		parseFile(theConfiguration.getEU4Path() + "/common/religions/" + filename);

	for (const auto& modName: theConfiguration.getEU4Mods())
		for (const auto& filename: commonItems::GetAllFilesInFolder(modName + "/common/religions/"))
			parseFile(modName + "/common/religions/" + filename);

	clearRegisteredKeywords();
}

EU4::Religions::Religions(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::Religions::registerKeys()
{
	registerRegex(commonItems::stringRegex, [this](const std::string& unused, std::istream& theStream) {
		ReligionGroup newGroup(theStream);
		const auto fetchedReligions = newGroup.takeReligions();
		theReligions.insert(fetchedReligions.begin(), fetchedReligions.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
