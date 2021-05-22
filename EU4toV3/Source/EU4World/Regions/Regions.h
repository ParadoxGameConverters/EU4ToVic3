#ifndef EU4_REGIONS_H
#define EU4_REGIONS_H
#include "Parser.h"
#include "Region.h"
#include "Areas.h"
#include "SuperRegions.h"
#include <map>

namespace EU4
{
	class Regions: commonItems::parser
	{
	public:
		Regions(const SuperRegions& sRegions, const Areas& areas, std::istream& regionsFile);
		explicit Regions(const Areas& areas);
		
		[[nodiscard]] bool provinceInRegion(int province, const std::string& regionName) const;
		[[nodiscard]] bool regionIsValid(const std::string& regionName) const;

		[[nodiscard]] std::optional<std::string> getParentAreaName(int provinceID) const;
		[[nodiscard]] std::optional<std::string> getParentRegionName(int provinceID) const;
		[[nodiscard]] std::optional<std::string> getParentSuperRegionName(int provinceID) const;

	private:
		void registerKeys(const Areas& areas);
		
		std::map<std::string, Region> regions;
		std::map<std::string, std::set<std::string>> superRegions;
	};
}

#endif //EU4_REGIONS_H
