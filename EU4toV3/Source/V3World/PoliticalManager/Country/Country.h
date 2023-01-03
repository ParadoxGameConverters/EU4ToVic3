#ifndef V3_COUNTRY_H
#define V3_COUNTRY_H
#include "Color.h"
#include "Configuration.h"
#include "DatingData.h"
#include "Parser.h"
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
} // namespace mappers
namespace V3
{
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
	double literacy = 0;
	double civLevel = 0;
	bool westernized = false;
	double industryFactor = 1.0; // Modifier set by EuroCentrism or calculated by dev

	double industryWeight = 0; // Share of global industry a country should get, not normalized
	int CPBudget = 0;				// Construction Points for a country to spend on it's development

	std::map<std::string, std::vector<std::string>> productionMethods; // Non-default production methods used by country, Building -> PMs

	std::string name;
	std::string adjective;
	std::map<std::string, std::string> namesByLanguage;		// language, name
	std::map<std::string, std::string> adjectivesByLanguage; // language, adj
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
	void setSourceCountry(const std::shared_ptr<EU4::Country>& theCountry) { sourceCountry = theCountry; }

	void convertFromEU4Country(const ClayManager& clayManager,
		 mappers::CultureMapper& cultureMapper,
		 const mappers::ReligionMapper& religionMapper,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader);
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
	[[nodiscard]] static int getPopCount(std::vector<std::shared_ptr<SubState>> theSubstates);


	void determineWesternizationWealthAndLiteracy(double topTech,
		 double topInstitutions,
		 const mappers::CultureMapper& cultureMapper,
		 const mappers::ReligionMapper& religionMapper,
		 Configuration::EUROCENTRISM eurocentrism,
		 const DatingData& datingData,
		 const mappers::PopulationSetupMapper& populationSetupMapper);

	// TODO(Gawquon): Implement, maximum infrastructure that can be created by population according to technology
	[[nodiscard]] int getTechInfraCap() const { return 0; }
	// TODO(Gawquon): Implement, multiplier for amount of infrastructure created by population
	[[nodiscard]] double getTechInfraMult() const { return 0.0; }
	[[nodiscard]] bool isTechLocked(const std::vector<std::string>& techs) const { return false; }

	void distributeGovAdmins(int numGovAdmins) const;
	[[nodiscard]] std::vector<std::shared_ptr<SubState>> topPercentileStatesByPop(double percentile) const;
	[[nodiscard]] double calculateBureaucracyUsage() const;

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

	std::string tag;
	std::optional<VanillaCommonCountryData> vanillaData;
	ProcessedData processedData;


	std::shared_ptr<EU4::Country> sourceCountry;
	std::vector<std::shared_ptr<SubState>> substates;
};
} // namespace V3

#endif // V3_COUNTRY_H
