#ifndef BUILDING_MAPPING_H
#define BUILDING_MAPPING_H
#include "Parser.h"

namespace mappers
{
class BuildingMapping: commonItems::parser
{
  public:
	explicit BuildingMapping(std::istream& theStream);


	[[nodiscard]] const auto& getEU4Building() const { return eu4Building; }
	[[nodiscard]] const auto& getVic3Buildings() const { return vic3Buildings; }

  private:
	void registerKeys();

	std::string eu4Building;
	std::set<std::string> vic3Buildings;
};
} // namespace mappers

#endif // BUILDING_MAPPING_H