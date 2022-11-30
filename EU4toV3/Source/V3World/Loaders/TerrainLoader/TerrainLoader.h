#ifndef TERRAIN_LOADER_H
#define TERRAIN_LOADER_H
#include "Parser.h"
#include <map>
#include <string>

namespace V3
{
class TerrainLoader: commonItems::parser
{
  public:
	TerrainLoader() = default;

	void loadTerrains(const std::string& v3Path);
	[[nodiscard]] const auto& getTerrains() const { return provincialTerrains; }

  private:
	void registerKeys();

	std::map<std::string, std::string> provincialTerrains;
};
} // namespace V3
#endif // TERRAIN_LOADER_H