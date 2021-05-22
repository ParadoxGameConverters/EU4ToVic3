#include "SuperGroupMapper.h"
#include "ParserHelpers.h"
#include "Log.h"
#include "SuperRegion.h"
#include <cmath>

void mappers::SuperGroupMapper::init()
{
	LOG(LogLevel::Info) << "Parsing starting supergroups.";

	registerRegex("[a-z_]+", [this](const std::string& superGroupName, std::istream& theStream)
		{
			const SuperRegion newSuperRegionBlock(theStream);
			const auto& srlist = newSuperRegionBlock.getSuperRegionNames();
			superGroups[superGroupName].insert(srlist.begin(), srlist.end());
			const auto& amap = newSuperRegionBlock.getSuperRegionAssimilationMap();
			assimilationMap.insert(amap.begin(), amap.end());
		});
	registerRegex("[a-zA-Z0-9\\_]+", commonItems::ignoreItem);

	parseFile("configurables/world_supergroups.txt");
	clearRegisteredKeywords();
}

std::optional<std::string> mappers::SuperGroupMapper::getGroupForSuperRegion(const std::string& superRegionName) const
{
	for (const auto& superGroup: superGroups) if (superGroup.second.count(superRegionName)) return superGroup.first;
	return std::nullopt;
}

double mappers::SuperGroupMapper::getAssimilationFactor(const std::string& superRegionName) const
{
	auto rate = 1;
	const auto& srIter = assimilationMap.find(superRegionName);
	if (srIter != assimilationMap.end()) rate = srIter->second;

	const auto factor = (1.0 - pow(0.75, rate)) / 100;
	return factor;
}