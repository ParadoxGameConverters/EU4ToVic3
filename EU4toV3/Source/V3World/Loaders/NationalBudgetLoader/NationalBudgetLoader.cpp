#include "NationalBudgetLoader.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void V3::NationalBudgetLoader::loadNationalBudget(const std::string& filePath)
{
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << sectorBlueprints.size() << " sectors of industry loaded.";
}

void V3::NationalBudgetLoader::buildBuildingSectorMap()
{
	// TODO(Gawquon): construct buildingSectorMap from sectorBlueprints. building name -> sector name of the sector building is in
}

void V3::NationalBudgetLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& sectorName, std::istream& theStream) {
		SectorLoader newSectorBlueprint;
		newSectorBlueprint.loadSector(theStream);
		newSectorBlueprint.setName(sectorName);
		sectorBlueprints.push_back(newSectorBlueprint);
	});
}
