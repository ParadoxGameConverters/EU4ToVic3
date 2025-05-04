#ifndef AI_SECRET_GOAL_MAPPER
#define AI_SECRET_GOAL_MAPPER
#include "AISecretGoalMapping.h"
#include "Parser.h"
#include <filesystem>

namespace mappers
{
class AISecretGoalMapper: commonItems::parser
{
  public:
	AISecretGoalMapper() = default;
	void loadMappingRules(const std::filesystem::path& filePath);

	[[nodiscard]] std::optional<std::string> matchSecretGoal(const V3::Country& country, const V3::Country& target, const V3::ClayManager& clayManager) const;

  private:
	void registerKeys();

	parser goalParser;

	std::string activeGoal;
	std::map<std::string, std::vector<std::pair<int, AISecretGoalMapping>>> goalRules;
};
} // namespace mappers

#endif // AI_SECRET_GOAL_MAPPER