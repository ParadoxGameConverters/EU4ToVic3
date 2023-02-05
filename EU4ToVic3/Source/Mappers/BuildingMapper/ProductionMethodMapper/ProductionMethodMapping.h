#ifndef PRODUCTION_METHOD_MAPPING_H
#define PRODUCTION_METHOD_MAPPING_H
#include "PMRule.h"
#include "Parser.h"

namespace mappers
{
class ProductionMethodMapping: commonItems::parser
{
  public:
	ProductionMethodMapping() = default;
	void loadRules(std::istream& theStream);

	[[nodiscard]] const auto& getRules() const { return rules; }

  private:
	void registerKeys();

	std::vector<PMRule> rules;
};
} // namespace mappers

#endif // PRODUCTION_METHOD_MAPPING_H