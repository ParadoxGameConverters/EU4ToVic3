#ifndef POLITICAL_MANAGER_H
#define POLITICAL_MANAGER_H
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace EU4
{
class EU4LocalizationLoader;
class Country;
} // namespace EU4
namespace mappers
{
class CountryMapper;
}
namespace V3
{
struct Demographic;
class Country;
class SubState;
class PopManager;
class ClayManager;
class LocalizationLoader;
class PoliticalManager
{
  public:
	using CulturalSubStates = std::map<std::string, std::vector<std::shared_ptr<SubState>>>; // culture -> substates

	PoliticalManager() = default;

	void initializeVanillaCountries(const std::string& V3Path);
	void loadCountryMapper(const std::shared_ptr<mappers::CountryMapper>& theCountryMapper) { countryMapper = theCountryMapper; }
	void importEU4Countries(const std::map<std::string, std::shared_ptr<EU4::Country>>& eu4Countries);
	void generateDecentralizedCountries(const ClayManager& clayManager, const PopManager& popManager);
	void convertAllCountries(const ClayManager& clayManager, const LocalizationLoader& v3LocLoader, const EU4::EU4LocalizationLoader& eu4LocLoader) const;

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