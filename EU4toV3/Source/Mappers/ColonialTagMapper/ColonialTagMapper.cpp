#include "ColonialTagMapper.h"
#include "ClayManager/ClayManager.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include <ranges>

void mappers::ColonialTagMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Loading colonial tag mapping rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << colonialTagMappings.size() << " rules loaded.";
}

void mappers::ColonialTagMapper::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		colonialTagMappings.emplace_back(ColonialTagMapping(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::ColonialTagMapper::matchColonialTag(const V3::Country& country,
	 const ColonialRegionMapper& colonialRegionMapper,
	 const V3::ClayManager& clayManager) const
{
	for (const auto& mapping: colonialTagMappings)
		if (const auto& match = mapping.matchColonialTag(country, colonialRegionMapper, clayManager); match)
			return match;
	return std::nullopt;
}
