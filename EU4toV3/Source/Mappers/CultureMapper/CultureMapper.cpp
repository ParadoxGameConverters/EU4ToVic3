#include "CultureMapper.h"
#include "CultureMappingRule.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "Regions/Regions.h"
#include "CommonRegexes.h"

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
	for (const auto& cultureMappingRule: cultureMapRules)
	{
		const auto& possibleMatch = cultureMappingRule.cultureNonRegionalNonReligiousMatch(eu4Regions, eu4culture, eu4religion, eu4Province, eu4ownerTag);
		if (possibleMatch)
			return *possibleMatch;
	}
	return std::nullopt;
}
