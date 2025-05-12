#include "ReligionLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ModLoader/ModFilesystem.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "ReligionParser.h"
#include <ranges>

void EU4::ReligionLoader::loadReligions(const commonItems::ModFilesystem& modFS)
{
	registerKeys();

	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/religions/"))
	{
		if (getExtension(fileName) != "txt")
			continue;
		auto theFileName = trimPath(fileName);
		// Watch out for our own special religions.
		if (theFileName.starts_with("99_converted_"))
		{
			// This is one of ours.
			const auto pos = theFileName.find("-from-"); // 99_converted_dynamic_faith_107-from-shamanism.txt
			if (pos == std::string::npos || pos <= 13)
			{
				// Someone changed it. Skip metadata.
				Log(LogLevel::Warning) << "! Religion Mapper: Filename for custom religion is broken: " << theFileName;
			}
			else
			{
				auto source = theFileName.substr(pos + 6, theFileName.size() - 10 - pos); // shaving off ".txt" extension, grabbing "shamanism"
				if (source.empty()) // they may be corrupted. "99_converted_dynamic_faith_101-from-.txt" is known CK3 error.
					Log(LogLevel::Warning) << "! Religion Mapper: Cannot parse filename for metadata: " << theFileName;
				else
					activeTrappings = source;
			}
		}
		parseFile(fileName);
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
		const ReligionParser newGroup(theStream);
		auto fetchedReligions = newGroup.getReligions();
		// in case we're parsing our own ck3 religion source file, there should be only one religion in the file, and the trappings will be active.
		if (activeTrappings && fetchedReligions.size() == 1)
		{
			auto customReligion = *fetchedReligions.begin();
			customReligion.group = religionGroup;
			customReligion.trappings = *activeTrappings;
			religions.emplace(customReligion.name, customReligion);
		}
		else
		{
			// all the normal cases
			for (auto& religion: fetchedReligions)
			{
				religion.group = religionGroup;
				// We'll set trappings on the other side.
				if (religions.contains(religion.name))
					religions.at(religion.name) = religion;
				else
					religions.emplace(religion.name, religion);
			}
		}
	});
}

std::optional<std::string> EU4::ReligionLoader::getGroupForReligion(const std::string& religion) const
{
	const auto religionItr = religions.find(religion);
	if (religionItr == religions.end() || religionItr->second.group.empty())
		return std::nullopt;

	return religionItr->second.group;
}
