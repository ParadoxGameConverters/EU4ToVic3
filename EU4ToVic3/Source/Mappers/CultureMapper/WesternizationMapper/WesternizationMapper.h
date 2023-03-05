#ifndef WESTERNIZATION_MAPPER_H
#define WESTERNIZATION_MAPPER_H

#include "Parser.h"
#include <map>

namespace mappers
{
class WesternizationMapper: commonItems::parser
{
  public:
	WesternizationMapper() = default;
	void loadMappingRules(const std::string& filePath);
	void loadMappingRules(std::istream& theStream);

	[[nodiscard]] int getWesternizationForTraits(const std::set<std::string>& traits) const;
	[[nodiscard]] int getLiteracyForTraits(const std::set<std::string>& traits) const;
	[[nodiscard]] int getIndustryForTraits(const std::set<std::string>& traits) const;

  private:
	void registerKeys();

	std::map<std::string, int> westernizations;
	std::map<std::string, int> literacies;
	std::map<std::string, int> industries;
};
} // namespace mappers

#endif // WESTERNIZATION_MAPPER_H
