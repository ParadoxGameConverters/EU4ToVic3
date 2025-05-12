#ifndef CULTURE_TRAIT_MAPPER_H
#define CULTURE_TRAIT_MAPPER_H
#include "CultureTraitMapping.h"
#include "Parser.h"

namespace mappers
{
class CultureTraitMapper: commonItems::parser
{
  public:
	CultureTraitMapper() = default;
	void loadMappingRules(const std::string& filePath);
	void loadMappingRules(std::istream& theStream);

	[[nodiscard]] std::optional<CultureTraitMapping> getTraitsForCulture(const std::string& eu4Culture, const std::string& eu4CultureGroup) const;

  private:
	void registerKeys();

	std::vector<CultureTraitMapping> mappings;
};
} // namespace mappers

#endif // RELIGION_GROUP_MAPPER_H
