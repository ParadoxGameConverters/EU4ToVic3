#include "CultureMapper.h"
#include "CommonRegexes.h"
#include "CultureMappingRule.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "Regions/Regions.h"

mappers::CultureMapper::CultureMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::CultureMapper::loadFile(const std::string& fileName)
{
	registerKeys();
	parseFile(fileName);
	clearRegisteredKeywords();
}

void mappers::CultureMapper::registerKeys()
{
	registerKeyword("link", [this](const std::string& unused, std::istream& theStream) {
		const CultureMappingRule rule(theStream);
		if (rule.hasDestinationCulture() && rule.hasCultures())
			cultureMapRules.emplace_back(rule);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::CultureMapper::cultureMatch(const EU4::Regions& eu4Regions,
	 const std::string& eu4culture,
	 const std::string& eu4religion,
	 int eu4Province,
	 const std::string& eu4ownerTag) const
{
	// Don't match dynamics.
	if (eu4culture.starts_with("dynamic-"))
		return eu4culture;

	for (const auto& cultureMappingRule: cultureMapRules)
	{
		const auto& possibleMatch = cultureMappingRule.cultureMatch(eu4Regions, eu4culture, eu4religion, eu4Province, eu4ownerTag);
		if (possibleMatch)
			return *possibleMatch;
	}
	return std::nullopt;
}

std::optional<std::string> mappers::CultureMapper::cultureRegionalMatch(const EU4::Regions& eu4Regions,
	 const std::string& eu4culture,
	 const std::string& eu4religion,
	 int eu4Province,
	 const std::string& eu4ownerTag) const
{
	// Don't match dynamics for non-slave pops, they are same in eu4 and vic2.
	if (eu4culture.starts_with("dynamic-"))
		return eu4culture;

	for (const auto& cultureMappingRule: cultureMapRules)
	{
		const auto& possibleMatch = cultureMappingRule.cultureRegionalMatch(eu4Regions, eu4culture, eu4religion, eu4Province, eu4ownerTag);
		if (possibleMatch)
			return *possibleMatch;
	}
	return std::nullopt;
}

std::optional<std::string> mappers::CultureMapper::cultureNonRegionalNonReligiousMatch(const EU4::Regions& eu4Regions,
	 const std::string& eu4culture,
	 const std::string& eu4religion,
	 int eu4Province,
	 const std::string& eu4ownerTag) const
{
	// Don't match dynamics.
	if (eu4culture.starts_with("dynamic-"))
		return eu4culture;

	for (const auto& cultureMappingRule: cultureMapRules)
	{
		const auto& possibleMatch = cultureMappingRule.cultureNonRegionalNonReligiousMatch(eu4Regions, eu4culture, eu4religion, eu4Province, eu4ownerTag);
		if (possibleMatch)
			return *possibleMatch;
	}
	return std::nullopt;
}
