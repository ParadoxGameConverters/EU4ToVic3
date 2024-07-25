#include "OwnershipLoader.h"

void V3::OwnershipLoader::loadOwnership(const std::string& filePath)
{
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << sectorMap.size() << " sectors of investors loaded.";
}

const std::map<std::string, V3::OwnershipData>& V3::OwnershipLoader::getOwnershipsFromBuilding(const std::string& building) const
{
	return sectorMap.at(buildingSectorMap.at(building)).getOwnerships();
}

void V3::OwnershipLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& ownershipSectorName, std::istream& theStream) {
		OwnershipSector newOwnershipSector(theStream);
		newOwnershipSector.setName(ownershipSectorName);
		sectorMap.emplace(ownershipSectorName, newOwnershipSector);
		for (const auto& building: newOwnershipSector.getBuildings())
		{
			buildingSectorMap.emplace(building, ownershipSectorName);
		}
	});
}
