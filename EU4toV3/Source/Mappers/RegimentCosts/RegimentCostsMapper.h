#ifndef REGIMENT_COSTS_MAPPER_H
#define REGIMENT_COSTS_MAPPER_H
#include "Parser.h"
#include <map>

namespace mappers
{
class RegimentCostsMapper: commonItems::parser
{
  public:
	RegimentCostsMapper();
	explicit RegimentCostsMapper(std::istream& theStream);

	[[nodiscard]] std::optional<int> getCostForRegimentType(const std::string& regimentType) const;

  private:
	void registerKeys();

	std::map<std::string, int> regimentCosts;
};
} // namespace mappers

#endif // REGIMENT_COSTS_MAPPER_H
