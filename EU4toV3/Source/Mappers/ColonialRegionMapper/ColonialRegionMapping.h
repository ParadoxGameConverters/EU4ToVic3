#ifndef COLONIAL_REGION_MAPPING_H
#define COLONIAL_REGION_MAPPING_H
#include "Parser.h"
#include <set>

namespace mappers
{
class ColonialRegionMapping: commonItems::parser
{
  public:
	ColonialRegionMapping() = default;
	explicit ColonialRegionMapping(std::istream& theStream);

	[[nodiscard]] const auto& getRegions() const { return regions; }
	[[nodiscard]] const auto& getAloneName() const { return aloneName; }
	[[nodiscard]] const auto& getSplitName() const { return splitName; }

  private:
	void registerKeys();

	std::set<std::string> regions;
	std::string aloneName;
	std::string splitName;
};
} // namespace mappers

#endif // COLONIAL_REGION_MAPPING_H