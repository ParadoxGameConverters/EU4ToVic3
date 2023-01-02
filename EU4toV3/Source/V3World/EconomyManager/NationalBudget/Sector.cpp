#include "Sector.h"
#include "Loaders/NationalBudgetLoader/SectorLoader.h"

void V3::Sector::loadSector(std::shared_ptr<SectorLoader> sectorRules, std::shared_ptr<Country> country)
{
	name = sectorRules->getName();
	buildings = sectorRules->getBuildings();
	weight = sectorRules->calculateWeight(*country);
}

bool V3::Sector::hasBuilding(const std::string& building) const
{
	return buildings.contains(building);
}
