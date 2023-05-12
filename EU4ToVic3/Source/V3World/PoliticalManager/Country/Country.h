#ifndef V3_COUNTRY_H
#define V3_COUNTRY_H
#include "Character.h"
#include "Color.h"
#include "Configuration.h"
#include "CountryManager/NationalSymbol/EU4CustomColors.h"
#include "DatingData.h"
#include "EconomyManager/NationalBudget/Sector.h"
#include "FlagCrafter/FlagCrafter.h"
#include "IGIdeologiesMapper/IGIdeologiesMapping.h"
#include "IdeaEffectsMapper/IdeaEffectsMapper.h"
#include "Loaders/TechLoader/Tech.h"
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
class CountryTierMapper;
} // namespace mappers
namespace V3
{
class ClayManager;
class LocalizationLoader;
class SubState;
struct Law;
struct VanillaCommonCountryData
{
	std::string type;
	std::string vanillaType;
	std::string tier;
	std::set<std::string> cultures;
	std::string religion;
	std::string capitalStateName;
	std::optional<commonItems::Color> color;
	bool is_named_from_capital = false;
};

struct UnprocessedData
{
	std::vector<std::string> vanillaHistoryElements;
	std::vector<std::string> vanillaPopulationElements;
	std::vector<std::string> vanillaCharacterElements;
	std::vector<std::string> vanillaDiplomaticPlayElements;
};

struct ProcessedData
{
	enum class DISCRIMINATION_LEVEL
	{
		Ethnostate = 1,
		National_Supremacy = 2,
		Racial_Segregation = 3,
		Cultural_Exclusion = 4,
		Multicultural = 5,
		Unknown = 6
	};

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
	std::map<std::string, int> institutions;
	double literacy = 0;
	double civLevel = 0;
	bool westernized = false;
	double industryFactor = 1.0; // Modifier changed if EuroCentrism enabled
	mappers::IdeaEffect ideaEffect;
	std::map<std::string, mappers::IGIdeologyMod> igIdeologyModifiers;
	std::set<std::string> techs;
	std::map<std::string, Relation> relations;
	std::string overlordTag;
	std::set<std::string> rivals;
	std::map<std::string, int> truces;
	std::vector<Character> characters;
	std::vector<std::string> vanillaHistoryElements;		  // stanzas from vanilla country histories, ready for direct dump.
	std::vector<std::string> vanillaPopulationElements;	  // stanzas from vanilla population histories, ready for direct dump.
	std::vector<std::string> vanillaCharacterElements;		  // stanzas from vanilla character histories, ready for direct dump.
	std::vector<std::string> vanillaDiplomaticPlayElements; // stanzas from vanilla diplomatic play histories, ready for direct dump.

	double industryWeight = 0; // share of global industry a country should get, not normalized
	int CPBudget = 0;				// Construction Points for a country to spend on it's development
	std::map<std::string, std::shared_ptr<Sector>> industrySectors;
	double productionTechPercentile = 0;
	double perCapitaDev = 0;

	std::string name;
	std::string adjective;
	std::map<std::string, std::string> namesByLanguage;		// language, name
	std::map<std::string, std::string> adjectivesByLanguage; // language, adj

	std::map<FlagCrafter::FLAGTYPE, std::string> flags;
	std::optional<EU4::CustomColorsBlock> customColors;		 // used for flag generation for custom flag.
	std::optional<commonItems::Color> revolutionaryColor;		 // used for flag generation for revolutionary flag.
	std::map<FlagCrafter::FLAGTYPE, std::string> customFlags; // stuff we crafted ourselves.

	std::map<std::string, int> admStrategies;
	std::map<std::string, int> dipStrategies;
	std::map<std::string, int> polStrategies;
	std::map<std::string, std::string> targetSecretGoals;

	DISCRIMINATION_LEVEL discriminationLevel = DISCRIMINATION_LEVEL::Unknown;
};

class Country: commonItems::parser
{
  public:
	Country() = default;
	void initializeCountry(std::istream& theStream);
	void storeVanillaCountryType(std::istream& theStream);
	void setTag(const std::string& theTag) { tag = theTag; }
	void setIndustryWeight(const double theIndustryWeight) { processedData.industryWeight = theIndustryWeight; }
	void setCPBudget(const int theBudget) { processedData.CPBudget = theBudget; }
	void setSourceCountry(const std::shared_ptr<EU4::Country>& theCountry) { sourceCountry = theCountry; }
	void setReligion(const std::string& religion) { processedData.religion = religion; }
	void setProductionTechPercentile(const double productionScore) { processedData.productionTechPercentile = productionScore; }
	void setPerCapitaDev(const double pDev) { processedData.perCapitaDev = pDev; }

