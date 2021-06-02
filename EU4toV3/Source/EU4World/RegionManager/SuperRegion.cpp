#include "SuperRegion.h"
#include <ranges>

EU4::SuperRegion::SuperRegion(const std::vector<std::string>& theRegions)
{
	for (const auto& region: theRegions)
		regions.emplace(region, nullptr);
}

bool EU4::SuperRegion::superRegionContainsProvince(const int province) const
{
	for (const auto& region: regions | std::views::values)
		if (region->regionContainsProvince(province))
			return true;
	return false;
}

bool EU4::SuperRegion::superRegionContainsNativeCulture(const std::string& culture) const
{
	return nativeCultures.contains(culture);
}
