#ifndef SUPER_REGIONS_H
#define SUPER_REGIONS_H
#include "Parser.h"
#include <map>
#include <set>

namespace EU4
{
class SuperRegions: commonItems::parser
{
  public:
	SuperRegions() = default;
	explicit SuperRegions(std::istream& theStream);

	[[nodiscard]] const auto& getSuperRegions() const { return superRegionMap; }

  private:
	void registerKeys();

	std::map<std::string, std::set<std::string>> superRegionMap;
};
} // namespace EU4

#endif // SUPER_REGIONS_H
