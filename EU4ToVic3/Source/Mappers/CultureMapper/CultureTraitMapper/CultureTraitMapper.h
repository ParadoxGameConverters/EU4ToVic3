#ifndef CULTURE_TRAIT_MAPPER_H
#define CULTURE_TRAIT_MAPPER_H
#include "CultureTraitMapping.h"
#include "Parser.h"
#include <filesystem>

namespace mappers
{
class CultureTraitMapper: commonItems::parser
{
  public:
	CultureTraitMapper() = default;
	void loadMappingRules(const std::filesystem::path& heritageFilePath,
		 const std::filesystem::path& languageFilePath,
		 const std::filesystem::path& traditionFilePath);
	void loadMappingRules(std::istream& heritageStream, std::istream& languageStream, std::istream& traditionStream);

	[[nodiscard]] std::optional<CultureTraitMapping> getTraitsForCulture(const std::string& eu4Culture, const std::string& eu4CultureGroup) const;

  private:
	void registerHeritageKeys();
	void registerLanguageKeys();
	void registerTraditionKeys();

	std::vector<CultureTraitMapping> heritageMappings;
	std::vector<CultureTraitMapping> languageMappings;
	std::vector<CultureTraitMapping> traditionMappings;
};
} // namespace mappers

#endif // RELIGION_GROUP_MAPPER_H
