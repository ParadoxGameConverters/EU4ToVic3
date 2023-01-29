#include "ProductionMethodMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "ProductionMethodMapping.h"

void mappers::ProductionMethodMapper::loadRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Loading PM rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> Loaded PM rules for" << buildingToRules.size() << " buildings.";
}


void mappers::ProductionMethodMapper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& buildingName, std::istream& theStream) {
		ProductionMethodMapping PMMapping;
		PMMapping.loadRules(theStream);
		buildingToRules.emplace(buildingName, PMMapping.getRules());
	});
}

void mappers::ProductionMethodMapper::applyRules(const std::shared_ptr<V3::Country>& country,
	 const std::map<std::string, V3::ProductionMethod>& PMs,
	 const std::map<std::string, V3::ProductionMethodGroup>& PMGroups)
{
	// For all building types in a country gather all instances off the building in a single vector
	// Apply Subset-sum problem to vector to find out remaining buildings (if percent < 1.0) NOTE(Gawquon): I have implemented this, all ready to transfer in
	// For every PM in the given PMs, check the group and walk until PM encountered or tech wall hit, this is our actual PM
	// For each building in the vector, add the PMs to it
}