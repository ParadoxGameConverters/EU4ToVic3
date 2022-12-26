#ifndef IDEA_EFFECTS_MAPPING_H
#define IDEA_EFFECTS_MAPPING_H
#include "Parser.h"
#include <set>

namespace mappers
{
class IdeaEffectsMapping: commonItems::parser
{
  public:
	IdeaEffectsMapping() = default;
	explicit IdeaEffectsMapping(std::istream& theStream);

	[[nodiscard]] auto getLiteracy() const { return literacy; }
	[[nodiscard]] auto getAdm() const { return adm; }
	[[nodiscard]] auto getDip() const { return dip; }
	[[nodiscard]] auto getMil() const { return mil; }
	[[nodiscard]] const auto& getBoostedInterestGroups() const { return boostedInterestGroups; }
	[[nodiscard]] const auto& getSuppressedInterestGroups() const { return suppressedInterestGroups; }

  private:
	void registerKeys();

	int literacy = 0;
	int adm = 0;
	int dip = 0;
	int mil = 0;
	std::set<std::string> boostedInterestGroups;
	std::set<std::string> suppressedInterestGroups;
};
} // namespace mappers

#endif // IDEA_EFFECTS_MAPPING_H