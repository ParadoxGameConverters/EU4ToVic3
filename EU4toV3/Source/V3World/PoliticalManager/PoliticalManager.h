#ifndef POLITICAL_MANAGER_H
#define POLITICAL_MANAGER_H
#include "Configuration.h"
#include "DatingData.h"
#include "IdeaEffectsMapper/IdeaEffectsMapper.h"
#include "LawMapper/LawMapper.h"
#include "ModLoader/ModFilesystem.h"
#include "PopulationSetupMapper/PopulationSetupMapper.h"
#include "TechSetupMapper/TechSetupMapper.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace EU4
{
class EU4LocalizationLoader;
class Country;
class CultureLoader;
class ReligionLoader;
} // namespace EU4
namespace mappers
{
class CountryMapper;
class CultureMapper;
class ReligionMapper;
} // namespace mappers
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

	void initializeVanillaCountries(const commonItems::ModFilesystem& modFS);
	void loadCountryMapper(const std::shared_ptr<mappers::CountryMapper>& theCountryMapper);
	void loadPopulationSetupMapperRules(const std::string& filePath);
	void loadIdeaEffectMapperRules(const std::string& filePath);
	void loadTechSetupMapperRules(const std::string& filePath);
	void loadLawMapperRules(const std::string& filePath);
	void loadLawDefinitions(const commonItems::ModFilesystem& modFS);
	void importEU4Countries(const std::map<std::string, std::shared_ptr<EU4::Country>>& eu4Countries);
	void generateDecentralizedCountries(const ClayManager& clayManager, const PopManager& popManager);
	void convertAllCountries(const ClayManager& clayManager,
		 mappers::CultureMapper& cultureMapper,
		 const mappers::ReligionMapper& religionMapper,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader,
		 const LocalizationLoader& v3LocLoader,
		 const EU4::EU4LocalizationLoader& eu4LocLoader) const;

	[[nodiscard]] const auto& getCountries() const { return countries; }
	[[nodiscard]] std::shared_ptr<Country> getCountry(const std::string& v3Tag) const;
	[[nodiscard]] bool isTagDecentralized(const std::string& v3Tag) const;

	void determineAndApplyWesternization(const mappers::CultureMapper& cultureMapper,
		 const mappers::ReligionMapper& religionMapper,
		 Configuration::EUROCENTRISM eurocentrism,
		 const DatingData& datingData);
	void setupTech();
	void setupLaws();

  private:
	void generateDecentralizedCountry(const std::string& culture, const std::vector<std::shared_ptr<SubState>>& subStates);
	static CulturalSubStates sortSubStatesByCultures(const ClayManager& clayManager, const PopManager& popManager);
	static std::string getDominantDemographic(const std::vector<Demographic>& demographics);
	void grantLaw(const std::string& lawGroup, const std::shared_ptr<Country>& country) const;

	std::map<std::string, std::shared_ptr<Country>> countries;

	std::shared_ptr<mappers::CountryMapper> countryMapper;
	mappers::PopulationSetupMapper populationSetupMapper;
	mappers::IdeaEffectsMapper ideaEffectMapper;
	mappers::TechSetupMapper techSetupMapper;
	mappers::LawMapper lawMapper;
};
} // namespace V3
#endif // POLITICAL_MANAGER_H