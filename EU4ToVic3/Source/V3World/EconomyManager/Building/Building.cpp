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
	registerKeyword("buildable", [this](std::istream& theStream) {
		buildable = commonItems::getString(theStream) == "yes";
	});
	registerKeyword("has_max_level", [this](std::istream& theStream) {
		governmentCapped = commonItems::getString(theStream) == "yes";
	});
	registerKeyword("unique", [this](std::istream& theStream) {
		unique = commonItems::getString(theStream) == "yes";
	});
	registerKeyword("statue", [this](std::istream& theStream) {
		statue = commonItems::getString(theStream) == "yes";
	});
	registerKeyword("production_method_groups", [this](std::istream& theStream) {
		for (const auto& PMGroup: commonItems::getStrings(theStream))
			PMGroups.emplace(PMGroup);
	});
	registerKeyword("unlocking_technologies", [this](std::istream& theStream) {
		for (const auto& tech: commonItems::getStrings(theStream))
			unlockingTechs.emplace(tech);
	});
	registerKeyword("required_construction", [this, &costTiers](std::istream& theStream) {
		if (const auto costTier = commonItems::getString(theStream); costTiers.contains(costTier))
		{
			constructionCost = costTiers.at(costTier);
			if (costTier == "construction_cost_monument")
			{
				monumental = true;
			}
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

void V3::Building::addInvestor(const int theLevel, const std::string& type, const std::string& theState, const std::string& theNation)
{
	shareholders.push_back({type, theNation, theState, theLevel});
}

void V3::Building::addShareholderLevels(const int theLevels, const std::string& type)
{
	for (auto it = shareholders.begin(); it != shareholders.end(); ++it)
	{
		if (it->type == type)
		{
			it->level += theLevels;
			return;
		}
	}

	Log(LogLevel::Warning) << "Shareholder " << type << " not found in building.";
}
