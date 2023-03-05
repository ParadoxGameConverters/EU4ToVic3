#include "CultureMappingRule.h"
#include "ClayManager/ClayManager.h"
#include "CommonRegexes.h"
#include "Configuration.h"
#include "CultureLoader/CultureLoader.h"
#include "ParserHelpers.h"
#include "ReligionLoader/ReligionLoader.h"
#include <sstream>

void mappers::CultureMappingRule::loadMappingRules(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::CultureMappingRule::loadMappingRules(const std::string& theString)
{
	registerKeys();
	auto theStream = std::stringstream(theString);
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::CultureMappingRule::registerKeys()
{
	registerRegex(R"(@\w+)", [this](const std::string& macro, std::istream& theStream) {
		requestedMacros.insert(macro);
	});
	registerKeyword("vic3", [this](std::istream& theStream) {
		v3culture = commonItems::getString(theStream);
	});
	registerKeyword("eu4", [this](std::istream& theStream) {
		cultures.insert(commonItems::getString(theStream));
	});
	registerKeyword("eu4group", [this](std::istream& theStream) {
		cultureGroups.insert(commonItems::getString(theStream));
	});
	registerKeyword("religion", [this](std::istream& theStream) {
		religions.insert(commonItems::getString(theStream));
	});
	registerKeyword("religion_group", [this](std::istream& theStream) {
		religionGroups.insert(commonItems::getString(theStream));
	});
	registerKeyword("region", [this](std::istream& theStream) {
		regions.insert(commonItems::getString(theStream));
	});
	registerKeyword("owner", [this](std::istream& theStream) {
		owners.insert(commonItems::getString(theStream));
	});
	registerKeyword("neoculture_override", [this](std::istream& theStream) {
		neocultureOverride = (commonItems::getString(theStream) == "yes");
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::CultureMappingRule::cultureMatch(const V3::ClayManager& clayManager,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader,
	 const std::string& eu4culture,
	 const std::string& eu4religion,
	 const std::string& v3state,
	 const std::string& v3ownerTag) const
{
	// We need at least a viable incoming EU4culture
	if (eu4culture.empty())
		return std::nullopt;

	// if we fail on both cultural match and culture group match, bail
	const auto& incCultureGroup = cultureLoader.getGroupNameForCulture(eu4culture);
	if (!cultures.contains(eu4culture) && (!incCultureGroup || !cultureGroups.contains(*incCultureGroup)))
		return std::nullopt;

	// if there's an owner requirement and we fail, bail
	if (!owners.empty())
		if (v3ownerTag.empty() || !owners.contains(v3ownerTag))
			return std::nullopt;

	// if there's a religion and we fail on both religions and groups, bail
	if (!religions.empty() || !religionGroups.empty())
	{
		if (eu4religion.empty())
			return std::nullopt;

		// need to check both religions and groups
		const auto incReligiousGroup = religionLoader.getGroupForReligion(eu4religion);
		if (!incReligiousGroup)
			return std::nullopt;

		if (!religions.contains(eu4religion) && !religionGroups.contains(*incReligiousGroup))
			return std::nullopt;
	}

	// If there's a state given, don't fail on regions checks.
	if (!regions.empty())
	{
		if (v3state.empty())
			return std::nullopt;

		auto regionMatch = false;
		for (const auto& region: regions)
		{
			if (!clayManager.regionIsValid(region))
			{
				// Regions can change between versions so don't react to invalid region name.
				continue;
			}
			if (clayManager.stateIsInRegion(v3state, region))
				regionMatch = true;
		}
		if (!regionMatch)
			return std::nullopt;
	}
	return v3culture;
}

std::optional<std::string> mappers::CultureMappingRule::cultureRegionalMatch(const V3::ClayManager& clayManager,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader,
	 const std::string& eu4culture,
	 const std::string& eu4religion,
	 const std::string& v3state,
	 const std::string& v3ownerTag) const
{
	// This is useful for generating neocultures in specific target regions like the new world without actually being there.

	// This is a regional match. We need a mapping within the given SPECIFIC region, so if the
	// mapping rule has no regional qualifiers it needs to fail.

	// If we have a Neoculture Override, cheat and try to match anyway. This way brazilians match brazilian directly although the link lacks region.
	if (regions.empty() && !neocultureOverride)
		return std::nullopt;

	// Otherwise, as usual.
	return cultureMatch(clayManager, cultureLoader, religionLoader, eu4culture, eu4religion, v3state, v3ownerTag);
}

std::optional<std::string> mappers::CultureMappingRule::cultureNonRegionalNonReligiousMatch(const V3::ClayManager& clayManager,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader,
	 const std::string& eu4culture,
	 const std::string& eu4religion,
	 const std::string& v3state,
	 const std::string& v3ownerTag) const
{
	// This is useful when generating neocultures by asking what a given original culture maps to absent any specific qualifiers.

	// This is a non regional non religious match. We need a mapping without any region/religion, so if the
	// mapping rule has any regional/religious qualifiers it needs to fail.
	if (!regions.empty())
		return std::nullopt;
	if (!religions.empty() || !religionGroups.empty())
		return std::nullopt;

	// Otherwise, as usual.
	return cultureMatch(clayManager, cultureLoader, religionLoader, eu4culture, eu4religion, v3state, v3ownerTag);
}
