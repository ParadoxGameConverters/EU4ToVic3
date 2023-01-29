#ifndef PRODUCTION_METHOD_RULE_MAPPING_H
#define PRODUCTION_METHOD_RULE_MAPPING_H
#include "Parser.h"

namespace mappers
{
struct PMRule
{
	std::string PMName;
};
class ProductionMethodRuleMapping: commonItems::parser
{
	  public:
		ProductionMethodRuleMapping() = default;
		void loadRules(const std::string& filePath);

		[[nodiscard]] std::optional<std::string> getPersonality(const std::string& eu4Personality) const;
		[[nodiscard]] std::string getGratisIncompetency(int seed) const;

	  private:
		void registerKeys();
		[[nodiscard]] std::optional<std::string> matchLeaderSkill(const std::string& leaderType, const std::vector<int>& skills) const;

		std::map<std::string, std::string> personalities;
};
} // namespace mappers

#endif // PRODUCTION_METHOD_RULE_MAPPER_H