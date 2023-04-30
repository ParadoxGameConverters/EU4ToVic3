#ifndef POLITICAL_MANAGER_H
#define POLITICAL_MANAGER_H
#include "AISecretGoalMapper/AISecretGoalMapper.h"
#include "AIStrategyMapper/AIStrategyMapper.h"
#include "CharacterTraitMapper/CharacterTraitMapper.h"
#include "ColonialTagMapper/ColonialTagMapper.h"
#include "Configuration.h"
#include "CountryTierMapper/CountryTierMapper.h"
#include "DatingData.h"
#include "Diplomacy/Agreement.h"
#include "DiplomaticMapper/DiplomaticMapper.h"
#include "IGIdeologiesMapper/IGIdeologiesMapper.h"
#include "IdeaEffectsMapper/IdeaEffectsMapper.h"
#include "LawMapper/LawMapper.h"
#include "Loaders/MajorFormablesLoader/MajorFormablesLoader.h"
#include "Loaders/VanillaCharacterLoader/VanillaCharacterLoader.h"
#include "Loaders/VanillaCountryHistoryLoader/VanillaCountryHistoryLoader.h"
#include "Loaders/VanillaDiplomacyLoader/VanillaDiplomacyLoader.h"
#include "Loaders/VanillaDiplomaticPlayLoader/VanillaDiplomaticPlayLoader.h"
#include "Loaders/VanillaPopulationHistoryLoader/VanillaPopulationHistoryLoader.h"
#include "ModLoader/ModFilesystem.h"
#include "PopulationSetupMapper/PopulationSetupMapper.h"
#include "TechSetupMapper/TechSetupMapper.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace EU4
{
class EU4Agreement;
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

	void initializeVanillaCountries(const commonItems::ModFilesystem& modFS, const commonItems::ModFilesystem& vanillaFS);
	void loadCountryMapper(const std::shared_ptr<mappers::CountryMapper>& theCountryMapper);
	void loadPopulationSetupMapperRules(const std::string& filePath);
	void loadIdeaEffectMapperRules(const std::string& filePath);
	void loadTechSetupMapperRules(const std::string& filePath);
	void loadLawMapperRules(const std::string& filePath);
	void loadLawDefinitions(const commonItems::ModFilesystem& modFS);
	void loadDiplomaticMapperRules(const std::string& filePath);
	void loadCharacterTraitMapperRules(const std::string& filePath);
	void loadColonialTagMapperRules(const std::string& filePath);
	void loadCountryTierMapperRules(const std::string& filePath);
	void loadIGIdeologiesMapperRules(const std::string& filePath);
	void loadMajorFormables(const std::string& filePath);
	void loadAIStrategies(const std::string& filePath);
	void loadAISecretGoals(const std::string& filePath);
	void importEU4Countries(const std::map<std::string, std::shared_ptr<EU4::Country>>& eu4Countries);
	void generateDecentralizedCountries(const ClayManager& clayManager, const PopManager& popManager);
	void convertAllCountries(const ClayManager& clayManager,
		 mappers::CultureMapper& cultureMapper,
		 const mappers::ReligionMapper& religionMapper,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader,
		 const LocalizationLoader& v3LocLoader,
		 const EU4::EU4LocalizationLoader& eu4LocLoader,
		 bool downTiers,
		 bool vn = false) const;
	void addCountry(const std::pair<std::string, std::shared_ptr<Country>>& country) { countries.emplace(country); }

	[[nodiscard]] const auto& getCountries() const { return countries; }
	[[nodiscard]] std::shared_ptr<Country> getCountry(const std::string& v3Tag) const;
	[[nodiscard]] int getWorldPopCount() const;
	[[nodiscard]] static int getCountriesPopCount(std::vector<std::shared_ptr<Country>> theCountries);
	[[nodiscard]] bool isTagDecentralized(const std::string& v3Tag) const;
	[[nodiscard]] std::map<std::string, Law> getLawsMap() const { return lawMapper.getLaws(); }
	[[nodiscard]] const auto& getMajorFormables() const { return majorFormablesLoader.getMajorFormables(); }

	void determineAndApplyWesternization(const mappers::CultureMapper& cultureMapper,
		 const mappers::ReligionMapper& religionMapper,
		 Configuration::EUROCENTRISM eurocentrism,
		 Configuration::STARTDATE startDate,
		 const DatingData& datingData);
	void setupTech();
	void setupLaws();
	void setupInstitutions(const std::shared_ptr<Country>& country) const;
	void convertDiplomacy(const std::vector<EU4::EU4Agreement>& agreements);
	[[nodiscard]] const auto& getAgreements() const { return agreements; }
	void convertRivals();
	void convertTruces(const date& lastEU4Date);
	void convertCharacters(const date& conversionDate,
		 Configuration::STARTDATE startDate,
		 const ClayManager& clayManager,
		 mappers::CultureMapper& cultureMapper,
		 const mappers::ReligionMapper& religionMapper,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader);
	void injectDynamicCulturesIntoFormables(const mappers::CultureMapper& cultureMapper);
	void expandReleasablesFootprint(const ClayManager& clayManager);
	void alterReleasablesReligion();
	void alterMajorFormables();
	void alterIGIdeologies(const mappers::CultureMapper& cultureMapper, const mappers::ReligionMapper& religionMapper, const ClayManager& clayManager);
	void generateAIStrategies(const ClayManager& clayManager);
	void generateAISecretGoals(const ClayManager& clayManager);
	void attemptColonialTagReplacement(const mappers::ColonialRegionMapper& colonialRegionMapper, const ClayManager& clayManager);
	void incorporateStates(const mappers::CultureMapper& cultureMapper, const ClayManager& clayManager);
	void designateTreatyPorts(const ClayManager& clayManager);
	void distributeColonialClaims(const ClayManager& clayManager);

	// VN specifics
	void importVNColonialDiplomacy(const ClayManager& clayManager);
	void importVanillaDiplomacy();

  private:
	void generateDecentralizedCountry(const std::string& culture, const std::vector<std::shared_ptr<SubState>>& subStates);
	static CulturalSubStates sortSubStatesByCultures(const ClayManager& clayManager, const PopManager& popManager);
	static std::string getDominantDemographic(const std::vector<Demographic>& demographics);
	void grantLawFromGroup(const std::string& lawGroup, const std::shared_ptr<Country>& country) const;
	[[nodiscard]] bool isEU4CountryConvertedAndLanded(const std::string& eu4Tag) const;
	[[nodiscard]] bool isVanillaCountryAndLanded(const std::string& tag) const;
	[[nodiscard]] bool isValidForColonialReplacement(const std::string& tag) const;
	void changeTag(const std::string& replacement, const std::string& tag);

	std::map<std::string, std::shared_ptr<Country>> countries;
	std::vector<Agreement> agreements;

	std::shared_ptr<mappers::CountryMapper> countryMapper;
	mappers::PopulationSetupMapper populationSetupMapper;
	mappers::IdeaEffectsMapper ideaEffectMapper;
	mappers::TechSetupMapper techSetupMapper;
	mappers::LawMapper lawMapper;
	mappers::DiplomaticMapper diplomaticMapper;
	mappers::CharacterTraitMapper characterTraitMapper;
	mappers::ColonialTagMapper colonialTagMapper;
	mappers::CountryTierMapper countryTierMapper;
	mappers::IGIdeologiesMapper igIdeologiesMapper;
	mappers::AIStrategyMapper aiStrategyMapper;
	mappers::AISecretGoalMapper aiSecretGoalMapper;
	VanillaDiplomacyLoader vanillaDiplomacyLoader;
	VanillaCountryHistoryLoader vanillaCountryHistoryLoader;
	VanillaPopulationHistoryLoader vanillaPopulationHistoryLoader;
	VanillaCharacterLoader vanillaCharacterLoader;
	VanillaDiplomaticPlayLoader vanillaDiplomaticPlayLoader;
	MajorFormablesLoader majorFormablesLoader;
};
} // namespace V3
#endif // POLITICAL_MANAGER_H