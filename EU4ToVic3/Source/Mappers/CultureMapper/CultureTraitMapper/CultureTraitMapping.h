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
	[[nodiscard]] const auto& getTraditions() const { return traditions; }
	[[nodiscard]] const auto& getHeritage() const { return heritage; }
	[[nodiscard]] const auto& getLanguage() const { return language; }
	[[nodiscard]] const auto& getEthnicity() const { return ethnicity; }
	[[nodiscard]] const auto& getGraphics() const { return graphics; }

  private:
	void registerkeys();

	std::string culture;
	std::string cultureGroup;
	std::set<std::string> traditions;
	std::string heritage;
	std::string language;
	std::string ethnicity;
	std::string graphics;
};
} // namespace mappers

#endif // CULTURE_TRAIT_MAPPING_H