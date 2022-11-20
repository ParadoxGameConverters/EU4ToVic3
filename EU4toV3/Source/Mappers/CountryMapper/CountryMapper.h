#ifndef COUNTRY_MAPPER_H
#define COUNTRY_MAPPER_H
#include "CountryMapping.h"
#include "Parser.h"

namespace EU4
{
class Country;
}

namespace mappers
{
class CountryMapper: commonItems::parser
{
  public:
	CountryMapper() = default;
	void loadMappingRules(const std::string& filePath);
	void loadMappingRules(std::istream& theStream);

	[[nodiscard]] std::optional<std::string> getV3Tag(const std::string& eu4Tag) const;
	[[nodiscard]] std::optional<std::string> getEU4Tag(const std::string& v3Tag) const;
	[[nodiscard]] std::optional<std::string> getFlagCode(const std::string& v3Tag) const;
	[[nodiscard]] static bool tagIsDynamic(const std::string& tag);  // alpha-digit-digit, eg. C01, T15
	[[nodiscard]] static bool tagIsNonCanon(const std::string& tag); // both dynamic and imported, eg. Z0A, X0J

	[[nodiscard]] std::string assignV3TagToEU4Country(const std::shared_ptr<EU4::Country>& country);

	[[nodiscard]] const auto& getMappingRules() const { return countryMappingRules; }

  private:
	void registerKeys();

	[[nodiscard]] bool tagIsAlreadyAssigned(const std::string& v3Tag) const;
	[[nodiscard]] std::string generateNewTag();

	static [[nodiscard]] bool existsLocks(const std::set<std::string>& ruleLocks, const std::set<std::string>& countryKeys);
	static [[nodiscard]] std::optional<bool> existsBlock(const std::optional<std::string>& ruleString, const std::string& countryString);
	[[nodiscard]] std::string mapToTag(const std::string& eu4Tag, const std::optional<std::string>& v3Tag, const std::optional<std::string>& flagCode);

	std::vector<CountryMapping> countryMappingRules;
	std::map<std::string, std::string> eu4TagToV3TagMap;
	std::map<std::string, std::string> v3TagToEU4TagMap;
	std::map<std::string, std::string> v3FlagCodes; // v3 tag -> flagcode

	char generatedV3TagPrefix = 'X';
	int generatedV3TagSuffix = 0;
};
} // namespace mappers

#endif // COUNTRY_MAPPINGS_H