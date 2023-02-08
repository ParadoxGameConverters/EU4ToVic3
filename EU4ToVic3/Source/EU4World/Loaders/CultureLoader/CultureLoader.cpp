#include "CultureLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include <algorithm>
#include <ranges>

void EU4::CultureLoader::loadCultures(const commonItems::ModFilesystem& modFS)
{
	registerKeys();

	for (const auto& file: modFS.GetAllFilesInFolder("/common/cultures/"))
	{
		if (getExtension(file) != "txt")
			continue;
		parseFile(file);
	}

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
	registerRegex(commonItems::stringRegex, [this](const std::string& cultureGroupName, std::istream& theStream) {
		auto newGroup = CultureGroupParser(cultureGroupName, theStream, knownCultures);

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
		const auto names = newGroup.getCultureNames();
		knownCultures.insert(names.begin(), names.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> EU4::CultureLoader::getGroupNameForCulture(const std::string& culture) const
{
	for (const auto& [groupName, group]: cultureGroupsMap)
		if (group.getCultures().contains(culture))
			return groupName;
	return std::nullopt;
}

std::optional<EU4::CultureGroupParser> EU4::CultureLoader::getGroupForCulture(const std::string& culture) const
{
	for (const auto& group: cultureGroupsMap | std::views::values)
		if (group.getCultures().contains(culture))
			return group;
	return std::nullopt;
}

bool EU4::CultureLoader::containsCulture(const std::string& culture) const
{
	return std::ranges::any_of(cultureGroupsMap.begin(), cultureGroupsMap.end(), [culture](const auto& cultureGroup) {
		return cultureGroup.second.getCultures().contains(culture);
	});
}
