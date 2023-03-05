#ifndef COUNTRY_TIER_MAPPER_H
#define COUNTRY_TIER_MAPPER_H
#include "CountryTierMapping.h"
#include "Parser.h"

namespace mappers
{
class CountryTierMapper: commonItems::parser
{
  public:
	CountryTierMapper() = default;
	void loadMappingRules(const std::string& filePath);

	[[nodiscard]] std::optional<std::string> matchCountryTier(int rank, size_t size, bool vn = false) const;

  private:
	void registerKeys();
	parser gameTypeParser;
	bool vanillaGame = false;

	std::vector<CountryTierMapping> vanillaMappings;
	std::vector<CountryTierMapping> vnMappings;
};
} // namespace mappers

#endif // COUNTRY_MAPPINGS_H