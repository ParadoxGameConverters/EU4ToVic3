#ifndef POLITICAL_MANAGER_H
#define POLITICAL_MANAGER_H
#include "CountryDefinitionLoader/Country.h"
#include "CountryMapper/CountryMapper.h"
#include <map>
#include <memory>
#include <string>

namespace V3
{
struct Demographic;
class PopManager;
class ClayManager;
class PoliticalManager
{
  public:
	using CulturalSubStates = std::map<std::string, std::vector<std::shared_ptr<SubState>>>; // culture -> substates

	PoliticalManager() = default;

	void initializeVanillaCountries(const std::string& V3Path);
	void loadCountryMapper(const std::shared_ptr<mappers::CountryMapper>& theCountryMapper) { countryMapper = theCountryMapper; }
	void importEU4Countries(const std::map<std::string, std::shared_ptr<EU4::Country>>& eu4Countries);
	void importVanillaCountries();
	void generateDecentralizedCountries(const ClayManager& clayManager, const PopManager& popManager);

	[[nodiscard]] const auto& getCountries() const { return countries; }

  private:
	void generateDecentralizedCountry(const std::string& culture, const std::vector<std::shared_ptr<SubState>>& subStates);
	static CulturalSubStates sortSubStatesByCultures(const ClayManager& clayManager, const PopManager& popManager);
	static std::string getDominantDemographic(const std::vector<Demographic>& demographics);

	std::map<std::string, std::shared_ptr<Country>> countries;

	std::shared_ptr<mappers::CountryMapper> countryMapper;
};
} // namespace V3
#endif // POLITICAL_MANAGER_H