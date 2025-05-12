#ifndef RELIGION_GROUP_MAPPER_H
#define RELIGION_GROUP_MAPPER_H
#include "Parser.h"
#include "ReligionGroupMapping.h"

namespace mappers
{
class ReligionGroupMapper: commonItems::parser
{
  public:
	ReligionGroupMapper() = default;
	void loadMappingRules(const std::string& filePath);
	void loadMappingRules(std::istream& theStream);

	[[nodiscard]] std::optional<ReligionGroupMapping> getMappingForEU4ReligionGroup(const std::string& eu4ReligionGroup) const;

  private:
	void registerKeys();

	std::vector<ReligionGroupMapping> mappings;
};
} // namespace mappers

#endif // RELIGION_GROUP_MAPPER_H
