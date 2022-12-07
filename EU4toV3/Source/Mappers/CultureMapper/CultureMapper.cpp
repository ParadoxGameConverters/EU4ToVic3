#include "CultureMapper.h"
#include "CommonRegexes.h"
#include "CultureLoader/CultureGroupParser.h"
#include "CultureMappingRule.h"
#include "Log.h"
#include "ParserHelpers.h"
#include <ranges>

#include "CultureLoader/CultureLoader.h"

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
	 bool silent) const
{
	// Speed things up. Don't match dynamics.
	if (eu4culture.starts_with("dynamic-"))
		return eu4culture;

	for (const auto& cultureMappingRule: cultureMapRules)
	{
		const auto& possibleMatch = cultureMappingRule.cultureMatch(clayManager, cultureLoader, religionLoader, eu4culture, eu4religion, v3state, v3ownerTag);
		if (possibleMatch)
			return *possibleMatch;
	}

	// If we failed to match, we're dealing with an unmapped culture, which can happen only if the culture is present in save but NOT in installation. Warn and
	// bail.
	if (!silent)
	{
		if (v3state.empty())
			Log(LogLevel::Warning) << "! CultureMapper - Attempting to match culture " << eu4culture << " failed.";
		else
			Log(LogLevel::Warning) << "! CultureMapper - Attempting to match culture " << eu4culture << " in state " << v3state << " failed.";
	}
	return std::nullopt;
}

std::optional<std::string> mappers::CultureMapper::cultureRegionalMatch(const V3::ClayManager& clayManager,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader,
	 const std::string& eu4culture,
	 const std::string& eu4religion,
	 const std::string& v3state,
	 const std::string& v3ownerTag) const
{
	// Don't match dynamics for non-slave pops, they are same in eu4 and vic3.
	if (eu4culture.starts_with("dynamic-"))
		return eu4culture;

	for (const auto& cultureMappingRule: cultureMapRules)
	{
		const auto& possibleMatch =
			 cultureMappingRule.cultureRegionalMatch(clayManager, cultureLoader, religionLoader, eu4culture, eu4religion, v3state, v3ownerTag);
		if (possibleMatch)
			return *possibleMatch;
	}
	return std::nullopt;
}

std::optional<std::string> mappers::CultureMapper::cultureNonRegionalNonReligiousMatch(const V3::ClayManager& clayManager,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader,
	 const std::string& eu4culture,
	 const std::string& eu4religion,
	 const std::string& v3state,
	 const std::string& v3ownerTag) const
{
	// Don't match dynamics.
	if (eu4culture.starts_with("dynamic-"))
		return eu4culture;

	for (const auto& cultureMappingRule: cultureMapRules)
	{
		const auto& possibleMatch =
			 cultureMappingRule.cultureNonRegionalNonReligiousMatch(clayManager, cultureLoader, religionLoader, eu4culture, eu4religion, v3state, v3ownerTag);
		if (possibleMatch)
			return *possibleMatch;
	}
	return std::nullopt;
}

void mappers::CultureMapper::expandCulturalMappings(const V3::ClayManager& clayManager,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader)
{
	// We'll simply iterate over all known eu4 cultures, see what maps, and then add literal mappings for what doesn't.

	for (const auto& cultureGroup: cultureLoader.getCultureGroupsMap() | std::views::values)
		for (const auto& cultureName: cultureGroup.getCultures() | std::views::keys)
			if (!cultureMatch(clayManager, cultureLoader, religionLoader, cultureName, "", "", "", true))
				unmappedCultures.emplace(cultureName);

	for (const auto& culture: unmappedCultures)
	{
		CultureMappingRule newRule;
		newRule.loadMappingRules("vic3 = " + culture + " eu4 = " + culture);
		cultureMapRules.push_back(newRule);
	}

	Log(LogLevel::Info) << "<> Additional " << unmappedCultures.size() << " cultures imported.";
}
