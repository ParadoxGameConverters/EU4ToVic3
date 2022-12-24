#ifndef POPULATION_SETUP_MAPPING_H
#define POPULATION_SETUP_MAPPING_H
#include "Parser.h"

namespace mappers
{
class PopulationSetupMapping: commonItems::parser
{
  public:
	explicit PopulationSetupMapping(std::istream& theStream);

	[[nodiscard]] const auto& getLiteracy() const { return literacy; }
	[[nodiscard]] const auto& getAverageDev() const { return averageDev; }
	[[nodiscard]] const auto& getEffect() const { return effect; }

  private:
	void registerKeys();

	std::optional<double> literacy;
	std::optional<double> averageDev;
	std::string effect;
};
} // namespace mappers

#endif // POPULATION_SETUP_MAPPING_H