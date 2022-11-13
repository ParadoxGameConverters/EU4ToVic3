#ifndef SUPERREGION_LOADER_H
#define SUPERREGION_LOADER_H
#include "V3SuperRegion.h"

namespace V3
{
class SuperRegionLoader: commonItems::parser
{
  public:
	SuperRegionLoader() = default;

	void loadSuperRegions(const std::string& v3Path);
	[[nodiscard]] const auto& getSuperRegions() const { return superRegions; }

  private:
	std::map<std::string, std::shared_ptr<SuperRegion>> superRegions;
};
} // namespace V3
#endif // SUPERREGION_LOADER_H