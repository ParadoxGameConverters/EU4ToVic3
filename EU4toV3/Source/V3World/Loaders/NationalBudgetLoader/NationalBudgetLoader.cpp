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

void V3::NationalBudgetLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& sectorName, std::istream& theStream) {
		const auto newSectorBlueprint = std::make_shared<SectorLoader>();
		newSectorBlueprint->loadSector(theStream);
		newSectorBlueprint->setName(sectorName);
		sectorBlueprints.push_back(newSectorBlueprint);
	});
}
