#ifndef V3_OWNERSHIP_LOADER_H
#define V3_OWNERSHIP_LOADER_H
#include "EconomyManager/Building/Ownership/OwnershipSector.h"
#include "Parser.h"

namespace V3
{
class OwnershipLoader: commonItems::parser
{
  public:
	OwnershipLoader() = default;
	void loadOwnership(const std::string& filePath);

	[[nodiscard]] const std::map<std::string, V3::OwnershipData>& getOwnershipsFromBuilding(const std::string& building) const;
	[[nodiscard]] const std::map<std::string, std::string>& getBuildingSectorMap() const { return buildingSectorMap; }

  private:
	void registerKeys();

	std::map<std::string, std::string> buildingSectorMap; // Building Name -> OwnershipSector Name
	std::map<std::string, OwnershipSector> sectorMap;		// OwnershipSector Name -> OwnershipSector
};
} // namespace V3

#endif // V3_OWNERSHIP_LOADER_H