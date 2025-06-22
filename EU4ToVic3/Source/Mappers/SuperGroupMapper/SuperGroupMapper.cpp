#include "SuperGroupMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "SuperRegionParser.h"
#include <cmath>
#include <filesystem>

void mappers::SuperGroupMapper::loadSuperGroups()
{
	Log(LogLevel::Info) << "-> Parsing starting supergroups.";
	registerKeys();
	parseFile("configurables/world_supergroups.txt");
	clearRegisteredKeywords();
}

void mappers::SuperGroupMapper::loadSuperGroups(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::SuperGroupMapper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& superGroupName, std::istream& theStream) {
		const SuperRegionParser newSuperRegionBlock(theStream);
		superGroups.emplace(superGroupName, newSuperRegionBlock.getSuperRegionNames());

		const auto& aMap = newSuperRegionBlock.getSuperRegionAssimilationMap();
		assimilationMap.insert(aMap.begin(), aMap.end());
	});
}

std::optional<std::string> mappers::SuperGroupMapper::getGroupForSuperRegion(const std::string& superRegionName) const
{
	for (const auto& [superGroupName, superGroup]: superGroups)
		if (superGroup.contains(superRegionName))
			return superGroupName;
	return std::nullopt;
}

double mappers::SuperGroupMapper::getAssimilationFactor(const std::string& superRegionName) const
{
	auto rate = 1;
	const auto& srIter = assimilationMap.find(superRegionName);
	if (srIter != assimilationMap.end())
		rate = srIter->second;

	const auto factor = (1.0 - pow(0.75, rate)) / 100;
	return factor;
}