#ifndef BUILDINGS_H
#define BUILDINGS_H
#include "Building.h"
#include "Parser.h"
#include <map>
#include <optional>

namespace mappers
{
class Buildings: commonItems::parser
{
  public:
	Buildings();
	explicit Buildings(std::istream& theStream);

	[[nodiscard]] std::optional<Building> getBuilding(const std::string& buildingName) const;

  private:
	void registerKeys();

	std::map<std::string, Building> buildings;
};
} // namespace mappers

#endif // BUILDINGS_H