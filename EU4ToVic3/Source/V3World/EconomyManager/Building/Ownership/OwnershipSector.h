#ifndef V3_OWNERSHIP_SECTOR_H
#define V3_OWNERSHIP_SECTOR_H
#include "Ownership.h"
#include "Parser.h"

namespace V3
{
// An ownership sector is a group of vic3 buildings with similar investment characteristics
class OwnershipSector : commonItems::parser
{
  public:
	OwnershipSector(std::istream& theStream);

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getOwnerships() const { return ownerships; }
	[[nodiscard]] bool contains(const std::string& building) const { return buildings.contains(building); }

  private:
	void registerKeys();

	std::string name;
	std::vector<Ownership> ownerships;
	std::set<std::string> buildings;
};
} // namespace V3

#endif // V3_OWNERSHIP_SECTOR_H