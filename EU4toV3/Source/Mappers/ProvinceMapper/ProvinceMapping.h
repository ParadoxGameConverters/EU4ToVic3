#ifndef PROVINCE_MAPPING_H
#define PROVINCE_MAPPING_H
#include "Parser.h"

namespace mappers
{
class ProvinceMapping: commonItems::parser
{
  public:
	ProvinceMapping() = default;
	explicit ProvinceMapping(std::istream& theStream);

	[[nodiscard]] const auto& getEU4Provinces() const { return eu4Provinces; }
	[[nodiscard]] const auto& getV3Provinces() const { return v3Provinces; }

  private:
	void registerKeys();

	std::vector<int> eu4Provinces;
	std::vector<std::string> v3Provinces;
};
} // namespace mappers

#endif // PROVINCE_MAPPING_H