#include "CountryMappings.h"
#include "CountryMapping.h"
#include "CultureGroups/CultureGroups.h"
#include "CultureGroups/CultureGroup.h"
#include "EU4World/Country/EU4Country.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "ProvinceMappings/ProvinceMapper.h"
#include "V3World/Localisation/Localisation.h"
#include <algorithm>
#include <iomanip>
#include <locale>
#include "CommonRegexes.h"

mappers::CountryMappings::CountryMappings():
	 titleMapper(std::make_unique<TitleMapper>()), colonialTagMapper(std::make_unique<ColonialTagMapper>()),
	 v2provinceRegionsMapper(std::make_unique<V2ProvinceRegionsMapper>())
{
	/* This rather involved mapper is at the heart of the converter. Unlike simpler title->tag mappers of CK* series, this one requires a lot of sub-mappers.
	 * Reason for this is that country location, culture and other modifiers can determine the course of the mapping. Furthermore, for countries with specific
	 * NAMES, which we may match against known names of CK titles, we can use those incoming CK TITLES to assign proper tags.
	 *
	 * We'll be dancing across multiple mappers so if you're new, follow the comments and the steps the converter takes.
	 */

	LOG(LogLevel::Info) << "Parsing Country Mapping Rules.";
	registerKeys();
	parseFile("configurables/country_mappings.txt");
	clearRegisteredKeywords();
}

mappers::CountryMappings::CountryMappings(std::istream& mainStream,
	 std::istream& titleMapperStream,
	 std::istream& v2RegionsStream,
	 std::istream& colonialTagStream):
	 titleMapper(std::make_unique<TitleMapper>(titleMapperStream)),
	 colonialTagMapper(std::make_unique<ColonialTagMapper>(colonialTagStream)),
	 v2provinceRegionsMapper(std::make_unique<V2ProvinceRegionsMapper>(v2RegionsStream))

{
	registerKeys();
	parseStream(mainStream);
	clearRegisteredKeywords();
}

