#ifndef AI_STRATEGY_MAPPING_H
#define AI_STRATEGY_MAPPING_H
#include "Parser.h"

namespace mappers
{
class AIStrategyMapping: commonItems::parser
{
  public:
	AIStrategyMapping() = default;
	explicit AIStrategyMapping(std::istream& theStream);

  private:
	void registerKeys();

	parser lawsParser;
	parser capitalParser;

	std::map<std::string, int> laws;
	std::map<std::string, int> capitalRegions;
	std::set<std::string> tags;
	int gp = 0;
	int notGP = 0;
};
} // namespace mappers

#endif // AI_STRATEGY_MAPPING_H