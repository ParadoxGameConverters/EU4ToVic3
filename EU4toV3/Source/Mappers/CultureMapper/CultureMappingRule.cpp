#include "CultureMappingRule.h"
#include "CommonRegexes.h"
#include "Configuration.h"
#include "ParserHelpers.h"
#include "Regions/Regions.h"

mappers::CultureMappingRule::CultureMappingRule(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();

	if (theConfiguration.isHpmEnabled() && !hpmCulture.empty())
		destinationCulture = hpmCulture;
}

void mappers::CultureMappingRule::registerKeys()
{
	registerKeyword("vic2", [this](const std::string& unused, std::istream& theStream) {
		destinationCulture = commonItems::singleString(theStream).getString();
	});
	registerKeyword("hpm", [this](const std::string& unused, std::istream& theStream) {
		hpmCulture = commonItems::singleString(theStream).getString();
	});
	registerKeyword("region", [this](const std::string& unused, std::istream& theStream) {
		regions.insert(commonItems::singleString(theStream).getString());
	});
	registerKeyword("religion", [this](const std::string& unused, std::istream& theStream) {
		religions.insert(commonItems::singleString(theStream).getString());
	});
	registerKeyword("owner", [this](const std::string& unused, std::istream& theStream) {
		owners.insert(commonItems::singleString(theStream).getString());
	});
	registerKeyword("provinceid", [this](const std::string& unused, std::istream& theStream) {
		provinces.insert(commonItems::singleInt(theStream).getInt());
	});
	registerKeyword("eu4", [this](const std::string& unused, std::istream& theStream) {
		cultures.insert(commonItems::singleString(theStream).getString());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::CultureMappingRule::cultureMatch(const EU4::Regions& eu4Regions,
	 const std::string& eu4culture,
	 const std::string& eu4religion,
	 int eu4Province,
	 const std::string& eu4ownerTag) const
{
	// We need at least a viable EU4culture.
	if (eu4culture.empty())
		return std::nullopt;

	if (!cultures.count(eu4culture))
		return std::nullopt;

	if (!eu4ownerTag.empty() && !owners.empty())
		if (!owners.count(eu4ownerTag))
			return std::nullopt;

	if (!eu4religion.empty() && !religions.empty())
		if (!religions.count(eu4religion))
			return std::nullopt;

	// This is a provinces check, not regions.
	if (eu4Province && !provinces.empty())
		if (!provinces.count(eu4Province))
			return std::nullopt;

	// This is a regions check, that checks if a provided province is within that region.
	if (eu4Province && !regions.empty())
	{
		auto regionMatch = false;
		for (const auto& region: regions)
		{
			if (!eu4Regions.regionIsValid(region))
			{
				// Regions change between versions so don't react to invalid region name.
				continue;
			}
			if (eu4Regions.provinceInRegion(eu4Province, region))
				regionMatch = true;
		}
		if (!regionMatch)
			return std::nullopt;
	}
	return destinationCulture;
}

std::optional<std::string> mappers::CultureMappingRule::cultureRegionalMatch(const EU4::Regions& eu4Regions,
	 const std::string& eu4culture,
	 const std::string& eu4religion,
	 int eu4Province,
	 const std::string& eu4ownerTag) const
{
	// This is a regional match. We need a mapping within the given region, so if the
	// mapping rule has no regional qualifiers it needs to fail.
	if (regions.empty())
		return std::nullopt;

	// Otherwise, as usual.
	return cultureMatch(eu4Regions, eu4culture, eu4religion, eu4Province, eu4ownerTag);
}

std::optional<std::string> mappers::CultureMappingRule::cultureNonRegionalNonReligiousMatch(const EU4::Regions& eu4Regions,
	 const std::string& eu4culture,
	 const std::string& eu4religion,
	 int eu4Province,
	 const std::string& eu4ownerTag) const
{
	// This is a non regional non religious match. We need a mapping without any region/religion, so if the
	// mapping rule has any regional/religious qualifiers it needs to fail.
	if (!regions.empty())
		return std::nullopt;
	if (!religions.empty())
		return std::nullopt;

	// Otherwise, as usual.
	return cultureMatch(eu4Regions, eu4culture, eu4religion, eu4Province, eu4ownerTag);
}
