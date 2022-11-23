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

	for (const auto& mod: mods)
		for (const auto& fileName: commonItems::GetAllFilesInFolder(mod.path + "/common/religions/"))
		{
			// Watch out for our own special religions.
			if (fileName.starts_with("99_converted_"))
			{
				// This is one of ours.
				const auto pos = fileName.find("-from-"); // 99_converted_dynamic_faith_107-from-shamanism.txt
				if (pos == std::string::npos || pos <= 13)
				{
					// Someone changed it. Skip metadata.
					Log(LogLevel::Warning) << "! Religion Mapper: Filename for custom religion is broken: " << fileName;
				}
				else
				{
					auto source = fileName.substr(pos + 6, fileName.size() - 10 - pos); // shaving off ".txt" extension, grabbing "shamanism"
					if (source.empty()) // they may be corrupted. "99_converted_dynamic_faith_101-from-.txt" is known CK3 error.
						Log(LogLevel::Warning) << "! Religion Mapper: Cannot parse filename for metadata: " << fileName;
					else
						activeTrappings = source;
				}
			}
			parseFile(mod.path + "/common/religions/" + fileName);
			activeTrappings.reset();
		}

	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> Loaded " << religions.size() << " religions.";
}

void EU4::ReligionLoader::loadReligions(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::ReligionLoader::registerKeys()
{
	registerRegex(commonItems::stringRegex, [this](const std::string& religionGroup, std::istream& theStream) {
		ReligionParser newGroup(theStream);
		const auto fetchedReligions = newGroup.takeReligions();
		// in case we're parsing our own ck3 religion source file, there should be only one religion in the file, and the trappings will be active.
		if (activeTrappings && fetchedReligions.size() == 1)
		{
			Religion religion;
			religion.name = *fetchedReligions.begin();
			religion.group = religionGroup;
			religion.trappings = *activeTrappings;
			religions.emplace(religion.name, religion);
		}
		else
		{
			// all the normal cases
			for (const auto& religionName: fetchedReligions)
			{
				Religion religion;
				religion.name = religionName;
				religion.group = religionGroup;
				// We'll set trappings on the other side.
				if (religions.contains(religionName))
					religions.at(religionName) = religion;
				else
					religions.emplace(religion.name, religion);
			}
		}
	});
}
