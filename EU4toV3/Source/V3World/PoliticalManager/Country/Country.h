#ifndef V3_COUNTRY_H
#define V3_COUNTRY_H
#include "Character.h"
#include "Color.h"
#include "Configuration.h"
#include "CountryManager/NationalSymbol/EU4CustomColors.h"
#include "DatingData.h"
#include "FlagCrafter/FlagCrafter.h"
#include "IdeaEffectsMapper/IdeaEffectsMapper.h"
#include "Parser.h"
#include "PoliticalManager/Diplomacy/Relation.h"
#include <memory>
#include <string>

namespace EU4
{
class Country;
class EU4LocalizationLoader;
class CultureLoader;
class ReligionLoader;
} // namespace EU4
namespace mappers
{
class ReligionMapper;
class CultureMapper;
class PopulationSetupMapper;
class TechSetupMapper;
class CharacterTraitMapper;
} // namespace mappers
namespace V3
{
struct Law;
struct VanillaCommonCountryData
{
	std::string type;
	std::string tier;
	std::set<std::string> cultures;
	std::string religion;
	std::string capitalStateName;
	std::optional<commonItems::Color> color;
	bool is_named_from_capital = false;
};

struct ProcessedData
{
	std::string type;
	std::string tier;
	std::set<std::string> cultures;
	std::string religion;
	std::string capitalStateName;
	std::optional<commonItems::Color> color;
	bool is_named_from_capital = false;
	std::set<std::string> effects;
	std::set<std::string> populationEffects;
	std::set<std::string> laws;
	std::set<std::string> institutions;
	double literacy = 0;
	double civLevel = 0;
	bool westernized = false;
	double industryFactor = 1.0; // Modifier set by EuroCentrism or calculated by dev
	mappers::IdeaEffect ideaEffect;
	std::set<std::string> techs;
	std::map<std::string, Relation> relations;
	std::set<std::string> rivals;
	std::map<std::string, int> truces;
	std::vector<Character> characters;

	double industryWeight = 0; // Share of global industry a country should get, not normalized
	int CPBudget = 0;				// Construction Points for a country to spend on it's development

	std::map<std::string, std::vector<std::string>> productionMethods; // Non-default production methods used by country, Building -> PMs

	std::string name;
	std::string adjective;
	std::map<std::string, std::string> namesByLanguage;		// language, name
	std::map<std::string, std::string> adjectivesByLanguage; // language, adj

	std::map<FlagCrafter::FLAGTYPE, std::string> flags;
	std::optional<EU4::CustomColorsBlock> customColors;		 // used for flag generation for custom flag.
	std::optional<commonItems::Color> revolutionaryColor;		 // used for flag generation for revolutionary flag.
	std::map<FlagCrafter::FLAGTYPE, std::string> customFlags; // stuff we crafted ourselves.
};

class SubState;
class ClayManager;
class LocalizationLoader;
class Country: commonItems::parser
{
  public:
	Country() = default;
	void initializeCountry(std::istream& theStream);
	void setTag(const std::string& theTag) { tag = theTag; }
	void setIndustryWeight(const double theIndustryWeight) { processedData.industryWeight = theIndustryWeight; }
	void setCPBudget(const int theBudget) { processedData.CPBudget = theBudget; }
	void setProductionMethods(const std::map<std::string, std::vector<std::string>>& thePMs) { processedData.productionMethods = thePMs; }
	void setSourceCountry(const std::shared_ptr<EU4::Country>& theCountry) { sourceCountry = theCountry; }

	void convertFromEU4Country(const ClayManager& clayManager,
		 mappers::CultureMapper& cultureMapper,
		 const mappers::ReligionMapper& religionMapper,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader,
		 const mappers::IdeaEffectsMapper& ideaEffectMapper);
	void copyVanillaData(const LocalizationLoader& v3LocLoader, const EU4::EU4LocalizationLoader& eu4LocLoader);
	void generateDecentralizedData(const LocalizationLoader& v3LocLoader, const EU4::EU4LocalizationLoader& eu4LocLoader);

	[[nodiscard]] const auto& getTag() const { return tag; }
	[[nodiscard]] const auto& getVanillaData() const { return vanillaData; }
	[[nodiscard]] const auto& getProcessedData() const { return processedData; }
	[[nodiscard]] const auto& getIndustryFactor() const { return processedData.industryFactor; }
	[[nodiscard]] const auto& getIndustryWeight() const { return processedData.industryWeight; }
	[[nodiscard]] const auto& getCPBudget() const { return processedData.CPBudget; }
	[[nodiscard]] const auto& getSourceCountry() const { return sourceCountry; }
	[[nodiscard]] const auto& getSubStates() const { return substates; }
	void addSubState(const std::shared_ptr<SubState>& theSubstate) { substates.push_back(theSubstate); }
	void setSubStates(const std::vector<std::shared_ptr<SubState>>& theSubstates) { substates = theSubstates; }
	void setProcessedData(const ProcessedData& data) { processedData = data; }

