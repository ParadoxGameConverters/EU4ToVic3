#ifndef PRODUCTION_METHOD_RULE_MAPPER_H
#define PRODUCTION_METHOD_RULE_MAPPER_H
#include "Parser.h"

namespace mappers
{
class ProductionMethodRuleMapper: commonItems::parser
{
  public:
	ProductionMethodRuleMapper() = default;
	void loadRules(const std::string& filePath);

	[[nodiscard]] std::optional<std::string> getPersonality(const std::string& eu4Personality) const;
	[[nodiscard]] std::string getGratisIncompetency(int seed) const;
	[[nodiscard]] std::string getGratisVeterancy(int seed) const;
	[[nodiscard]] std::string getGratisAgeism(int seed) const;
	[[nodiscard]] std::string getGratisDisorder(int seed) const;

  private:
	void registerKeys();
	[[nodiscard]] std::optional<std::string> matchLeaderSkill(const std::string& leaderType, const std::vector<int>& skills) const;


	std::map<std::string, std::string> personalities;
	std::map<std::string, std::string> global;
	std::set<std::string> incompetencies;
	std::set<std::string> veterancies;
	std::set<std::string> ageisms;
	std::set<std::string> disorders;
};
} // namespace mappers

#endif // PRODUCTION_METHOD_RULE_MAPPER_H