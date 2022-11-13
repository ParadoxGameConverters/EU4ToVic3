#ifndef CLAY_MANAGER_H
#define CLAY_MANAGER_H
#include "StateLoader/State.h"
#include "SuperRegionLoader/V3SuperRegion.h"
#include <map>
#include <string>

namespace V3
{
class ClayManager
{
  public:
	ClayManager() = default;

	void initializeVanillaStates(const std::string& v3Path);
	void loadTerrainsIntoProvinces(const std::string& v3Path);
	void initializeSuperRegions(const std::string& v3Path);
	void loadStatesIntoSuperRegions();

	[[nodiscard]] const auto& getStates() const { return states; }
	[[nodiscard]] const auto& getSuperRegions() const { return superRegions; }

  private:
	std::map<std::string, std::shared_ptr<State>> states;
	std::map<std::string, std::shared_ptr<SuperRegion>> superRegions;
};
} // namespace V3
#endif // CLAY_MANAGER_H