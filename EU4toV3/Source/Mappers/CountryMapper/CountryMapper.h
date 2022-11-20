#ifndef COUNTRY_MAPPER_H
#define COUNTRY_MAPPER_H
#include "CountryMapping.h"
#include "Parser.h"

namespace mappers
{
class CountryMapper: commonItems::parser
{
  public:
	CountryMapper() = default;
	void loadMappingRules(const std::string& filePath);
	void loadMappingRules(std::istream& theStream);

	[[nodiscard]] std::optional<std::string> getV2Tag(const std::string& eu4Tag) const;
	[[nodiscard]] std::optional<std::string> getEU4Tag(const std::string& v3Tag) const;
	[[nodiscard]] static bool tagIsAlphaDigitDigit(const std::string& tag);
	[[nodiscard]] static bool tagIsAlphaDigitAlphaNum(const std::string& tag);

	[[nodiscard]] const auto& getMappingRules() const { return countryMappingRules; }

  private:
	void registerKeys();

	[[nodiscard]] bool tagIsAlreadyAssigned(const std::string& v3Tag) const;
	[[nodiscard]] std::string generateNewTag();

	std::vector<CountryMapping> countryMappingRules;
	std::map<std::string, std::string> eu4TagToV3TagMap;
	std::map<std::string, std::string> v3TagToEU4TagMap;

	char generatedV3TagPrefix = 'X';
	int generatedV3TagSuffix = 0;
};
} // namespace mappers

#endif // COUNTRY_MAPPINGS_H