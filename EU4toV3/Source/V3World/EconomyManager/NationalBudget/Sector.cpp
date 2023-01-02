#include "Sector.h"
#include "Loaders/NationalBudgetLoader/SectorLoader.h"
#include "PoliticalManager/Country/Country.h"

V3::Sector::Sector(const std::shared_ptr<SectorLoader>& sectorRules, const std::shared_ptr<Country>& country)
{
	name = sectorRules->getName();
	buildings = sectorRules->getBuildings();
	weight = sectorRules->calculateWeight(*country);
}

void V3::Sector::calculateBudget(const double totalWeight, const std::shared_ptr<Country>& country)
{
	CPBudget = static_cast<int>(std::round(country->getCPBudget() * weight / totalWeight));
}

bool V3::Sector::hasBuilding(const std::string& building) const
{
	return buildings.contains(building);
}
