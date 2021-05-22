#ifndef COUNTRY_MAPPINGS_H
#define COUNTRY_MAPPINGS_H
#include "ColonialTags/ColonialTagsMapper.h"
#include "CountryMapping.h"
#include "Parser.h"
#include "Titles/TitleMapper.h"
#include "V2ProvinceRegionsMapper/V2ProvinceRegionsMapper.h"

namespace EU4
{
class Country;
}

namespace mappers
{
class ProvinceMapper;
class CultureGroups;
class CountryMappings: commonItems::parser
{
  public:
	CountryMappings();
	explicit CountryMappings(std::istream& mainStream, std::istream& titleMapperStream, std::istream& v2RegionsStream, std::istream& colonialTagStream);

	[[nodiscard]] std::optional<std::string> getV2Tag(const std::string& eu4Tag) const;
	[[nodiscard]] std::optional<std::string> getTitle(const std::string& countryName) const;
	[[nodiscard]] const auto& getTitleMapper() const { return *titleMapper; }
	[[nodiscard]] static bool tagIsAlphaDigitDigit(const std::string& tag);

	void createMappings(const std::shared_ptr<CultureGroups>& theCultureGroupsMapper,
		 const std::map<std::string, std::shared_ptr<EU4::Country>>& eu4Countries,
		 const ProvinceMapper& provinceMapper);

  private:
	void registerKeys();
	void makeOneMapping(const std::shared_ptr<EU4::Country>& country);
	void mapToTag(const std::string& eu4Tag, const std::string& vic2Tag);

	[[nodiscard]] std::optional<std::string> determineMappableCKTitle(const EU4::Country& country);
	[[nodiscard]] std::string generateNewTag();
	[[nodiscard]] bool attemptStraightMapping(const EU4::Country& country, const std::string& EU4Identifier);
	[[nodiscard]] bool mapToVic2Country(const std::string& V2Tag, const std::string& EU4Tag);
	[[nodiscard]] bool attemptColonialReplacement(EU4::Country& country, const ProvinceMapper& provinceMapper);
	[[nodiscard]] static bool isPotentialColonialReplacement(const std::pair<std::string, std::shared_ptr<EU4::Country>>& country);
	[[nodiscard]] bool tagIsAlreadyAssigned(const std::string& vic2Tag) const;

	std::vector<std::pair<std::string, CountryMapping>> eu4TagToV2TagsRules; // eu4Tag, related_rule
	std::map<std::string, std::string> eu4TagToV2TagMap;
	std::map<std::string, std::string> v2TagToEU4TagMap;

	char generatedV2TagPrefix = 'X';
	int generatedV2TagSuffix = 0;

	std::unique_ptr<TitleMapper> titleMapper;
	std::unique_ptr<ColonialTagMapper> colonialTagMapper;
	std::unique_ptr<V2ProvinceRegionsMapper> v2provinceRegionsMapper;
	std::shared_ptr<CultureGroups> cultureGroupsMapper;
};
} // namespace mappers

#endif // COUNTRY_MAPPINGS_H