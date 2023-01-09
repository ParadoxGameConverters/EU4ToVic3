#include "Sector.h"
#include "Loaders/NationalBudgetLoader/SectorLoader.h"
#include "PoliticalManager/Country/Country.h"
#include <cmath>

V3::Sector::Sector(const SectorLoader& sectorRules, const Country& country): name(sectorRules.getName()), buildings(sectorRules.getBuildings())
{
	weight = sectorRules.calculateWeight(country);
}

void V3::Sector::calculateBudget(const double totalWeight, const int countryCPBudget)
{
	CPBudget = static_cast<int>(std::round(countryCPBudget * weight / totalWeight));
}

bool V3::Sector::hasBuilding(const std::string& building) const
{
	return buildings.contains(building);
}
