#ifndef CLIMATE_MAPPER_H
#define CLIMATE_MAPPER_H
#include <set>
#include "Parser.h"

namespace mappers
{
class ClimateMapper: commonItems::parser
{
  public:
	ClimateMapper();
	explicit ClimateMapper(std::istream& theStream);

	[[nodiscard]] const auto& getClimateMap() const { return climateMap; }

  private:
	void registerKeys();

	std::map<std::string, std::vector<int>> climateMap;
	std::set<std::string> seenSoFar;
};
} // namespace mappers

#endif // CLIMATE_MAPPER_H