void mappers::CountryMappings::registerKeys()
{
	/* Step 1. Load our mappings. Already we hit exceptions, as we map against reforms as well as flags. We support universal mappings, so we can
	 * encounter incomplete rules without our key identifier - eu4 tag, and can map to the target TAG with appropriate reforms or flags.
	 */
	registerKeyword("link", [this](const std::string& unused, std::istream& theStream) {
		const CountryMapping newMapping(theStream);
		if (!newMapping.getEU4Tag().empty())
			eu4TagToV2TagsRules.emplace_back(std::make_pair(newMapping.getEU4Tag(), newMapping));
		else
			eu4TagToV2TagsRules.emplace_back(std::make_pair("---custom---", newMapping));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void mappers::CountryMappings::createMappings(const std::shared_ptr<CultureGroups>& theCultureGroupsMapper,
	 const std::map<std::string, std::shared_ptr<EU4::Country>>& eu4Countries,
	 const ProvinceMapper& provinceMapper)
{
	/* Step 2. This is entry proper, feeding it incoming eu4 countries and external mappers. First we try to see if any EU4 countries are colonies,
	 * putting them aside, as those have special treatment.
	 */

	cultureGroupsMapper = theCultureGroupsMapper;
	LOG(LogLevel::Info) << "Creating Country Mappings.";

	std::vector<std::shared_ptr<EU4::Country>> colonialCountries;
	for (const auto& EU4Country: eu4Countries)
		if (isPotentialColonialReplacement(EU4Country))
			colonialCountries.emplace_back(EU4Country.second);
		else
			makeOneMapping(EU4Country.second); // Make one mapping is a no-fail-allowed operation.

	for (const auto& colonialCountry: colonialCountries)
	{
		const auto& success = attemptColonialReplacement(*colonialCountry, provinceMapper);
		if (!success)
			makeOneMapping(colonialCountry);
	}
}

bool mappers::CountryMappings::isPotentialColonialReplacement(const std::pair<std::string, std::shared_ptr<EU4::Country>>& country)
{
	// Colonies are invariably C04 or similar. We're only concerned about countries which are still dependent, not released ones.
	if (country.second->isColony() && tagIsAlphaDigitDigit(country.first))
		return true;
	return false;
}

bool mappers::CountryMappings::tagIsAlphaDigitDigit(const std::string& tag)
{
	return isalpha(tag[0]) && isdigit(tag[1]) && isdigit(tag[2]);
}

void mappers::CountryMappings::makeOneMapping(const std::shared_ptr<EU4::Country>& country)
{
	/* Step 3. We attempt straight mapping according to input files. Failing that, we go into names and CK titles looking for targets,
	 * and if all fails, we generate a new dynamic tag for the country.
	 */

	const auto& EU4Tag = country->getTag();

	auto mapped = attemptStraightMapping(*country, EU4Tag);
	if (mapped)
		return;

	// There was no functional mapping in country_mapping.txt. Let's see if we can locate a CK title from the country's name or
	// from title_map.txt that we can map into vic2.

	const auto& potentialCKTitle = determineMappableCKTitle(*country);
	if (potentialCKTitle)
		mapped = attemptStraightMapping(*country, *potentialCKTitle);
	if (mapped)
		return;

	// with no CK fallback, or if we're a custom country, generate own tag.
	const auto& newVic2Tag = generateNewTag();
	mapToTag(EU4Tag, newVic2Tag);
}

bool mappers::CountryMappings::attemptStraightMapping(const EU4::Country& country, const std::string& EU4Identifier)
{
	/* Step 4. Here we finally try to match against one of our known mappings.
	 */
	auto mapped = false;
	for (const auto& mappingRule: eu4TagToV2TagsRules)
	{
		if (mappingRule.first != "---custom---" && mappingRule.first != EU4Identifier)
			continue;
		if (!mappingRule.second.getReforms().empty())
		{
			auto found = false;
			for (const auto& requiredReform: mappingRule.second.getReforms())
			{
				if (country.hasReform(requiredReform))
					found = true;
			}
			if (!found)
				continue;
		}
		if (!mappingRule.second.getFlags().empty())
		{
			auto found = false;
			for (const auto& requiredFlag: mappingRule.second.getFlags())
			{
				if (country.hasFlag(requiredFlag))
					found = true;
			}
			if (!found)
				continue;
		}

		// We have found a solid mapping candidate among existing definitions. However, since our incoming EU4Identifier may be a title,
		// we need to map against the actual EU4 tag.
		mapped = mapToVic2Country(mappingRule.second.getVic2Tag(), country.getTag());
		if (mapped)
			return mapped;
	}
	return mapped;
}

bool mappers::CountryMappings::mapToVic2Country(const std::string& V2Tag, const std::string& EU4Tag)
{
	// It's quite possible our target tag has already been used up.
	if (tagIsAlreadyAssigned(V2Tag))
		return false;

	// Otherwise, we're free to create a mapping.
	mapToTag(EU4Tag, V2Tag);
	return true;
}

std::string mappers::CountryMappings::generateNewTag()
{
	std::ostringstream generatedV2TagStream;
	generatedV2TagStream << generatedV2TagPrefix << std::setfill('0') << std::setw(2) << generatedV2TagSuffix;
	const auto& vic2Tag = generatedV2TagStream.str();

	++generatedV2TagSuffix;
	if (generatedV2TagSuffix > 99)
	{
		generatedV2TagSuffix = 0;
		--generatedV2TagPrefix;
	}

	return vic2Tag;
}

void mappers::CountryMappings::mapToTag(const std::string& eu4Tag, const std::string& vic2Tag)
{
	eu4TagToV2TagMap.insert(std::make_pair(eu4Tag, vic2Tag));
	v2TagToEU4TagMap.insert(std::make_pair(vic2Tag, eu4Tag));
}

std::optional<std::string> mappers::CountryMappings::determineMappableCKTitle(const EU4::Country& country)
{
	// We will attempt to locate a CK title based on country's name. This works only for simple names, nothing fancy,
	// but it's better than nothing.

	if (country.isCustom())
		return std::nullopt; // Custom countries must get generated V2 tags.

	// Look for any reasonable title that matcher, or even looks like our name, that we know of (in title_map.txt).
	auto title = getTitle(country.getName("english"));

	if (!title)
		return std::nullopt;

	// Do we have that title in our mapping rules?
	for (const auto& potentialMapping: eu4TagToV2TagsRules)
	{
		if (potentialMapping.first == *title)
			return *title;
	}

	return std::nullopt;
}

bool mappers::CountryMappings::attemptColonialReplacement(EU4::Country& country, const ProvinceMapper& provinceMapper)
{
	/* Step 5. Now it gets fun. We know a country may be a colony, and we have rules for those. We'll now gather all
	 * prerequisite info and see if we can match it against one of our known colonies that have predefined tags (in colonial_tags.txt).
	 */

	// incoming region
	const auto EU4Capital = country.getCapital();
	std::string eu4Region;
	if (const auto& potentialEU4Region = provinceMapper.getColonialRegionForProvince(EU4Capital); potentialEU4Region)
		eu4Region = *potentialEU4Region;

	// target region
	int vic2Capital = 0;
	if (const auto& potentialVic2Capitals = provinceMapper.getVic2ProvinceNumbers(EU4Capital); !potentialVic2Capitals.empty())
		vic2Capital = *potentialVic2Capitals.begin();
	std::string vic2Region;
	if (const auto& v2Region = v2provinceRegionsMapper->getRegionForProvince(vic2Capital); v2Region)
		vic2Region = *v2Region;

	// culture group
	std::string cultureGroup;
	if (const auto& culturalGroup = cultureGroupsMapper->getGroupForCulture(country.getPrimaryCulture()); culturalGroup)
		cultureGroup = culturalGroup->getName();

	// and match
	if (const auto& colonyMatch = colonialTagMapper->getColonialTag(eu4Region, vic2Region, cultureGroup); colonyMatch)
	{
		country.setColonialRegion(eu4Region);
		if (!tagIsAlreadyAssigned(*colonyMatch))
		{
			mapToTag(country.getTag(), *colonyMatch);
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

bool mappers::CountryMappings::tagIsAlreadyAssigned(const std::string& vic2Tag) const
{
	return v2TagToEU4TagMap.contains(vic2Tag);
}

std::optional<std::string> mappers::CountryMappings::getV2Tag(const std::string& eu4Tag) const
{
	// This is an override for anything that looks like a rebellious title.
	const std::vector<std::string> eu4RebelTags = {"REB", "PIR", "NAT"};
	const std::string v2RebelTag = "REB";
	if (find(eu4RebelTags.begin(), eu4RebelTags.end(), eu4Tag) != eu4RebelTags.end())
	{
		return std::optional{v2RebelTag};
	}

	if (const auto& findIter = eu4TagToV2TagMap.find(eu4Tag); findIter != eu4TagToV2TagMap.end())
		return findIter->second;
	else
		return std::nullopt;
}

std::optional<std::string> mappers::CountryMappings::getTitle(const std::string& countryName) const
{
	// The most bizarre part of the mapper. First look for titles that match our name, then get creative.

	auto name = V2::Localisation::convert(countryName);
	transform(name.begin(), name.end(), name.begin(), ::tolower);

	auto title = titleMapper->getTitleForName(name);
	if (!title)
	{
		const auto& titleName = V2::Localisation::stripAccents(name);
		const auto& c_name = "c_" + titleName;
		const auto& d_name = "d_" + titleName;
		const auto& k_name = "k_" + titleName;
		const auto& e_name = "e_" + titleName;

		if (titleMapper->doesTitleExist(c_name))
		{
			title = c_name;
		}
		else if (titleMapper->doesTitleExist(d_name))
		{
			title = d_name;
		}
		else if (titleMapper->doesTitleExist(k_name))
		{
			title = k_name;
		}
		else if (titleMapper->doesTitleExist(e_name))
		{
			title = e_name;
		}
	}
	return title;
}
