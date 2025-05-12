#include "ReligionDefinitionLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "ReligionDefinitionEntry.h"

void mappers::ReligionDefinitionLoader::loadDefinitions(const commonItems::ModFilesystem& modFS)
{
	Log(LogLevel::Info) << "-> Loading religion refinitions.";
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/religions/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(fileName);
	}
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << religionDefinitions.size() << " definitions loaded.";
}

void mappers::ReligionDefinitionLoader::loadDefinitions(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ReligionDefinitionLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& religionName, std::istream& theStream) {
		auto relDef = ReligionDefinitionEntry(theStream).getReligionDef();
		relDef.name = religionName;
		religionDefinitions.emplace(religionName, relDef);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<mappers::ReligionDef> mappers::ReligionDefinitionLoader::getReligionDef(const std::string& religion) const
{
	if (religionDefinitions.contains(religion))
		return religionDefinitions.at(religion);
	return std::nullopt;
}
