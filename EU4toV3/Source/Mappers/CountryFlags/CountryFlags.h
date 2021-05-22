#ifndef COUNTRY_FLAGS_H
#define COUNTRY_FLAGS_H
#include "Parser.h"

namespace mappers
{
class CountryFlags: commonItems::parser
{
  public:
	CountryFlags();
	explicit CountryFlags(std::istream& theStream);

	[[nodiscard]] const auto& getFlags() const { return flags; }

  private:
	void registerKeys();

	std::multimap<std::string, std::string> flags;
};
} // namespace mappers


#endif // COUNTRY_FLAGS_H