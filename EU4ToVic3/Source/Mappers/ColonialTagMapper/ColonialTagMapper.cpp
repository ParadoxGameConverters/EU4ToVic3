#include "ColonialTagMapper.h"
#include "ClayManager/ClayManager.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "PoliticalManager/Country/Country.h"
#include <ranges>

void mappers::ColonialTagMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Loading colonial tag mapping rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << colonialTagMappings.size() << " colonial tag rules loaded.";
}

void mappers::ColonialTagMapper::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		const auto mapping = ColonialTagMapping(theStream);
		knownColonialTags.emplace(mapping.getTag());
		colonialTagMappings.emplace_back(mapping);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::ColonialTagMapper::matchColonialTag(const V3::Country& country,
	 const ColonialRegionMapper& colonialRegionMapper,
	 const V3::ClayManager& clayManager) const
{
	// Don't replace tags which are alerady defined as colonial tags!
	// We got those through name or tag matches so leave them as they are!
	if (knownColonialTags.contains(country.getTag()))
		return std::nullopt;

	for (const auto& mapping: colonialTagMappings)
		if (const auto& match = mapping.matchColonialTag(country, colonialRegionMapper, clayManager); match)
			return match;
	return std::nullopt;
}
