#include "ReligionMapper.h"
#include "../../Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "ReligionMapping.h"
#include "CommonRegexes.h"

mappers::ReligionMapper::ReligionMapper()
{
	LOG(LogLevel::Info) << "Parsing religion mappings.";
	registerKeys();
	parseFile("configurables/religion_map.txt");
	clearRegisteredKeywords();
}

mappers::ReligionMapper::ReligionMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ReligionMapper::registerKeys()
{
	registerKeyword("link", [this](const std::string& unused, std::istream& theStream) {
		const ReligionMapping theMapping(theStream);
		for (const auto& eu4Religion: theMapping.getEU4Religions())
		{
			eu4ToVic2ReligionMap.insert(std::make_pair(eu4Religion, theMapping.getVic2Religion()));
		}
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::ReligionMapper::getVic2Religion(const std::string& eu4Religion) const
{
	const auto& mapping = eu4ToVic2ReligionMap.find(eu4Religion);
	if (mapping != eu4ToVic2ReligionMap.end())
		return mapping->second;
	return std::nullopt;
}

void mappers::ReligionMapper::scrapeCustomReligions()
{
	for (const auto& modName: theConfiguration.getEU4Mods())
	{
		if (commonItems::DoesFolderExist(modName + "/common/religions/"))
		{
			for (const auto& fileName: commonItems::GetAllFilesInFolder(modName + "/common/religions/"))
			{
				if (fileName.find("99_converted_") == 0)
				{
					// This is one of ours.
					const auto pos = fileName.find("-from-");
					if (pos == std::string::npos || pos <= 13)
					{
						Log(LogLevel::Error) << "Religion Mapper: Cannot import filename: " << fileName;
						continue;
					}
					auto target = fileName.substr(3, pos - 3); // "converted_dynamic_faith_102"
					auto source = fileName.substr(pos + 6, fileName.size() - 10 - pos); // shaving off .txt extension, "orthodox"
					if (eu4ToVic2ReligionMap.count(source))
					{
						auto destination = eu4ToVic2ReligionMap[source];
						eu4ToVic2ReligionMap.insert(std::pair(target, destination));
					}
					else
					{
						Log(LogLevel::Error) << "Cannot merge " << target << " with " << source << ", " << source << " has no Vic2 mapping! Check religion_map.txt!";
					}
				}
			}
		}
	}
}
