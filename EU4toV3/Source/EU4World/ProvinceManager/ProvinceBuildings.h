#ifndef PROVINCE_BUILDINGS_H
#define PROVINCE_BUILDINGS_H
#include "Parser.h"
#include <set>

namespace EU4
{
class ProvinceBuildings: commonItems::parser
{
  public:
	ProvinceBuildings() = default;
	explicit ProvinceBuildings(std::istream& theStream);

	[[nodiscard]] auto hasBuilding(const std::string& building) const { return buildings.count(building) > 0; }
	[[nodiscard]] const auto& getBuildings() const { return buildings; }

  private:
	void registerKeys();

	std::set<std::string> buildings;
};
} // namespace EU4

#endif // PROVINCE_BUILDINGS_H