#include "CultureMapper.h"
#include "ClayManager/ClayManager.h"
#include "ClayManager/State/State.h"
#include "ClayManager/State/SubState.h"
#include "CommonRegexes.h"
#include "CultureDefinitionLoader/CultureDefinitionLoader.h"
#include "CultureLoader/CultureGroupParser.h"
#include "CultureLoader/CultureLoader.h"
#include "CultureMappingRule.h"
#include "CultureTraitMapper/CultureTraitMapper.h"
#include "LocalizationLoader/EU4LocalizationLoader.h"
#include "Log.h"
#include "NameListLoader/NameListLoader.h"
#include "NameListMapper/NameListMapper.h"
#include "ParserHelpers.h"
#include <ranges>

namespace
{

void copyNamePoolNames(mappers::CultureDef& cultureDef, const mappers::NameListEntry& namePool)
{
	cultureDef.commonLastNames = namePool.getDynastyNames();
	cultureDef.nobleLastNames = namePool.getDynastyNames();
	cultureDef.maleCommonFirstNames = namePool.getMaleNames();
	cultureDef.maleRegalFirstNames = namePool.getMaleNames();
	cultureDef.femaleCommonFirstNames = namePool.getFemaleNames();
	cultureDef.femaleRegalFirstNames = namePool.getFemaleNames();
}

void copyEU4Names(mappers::CultureDef& cultureDef, const EU4::CultureParser& sourceCulture)
{
	for (const auto& name: sourceCulture.getDynastyNames())
	{
		cultureDef.commonLastNames.emplace(name);
		cultureDef.nobleLastNames.emplace(name);
	}
	for (const auto& name: sourceCulture.getMaleNames())
	{
		cultureDef.maleCommonFirstNames.emplace(name);
		cultureDef.maleRegalFirstNames.emplace(name);
	}
	for (const auto& name: sourceCulture.getFemaleNames())
	{
		cultureDef.femaleCommonFirstNames.emplace(name);
		cultureDef.femaleRegalFirstNames.emplace(name);
	}
	cultureDef.win1252Names = true; // Will need to normalize them later.
}

std::set<std::string> breakDownCulturalName(const std::string& eu4CultureName)
{
	// dynamic-afghani-culture-num1
	// dynamic-afghani-panjabi-culture-num2

	std::set<std::string> componentCultures;
	if (!eu4CultureName.starts_with("dynamic-") || eu4CultureName.size() <= 8)
	{
		componentCultures.emplace(eu4CultureName);
		return componentCultures;
	}

	// easy now. eeeasy. Max 2 cultures.
	auto theName = eu4CultureName.substr(8, eu4CultureName.size());
	if (const auto& pos = theName.find('-'); pos != std::string::npos && pos < theName.size() - 1)
	{
		componentCultures.emplace(theName.substr(0, pos));
		theName = theName.substr(pos + 1, theName.size());
	}
	if (const auto& pos = theName.find('-'); pos != std::string::npos && pos < theName.size() - 1)
	{
		if (theName.substr(0, pos) != "culture")
			componentCultures.emplace(theName.substr(0, pos));
	}
	return componentCultures;
}

mappers::CultureDef generateCultureDefinition(const std::string& eu4CultureName,
	 const mappers::CultureTraitMapper& cultureTraitMapper,
	 const mappers::NameListMapper& nameListMapper,
	 const mappers::NameListLoader& nameListLoader,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::EU4LocalizationLoader& eu4Locs)
{
	mappers::CultureDef newDef;
	newDef.name = eu4CultureName;

	// NAMEPOOLS & TRAITS
	// What's in a name? Could be a number of things.
	for (const auto& sourceCultureName: breakDownCulturalName(eu4CultureName))
	{
		const auto& sourceCultureGroup = cultureLoader.getGroupForCulture(sourceCultureName);
		if (!sourceCultureGroup)
		{
			// heavy corruption. CK3 ALWAYS maps to existing EU4 cultures! This is bullshit.
			Log(LogLevel::Error) << "Cannot find EU4 culture " << sourceCultureName << " in EU4 cultures! THIS IS BAD. Cannot stat " << eu4CultureName << "!";
			continue;
		}

		const auto& sourceCulture = sourceCultureGroup->getCultures().at(sourceCultureName);
		const auto& groupName = sourceCultureGroup->getName();
		if (const auto& traitsblock = cultureTraitMapper.getTraitsForCulture(sourceCultureName, groupName); !traitsblock)
		{
			Log(LogLevel::Warning) << "EU4 culture " << sourceCultureName << " has no mapped traits! Rectify!";
		}
		else
		{
			newDef.traits.insert(traitsblock->getTraits().begin(), traitsblock->getTraits().end());
			newDef.ethnicities.emplace(traitsblock->getEthnicity());
			newDef.graphics = traitsblock->getGraphics(); // Any will do but there must be only one.
		}
		if (const auto& nameListMatch = nameListMapper.getNamesForCulture(sourceCultureName, groupName); !nameListMatch)
		{
			Log(LogLevel::Warning) << "EU4 culture " << sourceCultureName << "/" << groupName << " has no mapped namelist! Falling back to EU4 names.";
			copyEU4Names(newDef, sourceCulture); // ok, use eu4 names.
		}
		else if (const auto& nameList = nameListLoader.getNameList(nameListMatch->getNamePool()); !nameList)
		{
			Log(LogLevel::Warning) << "EU4 culture " << sourceCultureName << " has has a namepool " << nameListMatch->getNamePool()
										  << " which doesn't exist! Falling back to EU4 names.";
			copyEU4Names(newDef, sourceCulture); // ok, use eu4 names. sigh.
		}
		else
		{
			copyNamePoolNames(newDef, *nameList);
		}
	}

	// sanities
	if (newDef.ethnicities.empty())
		newDef.ethnicities.emplace("neutral"); // fallback (?)

	// graphics
	if (newDef.graphics.empty())
		newDef.graphics = "generic";

	// locs
	if (const auto& locMatch = eu4Locs.getTextInEachLanguage(eu4CultureName); !locMatch)
		Log(LogLevel::Warning) << "WHY doesn't " << eu4CultureName << " have a localization in EU4?";
	else
		newDef.locBlock = *locMatch;

	return newDef;
}
} // namespace


