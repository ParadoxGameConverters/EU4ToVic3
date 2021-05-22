#ifndef PROVINCE_MAPPING_H
#define PROVINCE_MAPPING_H
#include "Parser.h"
#include <set>

namespace mappers
{
class ProvinceMapping: commonItems::parser
{
  public:
	explicit ProvinceMapping(std::istream& theStream);

	[[nodiscard]] const auto& getEU4Provinces() const { return eu4Provinces; }
	[[nodiscard]] const auto& getVic2Provinces() const { return vic2Provinces; }

  private:
	void registerKeys();

	std::set<int> eu4Provinces;
	std::set<int> vic2Provinces;
};
} // namespace mappers

#endif // PROVINCE_MAPPING_H