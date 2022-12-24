#ifndef WESTERNIZATION_MAPPING_H
#define WESTERNIZATION_MAPPING_H
#include "Parser.h"
#include <set>

namespace mappers
{
class WesternizationMapping: commonItems::parser
{
  public:
	explicit WesternizationMapping(std::istream& theStream);

	[[nodiscard]] auto getWesternization() const { return westernization; }
	[[nodiscard]] auto getLiteracy() const { return literacy; }
	[[nodiscard]] const auto& getIndustry() const { return industry; }
	[[nodiscard]] const auto& getTraits() const { return traits; }

  private:
	void registerKeys();

	// Values range 0-10. 5 is default.
	int westernization = 5;
	int literacy = 5;
	int industry = 5;
	std::set<std::string> traits;
};
} // namespace mappers

#endif // WESTERNIZATION_MAPPING_H