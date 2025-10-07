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
	[[nodiscard]] const auto& getRemoveTraditions() const { return removeTraditions; }
	[[nodiscard]] const auto& getAddTraditions() const { return addTraditions; }
	[[nodiscard]] const auto& getReplaceLanguage() const { return replaceLanguage; }
	[[nodiscard]] const auto& getReplaceHeritage() const { return replaceHeritage; }

  private:
	void registerkeys();

	std::set<std::string> cultures;
	std::set<std::string> removeTraditions;
	std::set<std::string> addTraditions;
	std::optional<std::string> replaceLanguage;
	std::optional<std::string> replaceHeritage;
};
} // namespace mappers

#endif // NEW_EU4_CULTURE_MAPPING_H