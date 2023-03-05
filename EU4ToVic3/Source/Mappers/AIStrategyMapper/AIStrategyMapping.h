#ifndef AI_STRATEGY_MAPPING_H
#define AI_STRATEGY_MAPPING_H
#include "Parser.h"

namespace V3
{
class ClayManager;
class Country;
} // namespace V3

namespace mappers
{
class AIStrategyMapping: commonItems::parser
{
  public:
	AIStrategyMapping() = default;
	explicit AIStrategyMapping(std::istream& theStream);

	[[nodiscard]] auto isOverride() const { return override; }
	[[nodiscard]] int matchStrategy(const V3::Country& country, const V3::ClayManager& clayManager) const;

  private:
	void registerKeys();

	parser lawsParser;
	parser capitalParser;

	std::map<std::string, int> laws;
	std::map<std::string, int> capitalRegions;
	int gp = 0;
	int notGP = 0;
	int hremperor = 0;
	int cemperor = 0;
	bool override = false;
};
} // namespace mappers

#endif // AI_STRATEGY_MAPPING_H