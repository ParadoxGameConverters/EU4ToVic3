#include "ProductionMethodMapper.h"
#include "ClayManager/State/SubState.h"
#include "CommonRegexes.h"
#include "EconomyManager/Building/Building.h"
#include "EconomyManager/Building/ProductionMethods/ProductionMethod.h"
#include "EconomyManager/Building/ProductionMethods/ProductionMethodGroup.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "PoliticalManager/Country/Country.h"
#include "ProductionMethodMapping.h"
#include <ranges>

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
	 const std::map<std::string, V3::ProductionMethodGroup>& PMGroups,
	 const std::map<std::string, V3::Building> buildings) const
{
	// For all building types in a country gather all instances of the building in a single vector
	for (const auto& buildingType: buildings | std::views::keys)
	{
		std::vector<std::shared_ptr<V3::Building>> builtBuildings; // All instances of the current building type in the country

		// Make own function V
		const auto& findBuilding = [buildingType](const std::shared_ptr<V3::Building>& subStateBuilding) {
			return subStateBuilding->getName() == buildingType;
		};

		for (const auto& subState: country->getSubStates())
		{
			if (const auto& builtBuilding = std::ranges::find_if(subState->getBuildings(), findBuilding); builtBuilding != subState->getBuildings().end())
				builtBuildings.push_back(*builtBuilding);
		}
		// Make own function ^
	}

	// Apply Subset-sum problem to vector to find out remaining buildings (if percent < 1.0) NOTE(Gawquon): I have implemented this, all ready to transfer in
	// For every PM in the given PMs, check the group and walk until PM encountered or tech wall hit, this is our actual PM

	// For each building in the vector, add the PMs to it
	std::vector<std::shared_ptr<V3::Building>> selectedBuildings; // Probably set this before
	std::string selectedPM;													  // Is 100% set before

	for (const auto& selectedBuilding: selectedBuildings)
		selectedBuilding->addPM(selectedPM);
}