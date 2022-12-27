#include "Building.h"

#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void V3::Building::loadBuilding(std::istream& theStream, const std::map<std::string, int>& costTiers)
{
	registerKeys(costTiers);
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::Building::registerKeys(const std::map<std::string, int>& costTiers)
{
	registerKeyword("building_group", [this](std::istream& theStream) {
		buildingGroup = commonItems::getString(theStream);
	});
	registerKeyword("production_method_groups", [this](std::istream& theStream) {
		PMGroups = commonItems::getStrings(theStream);
	});
	registerKeyword("unlocking_technologies", [this](std::istream& theStream) {
		unlockingTechs = commonItems::getStrings(theStream);
	});
	registerKeyword("required_construction", [this, &costTiers](std::istream& theStream) {
		if (const std::string costTier = commonItems::getString(theStream); costTiers.contains(costTier))
		{
			constructionCost = costTiers.at(costTier);
		}
		else
		{
			// Maybe its a literal instead of a constant variable?
			try
			{
				constructionCost = std::stoi(costTier);
			}
			catch (const std::exception& e)
			{
				Log(LogLevel::Error) << "Failed to understand building cost " << costTier << ": " << e.what();
			}
		}
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
