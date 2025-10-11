#ifndef CULTURE_TRAIT_MAPPING_H
#define CULTURE_TRAIT_MAPPING_H
#include "Parser.h"

namespace mappers
{
class CultureTraitMapping: commonItems::parser
{
  public:
	CultureTraitMapping() = default;
	explicit CultureTraitMapping(std::istream& theStream);

	[[nodiscard]] const auto& getCultures() const { return cultures; }
	[[nodiscard]] const auto& getCultureGroups() const { return cultureGroups; }
	[[nodiscard]] const auto& getTraditions() const { return traditions; }
	[[nodiscard]] const auto& getHeritage() const { return heritage; }
	[[nodiscard]] const auto& getLanguage() const { return language; }
	[[nodiscard]] const auto& getEthnicity() const { return ethnicity; }
	[[nodiscard]] const auto& getGraphics() const { return graphics; }

	void setLanguage(const std::string& theLanguage) { language = theLanguage; }
	void setTraditions(const std::set<std::string>& theTraditions) { traditions = theTraditions; }

  private:
	void registerkeys();

	std::set<std::string> cultures;
	std::set<std::string> cultureGroups;
	std::set<std::string> traditions;
	std::string heritage;
	std::string language;
	std::string ethnicity;
	std::string graphics;
};
} // namespace mappers

#endif // CULTURE_TRAIT_MAPPING_H