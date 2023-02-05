#ifndef CULTURE_TRAIT_MAPPING_H
#define CULTURE_TRAIT_MAPPING_H
#include "Parser.h"

namespace mappers
{
class CultureTraitMapping: commonItems::parser
{
  public:
	explicit CultureTraitMapping(std::istream& theStream);

	[[nodiscard]] const auto& getCulture() const { return culture; }
	[[nodiscard]] const auto& getCultureGroup() const { return cultureGroup; }
	[[nodiscard]] const auto& getTraits() const { return traits; }
	[[nodiscard]] const auto& getEthnicity() const { return ethnicity; }
	[[nodiscard]] const auto& getGraphics() const { return graphics; }

  private:
	void registerkeys();

	std::string culture;
	std::string cultureGroup;
	std::set<std::string> traits;
	std::string ethnicity;
	std::string graphics;
};
} // namespace mappers

#endif // CULTURE_TRAIT_MAPPING_H