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
	[[nodiscard]] auto getAssimilationFactor() const { return assimilation; }
	[[nodiscard]] const auto& getSuperGroup() const { return superGroup; }

	[[nodiscard]] bool superRegionContainsProvince(int provinceID) const;
	[[nodiscard]] bool superRegionContainsNativeCulture(const std::string& culture) const;

	void setAssimilationFactor(double factor) { assimilation = factor; }
	void setSuperGroup(const auto& sGroup) { superGroup = sGroup; }
	void linkRegion(const std::pair<std::string, std::shared_ptr<Region>>& theRegion) { regions.at(theRegion.first) = theRegion.second; }
	void registerNativeCulture(const std::string& culture) { nativeCultures.insert(culture); }

  private:
	std::map<std::string, std::shared_ptr<Region>> regions;
	std::set<std::string> nativeCultures;

	double assimilation = 0;
	std::string superGroup;
};
} // namespace EU4

#endif // EU4_SUPERREGION_H