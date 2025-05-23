#ifndef POPULATION_SETUP_MAPPER_H
#define POPULATION_SETUP_MAPPER_H
#include "Parser.h"
#include <filesystem>
#include <map>

namespace mappers
{
class PopulationSetupMapper: commonItems::parser
{
  public:
	PopulationSetupMapper() = default;
	void loadMappingRules(const std::filesystem::path& filePath);
	void loadMappingRules(std::istream& theStream);

	[[nodiscard]] std::string getWealthEffectForDev(double dev) const;
	[[nodiscard]] std::string getLiteracyEffectForLiteracy(double literacy) const;

  private:
	void registerKeys();

	std::map<double, std::string> literacySteps;
	std::map<double, std::string> wealthSteps;
};
} // namespace mappers

#endif // POPULATION_SETUP_MAPPER_H
