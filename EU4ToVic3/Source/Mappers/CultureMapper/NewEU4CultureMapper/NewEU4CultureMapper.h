#ifndef NEW_EU4_CULTURE_MAPPER_H
#define NEW_EU4_CULTURE_MAPPER_H
#include "NewEU4CultureMapping.h"
#include "Parser.h"

namespace mappers
{
class NewEU4CultureMapper: commonItems::parser
{
  public:
	NewEU4CultureMapper() = default;
	void loadMappingRules(const std::string& filePath);
	void loadMappingRules(std::istream& theStream);

	[[nodiscard]] std::set<std::string> getRemoveTraitsForCulture(const std::string& eu4Culture) const;
	[[nodiscard]] std::set<std::string> getAddTraitsForCulture(const std::string& eu4Culture) const;
	[[nodiscard]] bool isInterestingCulture(const std::string& eu4Culture) const { return interestingCultures.contains(eu4Culture); }

  private:
	void registerKeys();

	std::vector<NewEU4CultureMapping> mappings;
	std::set<std::string> interestingCultures;
};
} // namespace mappers

#endif // NEW_EU4_CULTURE_MAPPER_H
