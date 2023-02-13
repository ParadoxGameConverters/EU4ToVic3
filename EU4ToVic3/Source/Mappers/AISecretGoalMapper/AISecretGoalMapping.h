#ifndef AI_SECRET_GOAL_MAPPING_H
#define AI_SECRET_GOAL_MAPPING_H
#include "Parser.h"

namespace V3
{
class ClayManager;
class Country;
} // namespace V3

namespace mappers
{
class AISecretGoalMapping: commonItems::parser
{
  public:
	AISecretGoalMapping() = default;
	explicit AISecretGoalMapping(std::istream& theStream);

	[[nodiscard]] bool matchGoal(const V3::Country& country, const V3::Country& target, const V3::ClayManager& clayManager) const;

  private:
	void registerKeys();

	std::optional<std::string> capital;
	std::optional<std::string> targetCapital;
	std::optional<bool> targetCapitalDiffRegion;
	std::optional<int> targetRankLEQ;
	std::optional<int> targetRankGEQ;
	std::optional<bool> targetSubject;
	std::optional<bool> targetOverlord;
	std::optional<bool> targetRival;
	std::optional<bool> targetGP;
	std::optional<bool> gp;
	std::optional<bool> targetIsClaimed;
	std::optional<bool> targetIsClaimedByRival;
	std::optional<bool> govFormDiff;
};
} // namespace mappers

#endif // AI_SECRET_GOAL_MAPPING_H