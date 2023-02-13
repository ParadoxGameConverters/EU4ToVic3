#ifndef AI_SECRET_GOAL_MAPPING_H
#define AI_SECRET_GOAL_MAPPING_H
#include "Parser.h"

namespace mappers
{
class AISecretGoalMapping: commonItems::parser
{
  public:
	AISecretGoalMapping() = default;
	explicit AISecretGoalMapping(std::istream& theStream);

  private:
	void registerKeys();

	std::optional<std::string> capital;
	std::optional<std::string> targetCapital;
	std::optional<bool> targetCapitalDiffRegion;
	std::optional<int> targetRankLEQ;
	std::optional<int> targetRankGEQ;
	std::optional<bool> targetSubject;
	std::optional<bool> targetOverlord;
	std::optional<bool> targetGP;
	std::optional<bool> gp;
	std::optional<bool> targetIsClaimed;
	std::optional<bool> targetIsClaimedByRival;
	std::optional<bool> govFormDiff;
};
} // namespace mappers

#endif // AI_SECRET_GOAL_MAPPING_H