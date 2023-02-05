#include "ColonialRegionMapper.h"
#include "ClayManager/ClayManager.h"
#include "CommonRegexes.h"
#include "Log.h"
#include <ranges>

void mappers::ColonialRegionMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Loading colonial region definitions.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << colonialRegions.size() << " regions loaded.";
}

void mappers::ColonialRegionMapper::loadMappingRules(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ColonialRegionMapper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& regionName, std::istream& theStream) {
		colonialRegions.emplace(regionName, ColonialRegionMapping(theStream));
	});
}

std::optional<mappers::ColonialRegionMapping> mappers::ColonialRegionMapper::getColonyForState(const std::string& v3State,
	 const V3::ClayManager& clayManager) const
{
	for (const auto& colony: colonialRegions | std::views::values)
	{
		if (colony.getRegions().contains(v3State))
			return colony;
		for (const auto& region: colony.getRegions())
			if (clayManager.stateIsInRegion(v3State, region))
				return colony;
	}
	return std::nullopt;
}

std::optional<std::string> mappers::ColonialRegionMapper::getColonyNameForState(const std::string& v3State, const V3::ClayManager& clayManager) const
{
	for (const auto& [colonyName, colony]: colonialRegions)
	{
		if (colony.getRegions().contains(v3State))
			return colonyName;
		for (const auto& region: colony.getRegions())
			if (clayManager.stateIsInRegion(v3State, region))
				return colonyName;
	}
	return std::nullopt;
}
