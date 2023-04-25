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
#include "TraitDefinitionLoader/TraitDefinitionLoader.h"
#include <numeric>
#include <ranges>

namespace
{
std::string normalizeString(const std::string& input)
{
	auto toReturn = commonItems::normalizeUTF8Path(input);
	std::replace(toReturn.begin(), toReturn.end(), ' ', '_');
	std::replace(toReturn.begin(), toReturn.end(), '\'', '_');
	return toReturn;
}
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
		auto normName = "eu4_" + normalizeString(name);
		cultureDef.nameLocBlock.emplace(normName, name);
		cultureDef.commonLastNames.emplace(normName);
		cultureDef.nobleLastNames.emplace(normName);
	}
	for (const auto& name: sourceCulture.getMaleNames())
	{
		auto normName = "eu4_" + normalizeString(name);
		cultureDef.nameLocBlock.emplace(normName, name);
		cultureDef.maleCommonFirstNames.emplace(normName);
		cultureDef.maleRegalFirstNames.emplace(normName);
	}
	for (const auto& name: sourceCulture.getFemaleNames())
	{
		auto normName = "eu4_" + normalizeString(name);
		cultureDef.nameLocBlock.emplace(normName, name);
		cultureDef.femaleCommonFirstNames.emplace(normName);
		cultureDef.femaleRegalFirstNames.emplace(normName);
	}
}

std::vector<std::string> breakDownCulturalName(const std::string& eu4CultureName)
{
	// dynamic-afghani-culture-num1
	// dynamic-afghani-panjabi-culture-num2

	std::vector<std::string> componentCultures;
	if (!eu4CultureName.starts_with("dynamic-") || eu4CultureName.size() <= 8)
	{
		componentCultures.emplace_back(eu4CultureName);
		return componentCultures;
	}

	// easy now. eeeasy. Max 2 cultures.
	auto theName = eu4CultureName.substr(8, eu4CultureName.size());
	if (const auto& pos = theName.find('-'); pos != std::string::npos && pos < theName.size() - 1)
	{
		componentCultures.emplace_back(theName.substr(0, pos));
		theName = theName.substr(pos + 1, theName.size());
	}
	if (const auto& pos = theName.find('-'); pos != std::string::npos && pos < theName.size() - 1)
	{
		if (theName.substr(0, pos) != "culture")
			componentCultures.emplace_back(theName.substr(0, pos));
	}
	return componentCultures;
}

void importLocalizationForNeoCulture(mappers::CultureDef& newDef, const mappers::ColonialRegionMapping& colony, int colonialCulturesCount)
{
	if (colonialCulturesCount == 1)
	{
		// overwriting everything with preset english. sorry. yup.
		for (auto& loc: newDef.locBlock | std::views::values)
			loc = colony.getAloneName();
	}
	else
	{
		std::string eng;
		if (newDef.locBlock.contains("english"))
			eng = newDef.locBlock.at("english");
		else
		{
			// ??? Don't have english? hmm.
			eng = newDef.name;
		}
		const auto& splitSuffix = colony.getSplitName();
		if (const auto& pos = splitSuffix.find("$PARENT$"); pos != std::string::npos)
		{
			eng = splitSuffix.substr(0, pos) + eng;
			if (splitSuffix.size() > pos + 8)
				eng += splitSuffix.substr(pos + 8, splitSuffix.size());
		}
		for (auto& loc: newDef.locBlock | std::views::values)
			loc = eng;
	}
}

