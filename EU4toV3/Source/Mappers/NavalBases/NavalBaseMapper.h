#ifndef MAP_PROVINCE_H
#define MAP_PROVINCE_H
#include "Parser.h"
#include <set>

namespace mappers
{
class NavalBaseMapper: commonItems::parser
{
  public:
	NavalBaseMapper();
	explicit NavalBaseMapper(std::istream& theStream);

	[[nodiscard]] auto isProvinceCoastal(int provID) const { return navalProvinces.contains(provID); }

  private:
	void registerKeys();

	std::set<int> navalProvinces;
};
} // namespace mappers

#endif // MAP_PROVINCE_H