	void convertFromEU4Country(const ClayManager& clayManager,
		 mappers::CultureMapper& cultureMapper,
		 const mappers::ReligionMapper& religionMapper,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader,
		 const mappers::IdeaEffectsMapper& ideaEffectMapper,
		 const mappers::CountryTierMapper& countryTierMapper,
		 bool downTiers,
		 bool vn = false);
	void copyVanillaData(const LocalizationLoader& v3LocLoader, const EU4::EU4LocalizationLoader& eu4LocLoader, bool vn = false);
	void generateDecentralizedData(const LocalizationLoader& v3LocLoader, const EU4::EU4LocalizationLoader& eu4LocLoader);

	[[nodiscard]] const auto& getTag() const { return tag; }
	[[nodiscard]] const auto& getVanillaData() const { return vanillaData; }
	[[nodiscard]] const auto& getProcessedData() const { return processedData; }
	[[nodiscard]] const auto& getIndustryFactor() const { return processedData.industryFactor; }
	[[nodiscard]] const auto& getIndustryWeight() const { return processedData.industryWeight; }
	[[nodiscard]] const auto& getCPBudget() const { return processedData.CPBudget; }
	[[nodiscard]] const auto& getSourceCountry() const { return sourceCountry; }
	[[nodiscard]] const auto& getSubStates() const { return subStates; }
	[[nodiscard]] const auto& getUnownedCoreSubStates() const { return unownedCoreSubStates; }
	[[nodiscard]] const auto& getUnownedProvinces() const { return unownedProvinces; }
	void addSubState(const std::shared_ptr<SubState>& theSubState) { subStates.push_back(theSubState); }
	void addUnownedCoreSubState(const std::shared_ptr<SubState>& theSubState) { unownedCoreSubStates.push_back(theSubState); }
	void addUnownedProvinces(const std::set<std::string>& theProvinces) { unownedProvinces.insert(theProvinces.begin(), theProvinces.end()); }
	void setSubStates(const std::vector<std::shared_ptr<SubState>>& theSubStates) { subStates = theSubStates; }
	void setProcessedData(const ProcessedData& data) { processedData = data; }
	void setIGIdeologyModifiers(const std::map<std::string, mappers::IGIdeologyMod>& modifiers) { processedData.igIdeologyModifiers = modifiers; }

	[[nodiscard]] std::string getName(const std::string& language) const;
	[[nodiscard]] std::string getAdjective(const std::string& language) const;
	[[nodiscard]] double getTotalDev() const;
	[[nodiscard]] int getPopCount() const;
	[[nodiscard]] int getVanillaPopCount() const; // vanilla pop count of all the provinces this country holds
	[[nodiscard]] int getIncorporatedPopCount() const;
	[[nodiscard]] static int getPopCount(const std::vector<std::shared_ptr<SubState>>& theSubStates);

	void determineWesternizationWealthAndLiteracy(double topTech,
		 double topInstitutions,
		 const mappers::CultureMapper& cultureMapper,
		 const mappers::ReligionMapper& religionMapper,
		 Configuration::EUROCENTRISM eurocentrism,
		 Configuration::STARTDATE startDate,
		 const DatingData& datingData,
		 const mappers::PopulationSetupMapper& populationSetupMapper);
	void setTechs(const mappers::TechSetupMapper& techSetupMapper, double productionScore, double militaryScore, double societyScore);
	void addTech(const std::string& tech) { processedData.techs.emplace(tech); }
	void addLaw(const auto& lawName) { processedData.laws.emplace(lawName); }
	void addInstitution(const auto& institutionName, const int level = 1) { processedData.institutions.emplace(institutionName, level); }
	[[nodiscard]] Relation& getRelation(const std::string& target);
	[[nodiscard]] const auto& getRelations() const { return processedData.relations; }
	void setRivals(const std::set<std::string>& theRivals) { processedData.rivals = theRivals; }
	[[nodiscard]] const auto& getRivals() const { return processedData.rivals; }
	void addTruce(const std::string& target, int months) { processedData.truces.emplace(target, months); }
	[[nodiscard]] const auto& getTruces() const { return processedData.truces; }
	void setVanillaHistoryElements(const std::vector<std::string>& elements) { unprocessedData.vanillaHistoryElements = elements; }
	void setVanillaPopulationElements(const std::vector<std::string>& elements) { unprocessedData.vanillaPopulationElements = elements; }
	void setVanillaCharacterElements(const std::vector<std::string>& elements) { unprocessedData.vanillaCharacterElements = elements; }
	void setVanillaDiplomaticPlayElements(const std::vector<std::string>& elements) { unprocessedData.vanillaDiplomaticPlayElements = elements; }

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

