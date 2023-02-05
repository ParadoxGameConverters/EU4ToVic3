#ifndef BUILDING_SCRIPT_VALUES_LOADER_H
#define BUILDING_SCRIPT_VALUES_LOADER_H
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"

namespace V3
{
class BuildingScriptValuesLoader: commonItems::parser
{
  public:
	BuildingScriptValuesLoader() = default;

	void loadBuildingScriptValues(const commonItems::ModFilesystem& modFS);
	[[nodiscard]] const auto& getBuildingCostConstants() const { return buildingCostConstants; }

  private:
	void registerKeys();

	std::map<std::string, int> buildingCostConstants;
};
} // namespace V3
#endif // BUILDING_SCRIPT_VALUES_LOADER_H