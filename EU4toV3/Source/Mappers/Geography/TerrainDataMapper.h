#ifndef TERRAIN_DATA_MAPPER_H
#define TERRAIN_DATA_MAPPER_H
#include "Parser.h"

namespace mappers
{
class TerrainDataMapper: commonItems::parser
{
  public:
	TerrainDataMapper();
	explicit TerrainDataMapper(std::istream& theStream);

	[[nodiscard]] std::optional<std::string> getTerrainForID(int provinceID) const;

  private:
	void registerKeys();

	std::map<int, std::string> terrainMap;
};
} // namespace mappers

#endif // TERRAIN_DATA_MAPPER_H
