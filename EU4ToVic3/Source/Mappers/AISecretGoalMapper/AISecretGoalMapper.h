#ifndef AI_SECRET_GOAL_MAPPER
#define AI_SECRET_GOAL_MAPPER
#include "AISecretGoalMapping.h"
#include "Parser.h"

namespace mappers
{
class AISecretGoalMapper: commonItems::parser
{
  public:
	AISecretGoalMapper() = default;
	void loadMappingRules(const std::string& filePath);

	[[nodiscard]] std::optional<std::string> matchSecretGoal(const V3::Country& country, const V3::Country& target, const V3::ClayManager& clayManager) const;

  private:
	void registerKeys();

	parser goalParser;

	std::string activeGoal;
	std::map<std::string, std::vector<std::pair<int, AISecretGoalMapping>>> goalRules;
};
} // namespace mappers

#endif // AI_SECRET_GOAL_MAPPER