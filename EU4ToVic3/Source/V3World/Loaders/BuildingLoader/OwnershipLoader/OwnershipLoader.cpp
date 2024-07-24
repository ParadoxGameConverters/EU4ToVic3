#include "OwnershipLoader.h"

const std::vector<V3::Ownership>& V3::OwnershipLoader::getOwnershipFromBuilding(const std::string& building) const
{
	return sectorMap.at(buildingSectorMap.at(building)).getOwnerships();
}

void V3::OwnershipLoader::registerKeys()
{
}
