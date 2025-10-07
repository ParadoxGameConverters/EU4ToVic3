#ifndef WESTERNIZATION_MAPPER_H
#define WESTERNIZATION_MAPPER_H

#include "Parser.h"
#include <filesystem>
#include <map>

namespace mappers
{
struct CultureDef;
class WesternizationMapper: commonItems::parser
{
  public:
	WesternizationMapper() = default;
	void loadMappingRules(const std::filesystem::path& filePath);
	void loadMappingRules(std::istream& theStream);

	[[nodiscard]] int getWesternizationForCulture(const CultureDef& culture) const;
	[[nodiscard]] int getLiteracyForCulture(const CultureDef& culture) const;
	[[nodiscard]] int getIndustryForCulture(const CultureDef& culture) const;

  private:
	void registerKeys();

	std::map<std::string, int> westernizations;
	std::map<std::string, int> literacies;
	std::map<std::string, int> industries;
};
} // namespace mappers

#endif // WESTERNIZATION_MAPPER_H