	[[nodiscard]] std::string getName(const std::string& language) const;
	[[nodiscard]] std::string getAdjective(const std::string& language) const;
	[[nodiscard]] int getPopCount() const;
	[[nodiscard]] static int getPopCount(const std::vector<std::shared_ptr<SubState>>& theSubstates);


	void determineWesternizationWealthAndLiteracy(double topTech,
		 double topInstitutions,
		 const mappers::CultureMapper& cultureMapper,
		 const mappers::ReligionMapper& religionMapper,
		 Configuration::EUROCENTRISM eurocentrism,
		 const DatingData& datingData,
		 const mappers::PopulationSetupMapper& populationSetupMapper);
	void setTechs(const mappers::TechSetupMapper& techSetupMapper, double productionScore, double militaryScore, double societyScore);
	void addLaw(const auto& lawName) { processedData.laws.emplace(lawName); }
	void addInstitution(const auto& institutionName) { processedData.institutions.emplace(institutionName); }
	[[nodiscard]] Relation& getRelation(const std::string& target);
	[[nodiscard]] const auto& getRelations() const { return processedData.relations; }
	void setRivals(const std::set<std::string>& theRivals) { processedData.rivals = theRivals; }
	[[nodiscard]] const auto& getRivals() const { return processedData.rivals; }
	void addTruce(const std::string& target, int months) { processedData.truces.emplace(target, months); }
	[[nodiscard]] const auto& getTruces() const { return processedData.truces; }

	void convertCharacters(const mappers::CharacterTraitMapper& characterTraitMapper,
		 float ageShift,
		 const ClayManager& clayManager,
		 mappers::CultureMapper& cultureMapper,
		 const mappers::ReligionMapper& religionMapper,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader,
		 const date& conversionDate);

	void setFlags(const std::map<FlagCrafter::FLAGTYPE, std::string>& flags) { processedData.flags = flags; }
	void addFlag(const FlagCrafter::FLAGTYPE flagType, const std::string& flag) { processedData.flags.emplace(flagType, flag); }
	[[nodiscard]] const auto& getFlags() const { return processedData.flags; }
	void addCustomFlag(FlagCrafter::FLAGTYPE flagType, const std::string& flagName) { processedData.customFlags.emplace(flagType, flagName); }
	[[nodiscard]] const auto& getCustomFlags() const { return processedData.customFlags; }

	// TODO(Gawquon): Implement, maximum infrastructure that can be created by population according to technology
	[[nodiscard]] int getTechInfraCap() const { return 0; }
	// TODO(Gawquon): Implement, multiplier for amount of infrastructure created by population
	[[nodiscard]] double getTechInfraMult() const { return 0.0; }
	[[nodiscard]] bool hasAnyOfTech(const std::vector<std::string>& techs) const;

	void distributeGovAdmins(int numGovAdmins) const;
	[[nodiscard]] std::vector<std::shared_ptr<SubState>> topPercentileStatesByPop(double percentile) const;
	[[nodiscard]] double calculateBureaucracyUsage(const std::map<std::string, V3::Law>& lawsMap) const;

  private:
	void registerKeys();

	void convertCapital(const ClayManager& clayManager);
	void convertReligion(const mappers::ReligionMapper& religionMapper);
	void convertCulture(const ClayManager& clayManager,
		 mappers::CultureMapper& cultureMapper,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader);
	void convertTier();
	void generateDecentralizedLocs(const LocalizationLoader& v3LocLoader, const EU4::EU4LocalizationLoader& eu4LocLoader);
	void calculateBaseLiteracy(const mappers::ReligionMapper& religionMapper);
	void calculateWesternization(double topTech, double topInstitutions, const mappers::CultureMapper& cultureMapper, Configuration::EUROCENTRISM eurocentrism);
	void adjustLiteracy(const DatingData& datingData, const mappers::CultureMapper& cultureMapper);
	[[nodiscard]] static double yearCapFactor(const date& targetDate);
	void applyLiteracyAndWealthEffects(const mappers::PopulationSetupMapper& populationSetupMapper);
	void setDecentralizedEffects();
	void determineCountryType();
	[[nodiscard]] double calcSubStateBureaucracy(const std::map<std::string, V3::Law>& lawsMap) const;
	[[nodiscard]] double calcInstitutionBureaucracy() const;
	[[nodiscard]] double calcCharacterBureaucracy() const;

	std::string tag;
	std::optional<VanillaCommonCountryData> vanillaData;
	ProcessedData processedData;


	std::shared_ptr<EU4::Country> sourceCountry;
	std::vector<std::shared_ptr<SubState>> substates;
};
} // namespace V3

#endif // V3_COUNTRY_H
