#ifndef EU4_REGIONMANAGER_H
#define EU4_REGIONMANAGER_H
#include "Area.h"
#include "ColonialRegionLoader/ColonialRegionLoader.h"
#include "Mappers/SuperGroupMapper/SuperGroupMapper.h"
#include "ModLoader/ModLoader.h"
#include "Parser.h"
#include "Region.h"
#include "SuperRegion.h"

namespace EU4
{
class ProvinceManager;
class RegionManager: commonItems::parser
{
  public:
	void loadRegions(const std::string& EU4Path, const Mods& mods);
	void loadRegions(std::istream& areaStream, std::istream& regionStream, std::istream& superRegionStream); // for testing
	void loadSuperGroups(const mappers::SuperGroupMapper& sgMapper) { superGroupMapper = sgMapper; }			// for testing

	void loadColonialRegions(const std::string& EU4Path, const Mods& mods) { colonialRegionLoader.loadColonialRegions(EU4Path, mods); }
	void loadColonialRegions(const ColonialRegionLoader& loader) { colonialRegionLoader = loader; } // testing

	[[nodiscard]] bool provinceIsInRegion(int provinceID, const std::string& regionName) const;
	[[nodiscard]] bool regionNameIsValid(const std::string& regionName) const;
	[[nodiscard]] bool provinceIsValid(int provinceID) const;
	[[nodiscard]] std::optional<std::string> getColonialRegionForProvince(int province) const;

	[[nodiscard]] std::optional<std::string> getParentAreaName(int provinceID) const;
	[[nodiscard]] std::optional<std::string> getParentRegionName(int provinceID) const;
	[[nodiscard]] std::optional<std::string> getParentSuperRegionName(int provinceID) const;
	[[nodiscard]] std::optional<std::string> getParentSuperGroupName(int provinceID) const;
	[[nodiscard]] std::optional<double> getAssimilationFactor(int provinceID) const;

	[[nodiscard]] bool doesProvinceRequireNeoCulture(int provinceID, const std::string& culture) const;

	void applySuperGroups();
	void catalogueNativeCultures(const ProvinceManager& provinceManager);

  private:
	void registerAreaKeys();
	void registerSuperRegionKeys();
	void registerRegionKeys();
	void linkSuperRegions();
	void linkRegions();

	std::map<std::string, std::shared_ptr<Area>> areas;
	std::map<std::string, std::shared_ptr<Region>> regions;
	std::map<std::string, std::shared_ptr<SuperRegion>> superRegions;

	mappers::SuperGroupMapper superGroupMapper;
	ColonialRegionLoader colonialRegionLoader;
};
} // namespace EU4

#endif // EU4_REGIONMANAGER_H
