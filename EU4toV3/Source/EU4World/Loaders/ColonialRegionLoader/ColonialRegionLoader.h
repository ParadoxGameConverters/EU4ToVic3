#ifndef COLONIAL_REGION_LOADER
#define COLONIAL_REGION_LOADER
#include "ModLoader/ModLoader.h"
#include "Parser.h"
#include <map>

namespace EU4
{
class ColonialRegionLoader: commonItems::parser
{
  public:
	ColonialRegionLoader() = default;

	void loadColonialRegions(std::istream& theStream);
	void loadColonialRegions(const std::string& EU4Path, const Mods& mods);

	[[nodiscard]] std::optional<std::string> getColonialRegionForProvince(int province) const;

  private:
	void registerKeys();

	std::map<int, std::string> provinceToColonialRegions;
};
} // namespace EU4

#endif // COLONIAL_REGION_LOADER