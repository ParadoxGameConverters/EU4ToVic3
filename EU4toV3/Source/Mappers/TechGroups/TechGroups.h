#ifndef TECH_GROUPS_H
#define TECH_GROUPS_H

#include "Parser.h"
#include <set>

namespace mappers
{
class TechGroups: commonItems::parser
{
  public:
	explicit TechGroups(std::istream& theStream);

	[[nodiscard]] auto getWesternization() const { return westernization; }
	[[nodiscard]] auto getLiteracyBoost() const { return literacyBoost; }
	[[nodiscard]] const auto& getCultures() const { return cultures; }

  private:
	// Values range 0-10. 5 is default.
	int westernization = 5;
	int literacyBoost = 5;
	std::set<std::string> cultures;
	std::set<std::string> hpmCultures;
};
} // namespace mappers

#endif // TECH_GROUPS_H