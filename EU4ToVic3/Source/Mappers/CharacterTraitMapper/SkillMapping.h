#ifndef SKILL_MAPPING_H
#define SKILL_MAPPING_H
#include "Parser.h"

namespace mappers
{
class SkillMapping: commonItems::parser
{
  public:
	SkillMapping() = default;
	explicit SkillMapping(std::istream& theStream);

	[[nodiscard]] std::optional<std::string> matchTrait(const std::vector<int>& stats) const;

  private:
	void registerKeys();

	int sum = 0;
	std::optional<std::string> primary;
	std::string trait;
};
} // namespace mappers

#endif // SKILL_MAPPING_H