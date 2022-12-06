#include "ReligionMapper.h"
#include "CommonRegexes.h"
#include "Loaders/ReligionLoader/Religion.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "ReligionDefinitionLoader/ReligionDefinitionLoader.h"
#include "ReligionGroupMapper/ReligionGroupMapper.h"
#include "ReligionMapping.h"
#include <ranges>

void mappers::ReligionMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Parsing religion mapping rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << eu4ToV3ReligionMap.size() << " rules loaded.";
}

void mappers::ReligionMapper::loadMappingRules(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ReligionMapper::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		const ReligionMapping theMapping(theStream);
		if (!theMapping.getV3Religion().empty() && !theMapping.getEU4Religions().empty())
			for (const auto& eu4Religion: theMapping.getEU4Religions())
				eu4ToV3ReligionMap.emplace(eu4Religion, theMapping.getV3Religion());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::ReligionMapper::getV3Religion(const std::string& eu4Religion) const
{
	if (eu4ToV3ReligionMap.contains(eu4Religion))
		return eu4ToV3ReligionMap.at(eu4Religion);
	return std::nullopt;
}

void mappers::ReligionMapper::expandReligionMappings(const std::map<std::string, EU4::Religion>& eu4Religions)
{
	// We're looking at all eu4 religions and creating new mappings for stuff we don't recognize.
	// This means all dynamic and unmapped eu4 religions will be retained and map to themselves.

	const auto curSize = eu4ToV3ReligionMap.size();
	for (const auto& eu4ReligionName: eu4Religions | std::views::keys)
	{
		if (!eu4ToV3ReligionMap.contains(eu4ReligionName))
			eu4ToV3ReligionMap.emplace(eu4ReligionName, eu4ReligionName);
	}
	Log(LogLevel::Info) << "<> Additional " << eu4ToV3ReligionMap.size() - curSize << " religions imported.";
}

void mappers::ReligionMapper::generateReligionDefinitions(const commonItems::ModFilesystem& modFS, const std::map<std::string, EU4::Religion>& eu4Religions)
{
	Log(LogLevel::Info) << "-> Generating Religion Definitions.";

	ReligionDefinitionLoader religionDefinitionLoader;
	religionDefinitionLoader.loadDefinitions(modFS);
	ReligionGroupMapper religionGroupMapper;
	religionGroupMapper.loadMappingRules("configurables/religion_group_map.txt");

	for (const auto& [eu4ReligionName, v3ReligionName]: eu4ToV3ReligionMap)
	{
		if (vic3ReligionDefinitions.contains(v3ReligionName))
			continue;

		// do we have a ready definition already?
		const auto defMatch = religionDefinitionLoader.getReligionDef(v3ReligionName);
		if (defMatch)
		{
			vic3ReligionDefinitions.emplace(v3ReligionName, *defMatch);
			continue;
		}

		// quick sanity, though this should never fire except when testing.
		if (!eu4Religions.contains(eu4ReligionName))
		{
			Log(LogLevel::Warning) << "EU4 religions don't contain " << eu4ReligionName << "? What are we even doing?";
			continue;
		}

		// generate one and file.
		auto newDef = generateReligionDefinition(v3ReligionName, religionGroupMapper, religionDefinitionLoader, eu4Religions.at(eu4ReligionName));
		vic3ReligionDefinitions.emplace(v3ReligionName, newDef);
	}

	Log(LogLevel::Info) << "-> Generated " << vic3ReligionDefinitions.size() << " Religion Definitions.";
}

mappers::ReligionDef mappers::ReligionMapper::generateReligionDefinition(const std::string& v3ReligionName,
	 const ReligionGroupMapper& religionGroupMapper,
	 const ReligionDefinitionLoader& religionDefinitionLoader,
	 const EU4::Religion& eu4Religion) const
{
	ReligionDef religionDef;
	religionDef.name = v3ReligionName;
	if (eu4Religion.color)
		religionDef.color = eu4Religion.color;

	std::string trappings;
	std::string icon;

	// can we match a group?
	if (const auto& groupMatch = religionGroupMapper.getMappingForEU4ReligionGroup(eu4Religion.group))
	{
		religionDef.traits.emplace(groupMatch->getTrait());
		religionDef.taboos = groupMatch->getTaboos();
		if (!eu4Religion.trappings.empty())
			trappings = eu4Religion.trappings;
		icon = groupMatch->getIcon();
	}
	else
	{
		Log(LogLevel::Warning) << "Religion " << v3ReligionName << " is using eu4 religion group " << eu4Religion.group << " which isn't mapped!";
		return religionDef;
	}

	// We now have an icon and possibly trappings. Icon is a  vic3 religion where we need to grab the exact icon filepath.
	// Trappings are an eu4 source religion that we need to grab the icon filepath of the vic3 religion it maps to.
	if (!trappings.empty() && eu4ToV3ReligionMap.contains(trappings))
	{
		if (const auto& targetDef = religionDefinitionLoader.getReligionDef(eu4ToV3ReligionMap.at(trappings)))
			religionDef.texture = targetDef->texture;
		// if this fails for whatever reason, we may yet silently fallback on the icon. Noone will notice anyway.
	}
	if (religionDef.texture.empty() && !icon.empty())
	{
		if (const auto& targetDef = religionDefinitionLoader.getReligionDef(icon))
			religionDef.texture = targetDef->texture;
	}

	// If all else fails, eh, it's their problem.
	if (religionDef.texture.empty())
		Log(LogLevel::Warning) << "Religion " << v3ReligionName << " has a misdefined icon!";

	return religionDef;
}
