#ifndef DEAD_DEFINITION_MAPPING_H
#define DEAD_DEFINITION_MAPPING_H
#include "Parser.h"
#include <set>
#include <string>

namespace mappers
{
struct DeadDefinition
{
	std::string tag;
	std::optional<std::string> culture;
	std::optional<std::string> religion;
	std::optional<std::string> government;
	std::optional<bool> civilized;
	std::optional<int> capital;
	std::set<std::string> acceptedCultures;
};

class DeadDefinitionMapping: commonItems::parser
{
  public:
	DeadDefinitionMapping() = default;
	DeadDefinitionMapping(std::istream& theStream);

	[[nodiscard]] const auto& getDefinition() const { return deadDefinition; }

  private:
	void registerKeys();

	DeadDefinition deadDefinition;
};
} // namespace mappers

#endif // DEAD_DEFINITION_MAPPING_H
