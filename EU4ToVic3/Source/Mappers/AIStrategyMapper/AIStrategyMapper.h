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

	[[nodiscard]] std::map<std::string, int> getAdmStrategies(const V3::Country& country, const V3::ClayManager& clayManager) const;
	[[nodiscard]] std::map<std::string, int> getDipStrategies(const V3::Country& country, const V3::ClayManager& clayManager) const;
	[[nodiscard]] std::map<std::string, int> getPolStrategies(const V3::Country& country, const V3::ClayManager& clayManager) const;

  private:
	void registerKeys();

	[[nodiscard]] static std::map<std::string, int> filterStrategies(const std::map<std::string, AIStrategyMapping>& strategies,
		 std::string defaultStrategy,
		 const V3::Country& country,
		 const V3::ClayManager& clayManager);

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