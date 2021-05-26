#ifndef EU4_SUPERREGION_H
#define EU4_SUPERREGION_H
#include "Region.h"

namespace EU4
{
class SuperRegion
{
  public:
	SuperRegion() = default;
	explicit SuperRegion(const std::vector<std::string>& theRegions);

	[[nodiscard]] const auto& getRegions() const { return regions; }
	[[nodiscard]] bool superRegionContainsProvince(int provinceID) const;

	void linkRegion(const std::pair<std::string, std::shared_ptr<Region>>& theRegion) { regions.at(theRegion.first) = theRegion.second; }

  private:
	std::map<std::string, std::shared_ptr<Region>> regions;
};
} // namespace EU4

#endif // EU4_SUPERREGION_H