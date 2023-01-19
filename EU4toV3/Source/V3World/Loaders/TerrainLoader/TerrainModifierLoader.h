#ifndef TERRAIN_MODIFIER_LOADER_H
#define TERRAIN_MODIFIER_LOADER_H
#include "Parser.h"
#include <map>
#include <string>

namespace V3
{
class TerrainModifierLoader: commonItems::parser
{
  public:
	TerrainModifierLoader() = default;

	void loadTerrainModifiers(const std::string& filePath);
	void loadTerrainModifiers(std::istream& theStream);
	[[nodiscard]] const auto& getTerrainStateModifiers() const { return stateTerrainModifiers; }
	[[nodiscard]] const auto& getTerrainBuildingModifiers() const { return buildingTerrainModifiers; }

  private:
	void registerKeys();

	std::map<std::string, double> stateTerrainModifiers;
	std::map<std::string, std::map<std::string, double>> buildingTerrainModifiers;
};
} // namespace V3
#endif // TERRAIN_MODIFIER_LOADER_H