	[[nodiscard]] static double yearCapFactor(const date& targetDate);
	[[nodiscard]] int getTechInfraCap(const std::map<std::string, Tech>& techMap) const;
	[[nodiscard]] double getTechInfraMult(const std::map<std::string, Tech>& techMap) const;
	[[nodiscard]] int getThroughputMax(const std::map<std::string, Tech>& techMap) const;
	[[nodiscard]] bool hasAnyOfTech(const std::set<std::string>& techs) const;
	[[nodiscard]] int getGovBuildingMax(const std::string& building,
		 const std::map<std::string, Law>& lawsMap,
		 const std::map<std::string, Tech>& techMap) const;

	void distributeGovAdmins(double target, int PMGeneration, const std::map<std::string, Tech>& techMap) const;
	[[nodiscard]] std::vector<std::shared_ptr<SubState>> topPercentileStatesByPop(double percentile) const;
	[[nodiscard]] double calculateBureaucracyUsage(const std::map<std::string, Law>& lawsMap) const;
	void addSector(const std::string& sectorName, const std::shared_ptr<Sector>& sector) { processedData.industrySectors.emplace(sectorName, sector); }

	void leaveIsolationism();

	void setAdmStrategies(const std::map<std::string, int>& strategies) { processedData.admStrategies = strategies; }
	void setDipStrategies(const std::map<std::string, int>& strategies) { processedData.dipStrategies = strategies; }
	void setPolStrategies(const std::map<std::string, int>& strategies) { processedData.polStrategies = strategies; }

	void setOverlord(const std::string& overlordTag) { processedData.overlordTag = overlordTag; }
	void addGoal(const std::string& target, const std::string& goal) { processedData.targetSecretGoals.emplace(target, goal); }

	void setDiscriminationLevel(const ProcessedData::DISCRIMINATION_LEVEL& discrimination) { processedData.discriminationLevel = discrimination; }
	[[nodiscard]] bool isCultureDiscriminated(const std::string& culture, const mappers::CultureMapper& cultureMapper) const;

  private:
	void registerKeys();
	void registerVanillaTypeKeys();

	void convertCapital(const ClayManager& clayManager);
	void convertReligion(const mappers::ReligionMapper& religionMapper);
	void convertCulture(const ClayManager& clayManager,
		 mappers::CultureMapper& cultureMapper,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader);
	void convertTier(const mappers::CountryTierMapper& countryTierMapper, bool downTiers, bool vn);
	void generateDecentralizedLocs(const LocalizationLoader& v3LocLoader, const EU4::EU4LocalizationLoader& eu4LocLoader);
	void calculateBaseLiteracy(const mappers::ReligionMapper& religionMapper);
	void calculateWesternization(double topTech, double topInstitutions, const mappers::CultureMapper& cultureMapper, Configuration::EUROCENTRISM eurocentrism);
	void adjustLiteracy(const DatingData& datingData, const mappers::CultureMapper& cultureMapper, Configuration::EUROCENTRISM eurocentrism);
	void applyLiteracyAndWealthEffects(const mappers::PopulationSetupMapper& populationSetupMapper);
	void setDecentralizedEffects();
	void determineCountryType();
	[[nodiscard]] double calcSubStateBureaucracy(const std::map<std::string, Law>& lawsMap) const;
	[[nodiscard]] double calcInstitutionBureaucracy() const;
	[[nodiscard]] double calcCharacterBureaucracy() const;
	[[nodiscard]] std::optional<Tech> getTechFromMap(const std::string& techName, const std::map<std::string, Tech>& techMap) const;

	std::string tag;
	std::optional<VanillaCommonCountryData> vanillaData;
	ProcessedData processedData;
	UnprocessedData unprocessedData;

	std::shared_ptr<EU4::Country> sourceCountry;
	std::vector<std::shared_ptr<SubState>> subStates;
	std::vector<std::shared_ptr<SubState>> unownedCoreSubStates; // substates this country has eu4 cores on but does *not* own.
	std::set<std::string> unownedProvinces; // Derived provinces we want released. Doesn't necessarily match unownedCoreSubStates! We tweak for borders!
};
} // namespace V3

#endif // V3_COUNTRY_H
