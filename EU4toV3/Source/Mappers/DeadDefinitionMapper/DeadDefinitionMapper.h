#ifndef DEAD_DEFINITION_MAPPER
#define DEAD_DEFINITION_MAPPER
#include "DeadDefinitionMapping.h"
#include "Parser.h"

namespace mappers
{
class DeadDefinitionMapper: commonItems::parser
{
  public:
	DeadDefinitionMapper();
	explicit DeadDefinitionMapper(std::istream& theStream);

	[[nodiscard]] std::optional<DeadDefinition> getDeadDefinitionForTag(const std::string& tag) const;

  private:
	void registerKeys();

	std::map<std::string, DeadDefinition> deadDefinitions;
};
} // namespace mappers

#endif // DEAD_DEFINITION_MAPPER
