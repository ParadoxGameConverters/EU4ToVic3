#ifndef EU4_COUNTRY_HISTORY_DATE
#define EU4_COUNTRY_HISTORY_DATE
#include "CountryManager/CountryLeader/EU4Leader.h"
#include "Parser.h"

namespace EU4
{
class CountryHistoryDate: commonItems::parser
{
  public:
	explicit CountryHistoryDate(std::istream& theStream);

	[[nodiscard]] const auto& getCharacters() const { return characters; }

  private:
	void registerKeys();

	std::vector<Character> characters;
};
} // namespace EU4

#endif // EU4_COUNTRY_HISTORY_DATE