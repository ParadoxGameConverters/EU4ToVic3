#ifndef AI_STRATEGY_MAPPER
#define AI_STRATEGY_MAPPER
#include "AIStrategyMapping.h"
#include "Parser.h"

namespace mappers
{
class AIStrategyMapper: commonItems::parser
{
  public:
	AIStrategyMapper() = default;
	void loadMappingRules(const std::string& filePath);

  private:
	void registerKeys();

	parser admParser;
	parser dipParser;
	parser polParser;

	std::map<std::string, AIStrategyMapping> admStrategies;
	std::string defaultAdmStrategy;
	std::map<std::string, AIStrategyMapping> dipStrategies;
	std::string defaultDipStrategy;
	std::map<std::string, AIStrategyMapping> polStrategies;
	std::string defaultPolStrategy;
};
} // namespace mappers

#endif // AI_STRATEGY_MAPPER