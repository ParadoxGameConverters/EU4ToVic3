#ifndef EU4_COUNTRY_FLAGS_H
#define EU4_COUNTRY_FLAGS_H
#include "Parser.h"
#include <set>

namespace EU4
{
class EU4CountryFlags: commonItems::parser
{
  public:
	EU4CountryFlags() = default;
	explicit EU4CountryFlags(std::istream& theStream);

	[[nodiscard]] const auto& getFlags() const { return flags; }

  private:
	void registerKeys();

	std::set<std::string> flags;
};
} // namespace EU4

#endif // EU4_COUNTRY_FLAGS_H