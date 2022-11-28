#include "CultureLoader.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include <ranges>

void EU4::CultureLoader::loadCultures(const std::string& EU4Path, const Mods& mods)
{
	registerKeys();

	for (const auto& cultureFile: commonItems::GetAllFilesInFolder(EU4Path + "/common/cultures/"))
		parseFile(EU4Path + "/common/cultures/" + cultureFile);

	for (const auto& mod: mods)
		for (const auto& cultureFile: commonItems::GetAllFilesInFolder(mod.path + "/common/cultures/"))
			parseFile(mod.path + "/common/cultures/" + cultureFile);

	clearRegisteredKeywords();

	Log(LogLevel::Info) << "<> Loaded " << cultureGroupsMap.size() << " culture groups.";
}

void EU4::CultureLoader::loadCultures(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::CultureLoader::registerKeys()
{
	registerRegex(commonItems::stringRegex, [this](std::string cultureGroupName, std::istream& theStream) {
		auto newGroup = CultureGroupParser(cultureGroupName, theStream);

		if (cultureGroupsMap.contains(cultureGroupName))
		{
			// We would normally override base definitions with incoming modded ones, but CK2 definitions
			// for example are crap and don't actually list all required cultures, so we have to merge.
			for (const auto& [cultureName, culture]: newGroup.getCultures())
				cultureGroupsMap[cultureGroupName].mergeCulture(cultureName, culture);
		}
		else
		{
			cultureGroupsMap.emplace(cultureGroupName, newGroup);
		}
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> EU4::CultureLoader::getGroupForCulture(const std::string& culture) const
{
	for (const auto& [groupName, group]: cultureGroupsMap)
		if (group.getCultures().contains(culture))
			return groupName;
	return std::nullopt;
}
