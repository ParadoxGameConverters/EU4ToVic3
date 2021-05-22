#ifndef V2PROVINCE_REGIONS_MAPPER
#define V2PROVINCE_REGIONS_MAPPER
#include "Parser.h"
#include <map>

namespace mappers
{
class V2ProvinceRegionsMapper: commonItems::parser
{
  public:
	V2ProvinceRegionsMapper();
	explicit V2ProvinceRegionsMapper(std::istream& theStream);

	[[nodiscard]] std::optional<std::string> getRegionForProvince(int province) const;

  private:
	void registerKeys();
	
	std::map<int, std::string> provinceRegions;
};
} // namespace mappers

#endif // V2PROVINCE_REGIONS_MAPPER