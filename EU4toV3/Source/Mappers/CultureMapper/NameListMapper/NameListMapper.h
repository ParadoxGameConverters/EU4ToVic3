#ifndef NAME_LIST_MAPPER_H
#define NAME_LIST_MAPPER_H
#include "NameListMapping.h"
#include "Parser.h"

namespace mappers
{
class NameListMapper: commonItems::parser
{
  public:
	NameListMapper() = default;
	void loadMappingRules(const std::string& filePath);
	void loadMappingRules(std::istream& theStream);

	[[nodiscard]] std::optional<NameListMapping> getNamesForCulture(const std::string& eu4Culture, const std::string& eu4CultureGroup) const;

  private:
	void registerKeys();

	std::vector<NameListMapping> mappings;
};
} // namespace mappers

#endif // NAME_LIST_MAPPER_H
