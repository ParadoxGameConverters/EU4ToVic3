#ifndef POLITICAL_MANAGER_H
#define POLITICAL_MANAGER_H
#include "CountryDefinitionLoader/Country.h"
#include "CountryMapper/CountryMapper.h"
#include <map>
#include <memory>
#include <string>

namespace V3
{
class PoliticalManager
{
  public:
	PoliticalManager() = default;

	void initializeVanillaCountries(const std::string& V3Path);
	void loadCountryMapper(const std::shared_ptr<mappers::CountryMapper>& theCountryMapper) { countryMapper = theCountryMapper; }
	void importEU4Countries(const std::map<std::string, std::shared_ptr<EU4::Country>>& eu4Countries);
	void importVanillaCountries();

	[[nodiscard]] const auto& getCountries() const { return countries; }

  private:
	std::map<std::string, std::shared_ptr<Country>> countries;

	std::shared_ptr<mappers::CountryMapper> countryMapper;
};
} // namespace V3
#endif // POLITICAL_MANAGER_H