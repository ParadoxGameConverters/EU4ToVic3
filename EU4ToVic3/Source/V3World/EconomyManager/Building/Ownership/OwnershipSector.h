#ifndef V3_OWNERSHIP_SECTOR_H
#define V3_OWNERSHIP_SECTOR_H
#include "Ownership.h"
#include "Parser.h"

namespace V3
{
// An ownership sector is a group of vic3 buildings with similar investment characteristics
class OwnershipSector: commonItems::parser
{
  public:
	OwnershipSector(std::istream& theStream);
	void setName(const std::string& theName) { name = theName; }

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getOwnerships() const { return ownerships; }
	[[nodiscard]] const auto& getBuildings() const { return buildings; }

  private:
	void registerKeys();
	commonItems::parser ownershipsParser;

	std::string name;
	std::map<std::string, OwnershipData> ownerships;
	std::set<std::string> buildings;
};
} // namespace V3

#endif // V3_OWNERSHIP_SECTOR_H