commonItems::Color generateCultureColors(const std::string& cultureName, const std::string& saltName)
{
	const auto salt = std::accumulate(saltName.begin(), saltName.end(), 0, [](int sum, const char letter) {
		return sum + static_cast<int>(static_cast<unsigned char>(letter));
	});

	if (cultureName.size() < 3)
		return commonItems::Color{std::array{200, 164, 213}};		  // I just like these numbers.
	int r = (static_cast<int>(cultureName[0]) * 29 + salt) % 256; // should be random enough.
	int g = (static_cast<int>(cultureName[1]) * 17 + salt) % 256;
	int b = (static_cast<int>(cultureName[2]) * 23 + salt) % 256;
	if (r < 50) // make them brighter.
		r += 50;
	if (g < 50)
		g += 50;
	if (b < 50)
		b += 50;
	return commonItems::Color{std::array{r, g, b}};
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

void mappers::CultureMapper::loadColonialRules(std::istream& theStream)
{
	colonialRegionMapper.loadMappingRules(theStream);
}

void mappers::CultureMapper::loadColonialRules(const std::string& fileName)
{
	colonialRegionMapper.loadMappingRules(fileName);
}

void mappers::CultureMapper::loadWesternizationRules(std::istream& theStream)
{
	westernizationMapper.loadMappingRules(theStream);
}

void mappers::CultureMapper::loadWesternizationRules(const std::string& fileName)
{
	westernizationMapper.loadMappingRules(fileName);
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

std::optional<std::string> mappers::CultureMapper::cultureMatch(const V3::ClayManager& clayManager,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader,
	 const std::string& eu4culture,
	 const std::string& eu4religion,
	 const std::string& v3state,
	 const std::string& v3ownerTag,
	 bool neoCultureRequest,
	 bool silent)
{
	for (const auto& cultureMappingRule: cultureMapRules)
	{
		if (!neoCultureRequest)
		{
			if (const auto& possibleMatch =
					  cultureMappingRule.cultureMatch(clayManager, cultureLoader, religionLoader, eu4culture, eu4religion, v3state, v3ownerTag);
				 possibleMatch)
			{
				usedCultures.emplace(*possibleMatch);
				return *possibleMatch;
			}
		}
		else
		{
			// we want neocultures in a very specific region. General rules don't apply unless they cover our region.
			if (const auto& possibleMatch =
					  cultureMappingRule.cultureRegionalMatch(clayManager, cultureLoader, religionLoader, eu4culture, eu4religion, v3state, v3ownerTag);
				 possibleMatch)
			{
				usedCultures.emplace(*possibleMatch);
				return *possibleMatch;
			}
		}
	}

	// if this normal culture is already recorded as unmapped, all is well.
	if (!neoCultureRequest && unmappedCultures.contains(eu4culture))
	{
		usedCultures.emplace(eu4culture);
		return eu4culture;
	}

	// Is this a normal unmapped culture? We shouldn't be here - it should have been recorded when expanding unless it's not present in vanilla eu4 defs,
	// which is bad in itself.
	if (!neoCultureRequest)
	{
		if (!silent)
			Log(LogLevel::Warning) << "! CultureMapper - Attempting to match culture " << eu4culture << " in state " << v3state << " failed.";
		unmappedCultures.emplace(eu4culture);
		usedCultures.emplace(eu4culture);
		return eu4culture;
	}

	// For neoculture requests we need to consult and potentially expand our global registry.
	return getNeoCultureMatch(eu4culture, v3state, clayManager);
}

std::optional<std::string> mappers::CultureMapper::suspiciousCultureMatch(const V3::ClayManager& clayManager,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader,
	 const std::string& eu4culture,
	 const std::string& eu4religion,
	 const std::string& v3state,
	 const std::string& v3ownerTag)
{
	if (const auto& potentialColony = colonialRegionMapper.getColonyNameForState(v3state, clayManager); potentialColony)
	{
		// this is option 1.
		if (colonyNeoCultureTargets.contains(*potentialColony) && colonyNeoCultureTargets.at(*potentialColony).contains(eu4culture))
			return colonyNeoCultureTargets.at(*potentialColony).at(eu4culture);
	}

	// Otherwise, do a straight match at that location.
	return cultureMatch(clayManager, cultureLoader, religionLoader, eu4culture, eu4religion, v3state, v3ownerTag, false, false);
}

std::string mappers::CultureMapper::getNeoCultureMatch(const std::string& eu4culture, const std::string& v3state, const V3::ClayManager& clayManager)
{
	if (v3state.empty()) // possibly pinging a general location.
	{
		unmappedCultures.emplace(eu4culture);
		usedCultures.emplace(eu4culture);
		return eu4culture;
	}

	const auto& colony = colonialRegionMapper.getColonyNameForState(v3state, clayManager);
	if (!colony)
	{
		// not uncommon if we don't have defined colonies.
		Log(LogLevel::Warning) << "We don't have a defined colony for " << v3state << "! Can't create neoculture for " << eu4culture;
		unmappedCultures.emplace(eu4culture);
		usedCultures.emplace(eu4culture);
		return eu4culture;
	}

	if (colonyNeoCultureTargets.contains(*colony) && colonyNeoCultureTargets.at(*colony).contains(eu4culture))
		return colonyNeoCultureTargets.at(*colony).at(eu4culture);

	// we have to generate a new neo culture. SIMPLE!
	auto generated = "neo-" + *colony + "-" + eu4culture;
	// neo-usa_north_colony-dynamic-culture1-culture7-culture-num1. Trivial.

	if (!colonyNeoCultureTargets.contains(*colony))
		colonyNeoCultureTargets.emplace(*colony, std::map<std::string, std::string>{});
	colonyNeoCultureTargets.at(*colony).emplace(eu4culture, generated);
	unmappedCultures.emplace(generated);
	usedCultures.emplace(generated);
	return generated;
}

void mappers::CultureMapper::expandCulturalMappings(const V3::ClayManager& clayManager,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader)
{
	// We'll simply iterate over all known eu4 cultures, see what maps, and then record what we see for the first time.

	for (const auto& cultureGroup: cultureLoader.getCultureGroupsMap() | std::views::values)
		for (const auto& cultureName: cultureGroup.getCultures() | std::views::keys)
			if (!cultureMatch(clayManager, cultureLoader, religionLoader, cultureName, "", "", "", false, true))
				unmappedCultures.emplace(cultureName);

	Log(LogLevel::Info) << "<> Additional " << unmappedCultures.size() << " cultures imported.";
}

void mappers::CultureMapper::loadCultureDefinitions(const commonItems::ModFilesystem& modFS)
{
	CultureDefinitionLoader cultureDefinitionLoader;
	cultureDefinitionLoader.loadDefinitions(modFS);
	v3CultureDefinitions = cultureDefinitionLoader.getDefinitions();
}

void mappers::CultureMapper::loadTraitDefinitions(const commonItems::ModFilesystem& modFS)
{
	TraitDefinitionLoader traitDefinitionLoader;
	traitDefinitionLoader.loadDefinitions(modFS);
	v3TraitDefinitions = traitDefinitionLoader.getDefinitions();
}

void mappers::CultureMapper::generateCultureDefinitions(const std::string& nameListsPath,
	 const std::string& nameListMapPath,
	 const std::string& cultureTraitsPath,
	 const V3::ClayManager& clayManager,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader,
	 const EU4::EU4LocalizationLoader& eu4Locs)
{
	Log(LogLevel::Info) << "-> Generating culture definitions.";

	NameListLoader nameListLoader;
	nameListLoader.loadNameLists(nameListsPath);
	NameListMapper nameListMapper;
	nameListMapper.loadMappingRules(nameListMapPath);
	CultureTraitMapper cultureTraitMapper;
	cultureTraitMapper.loadMappingRules(cultureTraitsPath);

	// shove existing vanilla defs into the bin.
	const auto& defCount = v3CultureDefinitions.size();

	for (const auto& eu4CultureName: usedCultures)
	{
		// do we have a ready definition already?
		if (v3CultureDefinitions.contains(eu4CultureName))
			continue;

		// neocultures need special treatment.
		if (eu4CultureName.starts_with("neo-"))
		{
			std::string actualEU4CultureName;
			ColonialRegionMapping colony;
			std::string actualColonyName;

			for (const auto& [colonyName, cultureMap]: colonyNeoCultureTargets)
				for (const auto& [eu4Culture, neoCultureName]: cultureMap)
					if (neoCultureName == eu4CultureName)
					{
						actualEU4CultureName = eu4Culture;
						actualColonyName = colonyName;
						colony = colonialRegionMapper.getColonialRegions().at(colonyName);
					}
			auto newDef = generateCultureDefinition(clayManager,
				 actualEU4CultureName,
				 cultureTraitMapper,
				 nameListMapper,
				 nameListLoader,
				 cultureLoader,
				 religionLoader,
				 eu4Locs,
				 eu4CultureName);
			newDef.name = eu4CultureName;
			importLocalizationForNeoCulture(newDef, colony, static_cast<int>(colonyNeoCultureTargets.at(actualColonyName).size()));
			v3CultureDefinitions.emplace(eu4CultureName, newDef);
			continue;
		}

		if (!cultureLoader.containsCulture(eu4CultureName))
		{
			Log(LogLevel::Warning) << "EU4 cultures don't contain " << eu4CultureName << "? What are we even doing?";
			continue;
		}

		// generate a definition and file.
		auto newDef =
			 generateCultureDefinition(clayManager, eu4CultureName, cultureTraitMapper, nameListMapper, nameListLoader, cultureLoader, religionLoader, eu4Locs);
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

int mappers::CultureMapper::getWesternizationScoreForCulture(const std::string& cultureName) const
{
	if (!v3CultureDefinitions.contains(cultureName))
	{
		Log(LogLevel::Warning) << "Can't retrieve Westernization for unknown culture: " << cultureName;
		return 0;
	}

	return westernizationMapper.getWesternizationForTraits(v3CultureDefinitions.at(cultureName).traits);
}

int mappers::CultureMapper::getLiteracyScoreForCulture(const std::string& cultureName) const
{
	if (!v3CultureDefinitions.contains(cultureName))
	{
		Log(LogLevel::Warning) << "Can't retrieve Literacy for unknown culture: " << cultureName;
		return 0;
	}

	return westernizationMapper.getLiteracyForTraits(v3CultureDefinitions.at(cultureName).traits);
}

int mappers::CultureMapper::getIndustryScoreForCulture(const std::string& cultureName) const
{
	if (!v3CultureDefinitions.contains(cultureName))
	{
		Log(LogLevel::Warning) << "Can't retrieve Industry for unknown culture: " << cultureName;
		return 0;
	}

	return westernizationMapper.getIndustryForTraits(v3CultureDefinitions.at(cultureName).traits);
}

mappers::CultureDef mappers::CultureMapper::generateCultureDefinition(const V3::ClayManager& clayManager,
	 const std::string& eu4CultureName,
	 const CultureTraitMapper& cultureTraitMapper,
	 const NameListMapper& nameListMapper,
	 const NameListLoader& nameListLoader,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader,
	 const EU4::EU4LocalizationLoader& eu4Locs,
	 const std::string& seedName)
{
	CultureDef newDef;
	newDef.name = eu4CultureName;
	bool dynamicCulture = false;
	if (eu4CultureName.starts_with("dynamic-") && eu4CultureName.size() > 8)
		dynamicCulture = true;
	bool primaryComponent = true;

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

		// If we broke down a dynamic culture, use its first component to find a closest vanilla vic3 culture. This is relevant for formables/releasables.
		if (dynamicCulture && primaryComponent)
		{
			if (const auto& match = cultureMatch(clayManager, cultureLoader, religionLoader, sourceCultureName, "", "", ""); match)
			{
				relatedCultures[*match].emplace(eu4CultureName);
				// eu4 cultures have no color defined. We need to seed-generate something.
				if (!seedName.empty())
					newDef.color = generateCultureColors(sourceCultureName, seedName);
				else
					newDef.color = generateCultureColors(sourceCultureName, eu4CultureName);
				primaryComponent = false;
			}
			else
				// primaryComponent intentionally remains true if we failed on the match so maybe second component can match something and become the primary
				// component.
				Log(LogLevel::Warning) << "Cannot find related/base culture of " << eu4CultureName << "!";
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
			// Can be common. Be silent about this.
			Log(LogLevel::Debug) << "EU4 culture " << sourceCultureName << "/" << groupName << " has no mapped namelist! Falling back to EU4 names.";
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

	if (!newDef.color)
	{
		if (!seedName.empty())
			newDef.color = generateCultureColors(eu4CultureName, seedName);
		else
			newDef.color = generateCultureColors(eu4CultureName, eu4CultureName);
	}

	return newDef;
}

std::optional<bool> mappers::CultureMapper::doCulturesShareHeritageTrait(const std::string& cultureA, const std::string& cultureB) const
{
	if (!v3CultureDefinitions.contains(cultureA))
		return std::nullopt;

	if (!v3CultureDefinitions.contains(cultureB))
		return std::nullopt;

	for (const auto& traitA: v3CultureDefinitions.at(cultureA).traits)
	{
		if (!v3TraitDefinitions.contains(traitA) || !v3TraitDefinitions.at(traitA).isHeritageTrait)
			continue;

		if (v3CultureDefinitions.at(cultureB).traits.contains(traitA))
			return true;
	}

	return false;
}

std::optional<bool> mappers::CultureMapper::doCulturesShareNonHeritageTrait(const std::string& cultureA, const std::string& cultureB) const
{
	if (!v3CultureDefinitions.contains(cultureA))
		return std::nullopt;

	if (!v3CultureDefinitions.contains(cultureB))
		return std::nullopt;

	for (const auto& traitA: v3CultureDefinitions.at(cultureA).traits)
	{
		if (!v3TraitDefinitions.contains(traitA) || v3TraitDefinitions.at(traitA).isHeritageTrait)
			continue;

		if (v3CultureDefinitions.at(cultureB).traits.contains(traitA))
			return true;
	}

	return false;
}
