#ifndef V3_COUNTRY_H
#define V3_COUNTRY_H
#include "Color.h"
#include "Parser.h"
#include <memory>
#include <string>

namespace EU4
{
class Country;
class EU4LocalizationLoader;
} // namespace EU4
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
	void setIndustryFactor(int theIndustryFactor) { industryFactor = theIndustryFactor; }
	void setIndustryScore(int theIndustryScore) { industryScore = theIndustryScore; }
	void setCPBudget(int theBudget) { CPBudget = theBudget; }
	void setTag(const std::string& theTag) { tag = theTag; }
	void setSourceCountry(const std::shared_ptr<EU4::Country>& theCountry) { sourceCountry = theCountry; }

	void convertFromEU4Country(const ClayManager& clayManager);
	void copyVanillaData(const LocalizationLoader& v3LocLoader, const EU4::EU4LocalizationLoader& eu4LocLoader);
	void generateDecentralizedData(const ClayManager& clayManager, const LocalizationLoader& v3LocLoader, const EU4::EU4LocalizationLoader& eu4LocLoader);

	[[nodiscard]] const auto& getTag() const { return tag; }
	[[nodiscard]] const auto& getVanillaData() const { return vanillaData; }
	[[nodiscard]] const auto& getProcessedData() const { return processedData; }
	[[nodiscard]] const auto& getIndustryFactor() const { return industryFactor; }
	[[nodiscard]] const auto& getIndustryScore() const { return industryScore; }
	[[nodiscard]] const auto& getSourceCountry() const { return sourceCountry; }
	[[nodiscard]] const auto& getSubStates() const { return substates; }
	void addSubState(const std::shared_ptr<SubState>& subState) { substates.push_back(subState); }
	void setSubStates(const std::vector<std::shared_ptr<SubState>>& subStates) { substates = subStates; }
	void setProcessedData(const ProcessedData& data) { processedData = data; }

	[[nodiscard]] std::string getName(const std::string& language) const;
	[[nodiscard]] std::string getAdjective(const std::string& language) const;
	[[nodiscard]] int getPopCount() const;


	// TODO(Gawquon): Implement, maximum infrastructure that can be created by population according to technology
	[[nodiscard]] int getTechInfraCap() const { return 0; }
	// TODO(Gawquon): Implement, multiplier for amount of infrastructure created by population
	[[nodiscard]] double getTechInfraMult() const { return 0.0; }

  private:
	void registerKeys();

	void generateDecentralizedLocs(const LocalizationLoader& v3LocLoader, const EU4::EU4LocalizationLoader& eu4LocLoader);

	std::string tag;
	std::optional<VanillaCommonCountryData> vanillaData;
	ProcessedData processedData;

	double industryFactor = 0; // Modifier set by EuroCentrism or calculated by dev
	double industryScore = 0;	// Share of global industry a country has, not normalized
	int CPBudget = 0;				// Construction Points for a country to spend on it's development

	std::shared_ptr<EU4::Country> sourceCountry;
	std::vector<std::shared_ptr<SubState>> substates;
};
} // namespace V3

#endif // V3_COUNTRY_H
