#ifndef VANILLA_BUILDING_LOADER_H
#define VANILLA_BUILDING_LOADER_H
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"

namespace V3
{
class VanillaBuildingLoader: commonItems::parser
{
  public:
	VanillaBuildingLoader() = default;
	void loadVanillaBuildings(const commonItems::ModFilesystem& modFS);

	[[nodiscard]] const auto& getBuildingElements() const { return buildingElements; }

  private:
	void registerKeys();
	parser stateParser;

	std::map<std::string, std::map<std::string, std::vector<std::string>>> buildingElements;
};
} // namespace V3
#endif // VANILLA_BUILDING_LOADER_H