#ifndef COUNTRY_TIER_MAPPING_H
#define COUNTRY_TIER_MAPPING_H
#include "Parser.h"

namespace mappers
{
class CountryTierMapping: commonItems::parser
{
  public:
	CountryTierMapping() = default;
	explicit CountryTierMapping(std::istream& theStream, std::string theName);

	[[nodiscard]] std::optional<std::string> matchTier(int incRank, size_t incSize) const;

  private:
	void registerKeys();

	int rank = -1;
	std::optional<size_t> size;
	std::string name;
};
} // namespace mappers

#endif // COUNTRY_TIER_MAPPING_H