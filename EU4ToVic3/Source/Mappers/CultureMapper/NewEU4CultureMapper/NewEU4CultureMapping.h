#ifndef NEW_EU4_CULTURE_MAPPING_H
#define NEW_EU4_CULTURE_MAPPING_H
#include "Parser.h"

namespace mappers
{
class NewEU4CultureMapping: commonItems::parser
{
  public:
	explicit NewEU4CultureMapping(std::istream& theStream);

	[[nodiscard]] const auto& getCultures() const { return cultures; }
	[[nodiscard]] const auto& getRemoveTraits() const { return removeTraits; }
	[[nodiscard]] const auto& getAddTraits() const { return addTraits; }

  private:
	void registerkeys();

	std::set<std::string> cultures;
	std::set<std::string> removeTraits;
	std::set<std::string> addTraits;
};
} // namespace mappers

#endif // NEW_EU4_CULTURE_MAPPING_H