void mappers::CultureMapper::loadMappingRules(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::CultureMapper::loadMappingRules(const std::string& fileName)
{
	Log(LogLevel::Info) << "-> Parsing culture mapping rules.";
	registerKeys();
	parseFile(fileName);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << cultureMapRules.size() << " rules loaded.";
}

void mappers::CultureMapper::registerKeys()
{
	registerRegex(R"(@\w+)", [this](const std::string& macro, std::istream& theStream) {
		encounteredMacros.emplace(macro, commonItems::getString(theStream));
	});
	registerKeyword("link", [this](std::istream& theStream) {
		CultureMappingRule rule;
		rule.loadMappingRules(theStream);
		for (const auto& macro: rule.getRequestedMacros())
			if (encounteredMacros.contains(macro))
				rule.loadMappingRules(encounteredMacros.at(macro));
		if ((!rule.getCultures().empty() || !rule.getCultureGroups().empty()) && !rule.getV3Culture().empty())
			cultureMapRules.emplace_back(rule);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

[[nodiscard]] std::optional<std::string> mappers::CultureMapper::cultureMatch(const V3::ClayManager& clayManager,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader,
	 const std::string& eu4culture,
	 const std::string& eu4religion,
	 const std::string& v3state,
	 const std::string& v3ownerTag,
	 bool neoCultureRequest)
{
	for (const auto& cultureMappingRule: cultureMapRules)
		if (const auto& possibleMatch = cultureMappingRule.cultureMatch(clayManager, cultureLoader, religionLoader, eu4culture, eu4religion, v3state, v3ownerTag);
			 possibleMatch)
			return *possibleMatch;

	// if this culture is already recorded as unmapped, all is well.
	if (unmappedCultures.contains(eu4culture))
		return eu4culture;

	// Is this a normal unmapped culture? We shouldn't be here - it should have been recorded when expanding unless it's not present in vanilla eu4 defs,
	// which is bad in itself.
	if (!neoCultureRequest)
	{
		Log(LogLevel::Warning) << "! CultureMapper - Attempting to match culture " << eu4culture << " in state " << v3state << " failed.";
		unmappedCultures.emplace(eu4culture);
		return eu4culture;
	}

	// For neoculture requests we need to consult and potentially expand our global registry.
	return getNeoCultureMatch(eu4culture, v3state, clayManager);
}

std::optional<std::string> mappers::CultureMapper::getNeoCultureMatch(const std::string& eu4culture,
	 const std::string& v3state,
	 const V3::ClayManager& clayManager)
{
	if (v3state.empty())
		return std::nullopt;

	auto colony = "col";

	if (colonyNeoCultureTargets.contains(colony) && colonyNeoCultureTargets.at(colony).contains(eu4culture))
		return colonyNeoCultureTargets.at(colony).at(eu4culture);

	// we have to generate a new neo culture.
	auto generated = "new";

	if (!colonyNeoCultureTargets.contains(colony))
		colonyNeoCultureTargets.emplace(colony, std::map<std::string, std::string>{});
	colonyNeoCultureTargets.at(colony).emplace(eu4culture, generated);
	return generated;
}

void mappers::CultureMapper::expandCulturalMappings(const V3::ClayManager& clayManager,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader)
{
	// We'll simply iterate over all known eu4 cultures, see what maps, and then record what we see for the first time.

	for (const auto& cultureGroup: cultureLoader.getCultureGroupsMap() | std::views::values)
		for (const auto& cultureName: cultureGroup.getCultures() | std::views::keys)
			if (!cultureMatch(clayManager, cultureLoader, religionLoader, cultureName, "", "", "", false))
				unmappedCultures.emplace(cultureName);

	Log(LogLevel::Info) << "<> Additional " << unmappedCultures.size() << " cultures imported.";
}

void mappers::CultureMapper::generateCultureDefinitions(const commonItems::ModFilesystem& modFS,
	 const std::string& nameListsPath,
	 const std::string& nameListMapPath,
	 const std::string& cultureTraitsPath,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::EU4LocalizationLoader& eu4Locs)
{
	Log(LogLevel::Info) << "-> Generating culture definitions.";

	CultureDefinitionLoader cultureDefinitionLoader;
	cultureDefinitionLoader.loadDefinitions(modFS);
	NameListLoader nameListLoader;
	nameListLoader.loadNameLists(nameListsPath);
	NameListMapper nameListMapper;
	nameListMapper.loadMappingRules(nameListMapPath);
	CultureTraitMapper cultureTraitMapper;
	cultureTraitMapper.loadMappingRules(cultureTraitsPath);

	// shove existing vanilla defs into the bin.
	v3CultureDefinitions = cultureDefinitionLoader.getDefinitions();
	const auto& defCount = v3CultureDefinitions.size();

	for (const auto& eu4CultureName: unmappedCultures)
	{
		// do we have a ready definition already?
		if (v3CultureDefinitions.contains(eu4CultureName))
			continue;

		// quick sanity, though this should never fire.
		if (!cultureLoader.containsCulture(eu4CultureName))
		{
			Log(LogLevel::Warning) << "EU4 cultures don't contain " << eu4CultureName << "? What are we even doing?";
			continue;
		}

		// generate a definition and file.
		auto newDef = generateCultureDefinition(eu4CultureName, cultureTraitMapper, nameListMapper, nameListLoader, cultureLoader, eu4Locs);
		v3CultureDefinitions.emplace(eu4CultureName, newDef);
	}

	Log(LogLevel::Info) << "-> Generated " << v3CultureDefinitions.size() - defCount << " culture definitions.";
}

void mappers::CultureMapper::injectReligionsIntoCultureDefs(const V3::ClayManager& clayManager)
{
	Log(LogLevel::Info) << "-> Updating primary religions for cultures according to census.";
	std::map<std::string, std::map<std::string, int>> culturalReligionPopSizeMap; // census cache

	for (const auto& state: clayManager.getStates() | std::views::values)
		for (const auto& subState: state->getSubStates())
			for (const auto& pop: subState->getSubStatePops().getPops())
				if (!pop.getCulture().empty() && !pop.getReligion().empty())
				{
					const auto& culture = pop.getCulture();
					const auto& religion = pop.getReligion();
					if (!culturalReligionPopSizeMap.contains(culture))
						culturalReligionPopSizeMap.emplace(culture, std::map<std::string, int>{});
					if (!culturalReligionPopSizeMap.at(culture).contains(religion))
						culturalReligionPopSizeMap.at(culture).emplace(religion, 0);
					culturalReligionPopSizeMap.at(culture).at(religion) += pop.getSize();
				}

	// file census results
	for (const auto& [culture, religionPopSizeMap]: culturalReligionPopSizeMap)
	{
		if (!v3CultureDefinitions.contains(culture)) // uh-huh.
			continue;
		if (religionPopSizeMap.empty())
			continue;
		const auto dominantReligion = std::max_element(std::begin(religionPopSizeMap), std::end(religionPopSizeMap), [](const auto& p1, const auto& p2) {
			return p1.second < p2.second;
		});
		v3CultureDefinitions.at(culture).religion = dominantReligion->first;
	}
	Log(LogLevel::Info) << "<> Update complete.";
}
