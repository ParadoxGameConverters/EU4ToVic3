#ifndef BUILDING_MAPPER_H
#define BUILDING_MAPPER_H
#include "Parser.h"
#include <map>

namespace mappers
{
class BuildingMapper: commonItems::parser
{
  public:
	BuildingMapper() = default;
	void loadBuildingMappings(const std::string& filePath);

	[[nodiscard]] std::set<std::string> getVic3Buildings(const std::string& eu4Building) const;

  private:
	void registerKeys();

	std::map<std::string, std::set<std::string>> mappings;
};
} // namespace mappers

#endif // BUILDING_MAPPER_H