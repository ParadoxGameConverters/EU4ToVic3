#include "CountryTierMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void mappers::CountryTierMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Parsing country tier mapping rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << vanillaMappings.size() + vnMappings.size() << " rules loaded.";
}

void mappers::CountryTierMapper::registerKeys()
{
	registerKeyword("vanilla", [this](std::istream& theStream) {
		vanillaGame = true;
		gameTypeParser.parseStream(theStream);
	});
	registerKeyword("vn", [this](std::istream& theStream) {
		vanillaGame = false;
		gameTypeParser.parseStream(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	gameTypeParser.registerRegex(commonItems::catchallRegex, [this](const std::string& tierName, std::istream& theStream) {
		const auto mapping = CountryTierMapping(theStream, tierName);
		if (vanillaGame)
			vanillaMappings.emplace_back(mapping);
		else
			vnMappings.emplace_back(mapping);
	});
}

std::optional<std::string> mappers::CountryTierMapper::matchCountryTier(const int rank, const size_t size, const bool vn) const
{
	if (!vn)
	{
		for (const auto& mapping: vanillaMappings)
			if (const auto& match = mapping.matchTier(rank, size); match)
				return *match;
	}
	else
	{
		for (const auto& mapping: vnMappings)
			if (const auto& match = mapping.matchTier(rank, size); match)
				return *match;
	}
	return std::nullopt;
}
