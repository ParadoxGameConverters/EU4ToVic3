#ifndef COUNTRY_FLAGS_MAPPING_H
#define COUNTRY_FLAGS_MAPPING_H
#include "Parser.h"
#include <set>

namespace mappers
{
class CountryFlagsMapping: commonItems::parser
{
  public:
	CountryFlagsMapping(std::istream& theStream);

	[[nodiscard]] const auto& getEU4Flag() const { return eu4Flag; }
	[[nodiscard]] const auto& getV2Flags() const { return v2Flags; }

  private:
	void registerKeys();

	std::string eu4Flag;
	std::set<std::string> v2Flags;
};
} // namespace mappers

#endif // COUNTRY_FLAGS_MAPPING_H