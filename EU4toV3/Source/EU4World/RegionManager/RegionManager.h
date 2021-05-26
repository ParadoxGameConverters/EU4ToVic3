#ifndef EU4_REGIONMANAGER_H
#define EU4_REGIONMANAGER_H
#include "Area.h"
#include "Parser.h"
#include "Region.h"
#include "SuperRegion.h"

namespace EU4
{
class RegionManager: commonItems::parser
{
  public:
	void loadRegions(const std::string& EU4Path);
	void loadRegions(std::istream& areaStream, std::istream& regionStream, std::istream& superRegionStream); // for testing

	[[nodiscard]] bool provinceIsInRegion(int provinceID, const std::string& regionName) const;
	[[nodiscard]] bool regionNameIsValid(const std::string& regionName) const;

	[[nodiscard]] std::optional<std::string> getParentAreaName(int provinceID) const;
	[[nodiscard]] std::optional<std::string> getParentRegionName(int provinceID) const;
	[[nodiscard]] std::optional<std::string> getParentSuperRegionName(int provinceID) const;

  private:
	void registerAreaKeys();
	void registerSuperRegionKeys();
	void registerRegionKeys();
	void linkSuperRegions();
	void linkRegions();

	std::map<std::string, std::shared_ptr<Area>> areas;
	std::map<std::string, std::shared_ptr<Region>> regions;
	std::map<std::string, std::shared_ptr<SuperRegion>> superRegions;
};
} // namespace EU4

#endif // EU4_REGIONMANAGER_H
