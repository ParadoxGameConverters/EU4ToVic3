#include "OwnershipSector.h"

V3::OwnershipSector::OwnershipSector(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::OwnershipSector::registerKeys()
{
	ownershipsParser.registerKeyword("local", [this](const std::string& type, std::istream& theStream) {
		OwnershipData ownership;
		ownership.type = type;
		ownership.weight = commonItems::getDouble(theStream);
		ownerships.emplace(type, ownership);
	});
	ownershipsParser.registerKeyword("national", [this](const std::string& type, std::istream& theStream) {
		OwnershipData ownership;
		ownership.type = type;
		ownership.weight = commonItems::getDouble(theStream);
		ownerships.emplace(type, ownership);
	});
	ownershipsParser.registerKeyword("national_service", [this](const std::string& type, std::istream& theStream) {
		OwnershipData ownership;
		ownership.type = type;
		ownership.weight = commonItems::getDouble(theStream);
		ownerships.emplace(type, ownership);
	});
	ownershipsParser.registerRegex(commonItems::catchallRegex, [this](const std::string& ownershipBuildingName, std::istream& theStream) {
		OwnershipData ownership = Ownership(theStream).getOwnership();
		ownership.type = ownershipBuildingName;
		ownerships.emplace(ownershipBuildingName, ownership);
	});

	registerKeyword("ownership", [this](std::istream& theStream) {
		ownershipsParser.parseStream(theStream);
	});
	registerRegex(commonItems::catchallRegex, [this](const std::string& buildingName, std::istream& theStream) {
		buildings.emplace(buildingName);
	});
}