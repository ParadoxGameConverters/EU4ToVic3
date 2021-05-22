#include "Regions.h"
#include "Areas.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::Regions::Regions(const SuperRegions& sRegions, const Areas& areas, std::istream& regionsFile)
{
	registerKeys(areas);
	parseStream(regionsFile);
	clearRegisteredKeywords();

	superRegions = sRegions.getSuperRegions();
}

void EU4::Regions::registerKeys(const Areas& areas)
{
	registerRegex(R"(\w+_region)", [this, areas](const std::string& regionName, std::istream& areasFile) {
		Region newRegion(areasFile);
		newRegion.addProvinces(areas);
		regions.emplace(regionName, newRegion);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

EU4::Regions::Regions(const Areas& areas)
{
	for (const auto& theArea: areas.getAreas())
		regions.emplace(theArea.first, Region(theArea.second));
}

bool EU4::Regions::provinceInRegion(int province, const std::string& regionName) const
{
	const auto& regionItr = regions.find(regionName);
	if (regionItr != regions.end())
		return regionItr->second.regionContainsProvince(province);

	// "Regions" are such a fluid term.
	const auto& superRegionItr = superRegions.find(regionName);
	if (superRegionItr != superRegions.end())
		for (const auto& regionalName: superRegionItr->second)
		{
			const auto& regionalItr = regions.find(regionalName);
			if (regionalItr == regions.end())
				continue;
			if (regionalItr->second.regionContainsProvince(province))
				return true;
		}

	// And sometimes they don't mean what people think they mean at all.
	for (const auto& region: regions)
		for (const auto& areaName: region.second.getAreaNames())
			if (areaName == regionName)
				return region.second.areaContainsProvince(areaName, province);

	return false;
}

std::optional<std::string> EU4::Regions::getParentAreaName(const int provinceID) const
{
	for (const auto& region: regions)
		for (const auto& areaName: region.second.getAreaNames())
			if (region.second.areaContainsProvince(areaName, provinceID))
				return areaName;

	Log(LogLevel::Warning) << "Province ID " << provinceID << " has no parent area name! (Area mismatch? Using newer EU4 version to convert older save?)";
	return std::nullopt;
}

std::optional<std::string> EU4::Regions::getParentRegionName(const int provinceID) const
{
	for (const auto& region: regions)
		if (region.second.regionContainsProvince(provinceID))
			return region.first;

	Log(LogLevel::Warning) << "Province ID " << provinceID << " has no parent region name! (Region mismatch? Using newer EU4 version to convert older save?)";
	return std::nullopt;
}

std::optional<std::string> EU4::Regions::getParentSuperRegionName(const int provinceID) const
{
	for (const auto& superRegion: superRegions)
		for (const auto& regionalName: superRegion.second)
		{
			const auto& regionalItr = regions.find(regionalName);
			if (regionalItr != regions.end())
				if (regionalItr->second.regionContainsProvince(provinceID))
					return superRegion.first;
		}
	return std::nullopt;
}

bool EU4::Regions::regionIsValid(const std::string& regionName) const
{
	if (const auto& regionItr = regions.find(regionName); regionItr != regions.end())
		return true;

	// Who knows what the mapper needs. All kinds of stuff.
	if (const auto& superRegionItr = superRegions.find(regionName); superRegionItr != superRegions.end())
		return true;

	// And more stuff, what's the worst that could happen?
	for (const auto& region: regions)
		if (region.second.getAreaNames().contains(regionName))
			return true;

	return false;
}