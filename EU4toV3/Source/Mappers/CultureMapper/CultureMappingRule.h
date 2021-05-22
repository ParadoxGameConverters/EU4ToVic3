#ifndef CULTURE_MAPPING_RULE_H
#define CULTURE_MAPPING_RULE_H
#include "Parser.h"
#include <set>

namespace EU4
{
class Regions;
}

namespace mappers
{
class CultureMappingRule: commonItems::parser
{
  public:
	CultureMappingRule() = default;
	explicit CultureMappingRule(std::istream& theStream);

	[[nodiscard]] std::optional<std::string> cultureMatch(const EU4::Regions& eu4Regions,
		 const std::string& eu4culture,
		 const std::string& eu4religion,
		 int eu4Province,
		 const std::string& eu4ownerTag) const;

	[[nodiscard]] std::optional<std::string> cultureRegionalMatch(const EU4::Regions& eu4Regions,
		 const std::string& eu4culture,
		 const std::string& eu4religion,
		 int eu4Province,
		 const std::string& eu4ownerTag) const;

	[[nodiscard]] std::optional<std::string> cultureNonRegionalNonReligiousMatch(const EU4::Regions& eu4Regions,
		 const std::string& eu4culture,
		 const std::string& eu4religion,
		 int eu4Province,
		 const std::string& eu4ownerTag) const;

  private:
	void registerKeys();

	std::string destinationCulture;
	std::string hpmCulture;
	std::set<std::string> cultures;
	std::set<std::string> religions;
	std::set<std::string> regions;
	std::set<std::string> owners;
	std::set<int> provinces;
};
} // namespace mappers

#endif // CULTURE_MAPPING_RULE_H
