#ifndef EU4_COUNTRY_H
#define EU4_COUNTRY_H
#include "../ProvinceManager/EU4Province.h"
#include "CountryArmy/EU4Army.h"
#include "CountryLeader/EU4Leader.h"
#include "CountryRelations/EU4RelationDetails.h"
#include "NationalSymbol/EU4NationalSymbol.h"
#include "Parser.h"
#include <memory>
#include <set>

namespace EU4
{
// Export data for hoi4.
struct HistoricalEntry
{
	std::string lastDynasty;
	bool monarchy = true;
};

class Country: commonItems::parser
{
  public:
	Country() = default;
	Country(std::string countryTag, std::istream& theStream);

	[[nodiscard]] const auto& getTag() const { return tag; }
	[[nodiscard]] const auto& getProvinces() const { return provinces; }
	[[nodiscard]] const auto& getCores() const { return cores; }
	void addProvince(const std::shared_ptr<Province>& province);
	void addCore(const std::shared_ptr<Province>& core);

	[[nodiscard]] auto getInHRE() const { return inHRE; }
	[[nodiscard]] auto getHolyRomanEmperor() const { return holyRomanEmperor; }
	[[nodiscard]] auto getCelestialEmperor() const { return celestialEmperor; }
	[[nodiscard]] auto isRevolutionary() const { return revolutionary; }
	void setInHRE(const bool _inHRE) { inHRE = _inHRE; }
	void setCelestialEmperor(const bool _celestialEmperor) { celestialEmperor = _celestialEmperor; }
	void setEmperor(const bool _emperor) { holyRomanEmperor = _emperor; }
	void setRevolutionary(const bool _rev) { revolutionary = _rev; }

	[[nodiscard]] auto getCapital() const { return capital; }
	[[nodiscard]] auto getScore() const { return score; }
	[[nodiscard]] auto getAdmTech() const { return admTech; }
	[[nodiscard]] auto getDipTech() const { return dipTech; }
	[[nodiscard]] auto getMilTech() const { return milTech; }

	[[nodiscard]] const auto& getTechGroup() const { return techGroup; }
	[[nodiscard]] const auto& getPrimaryCulture() const { return primaryCulture; }
	[[nodiscard]] const auto& getReligion() const { return religion; }

	[[nodiscard]] auto getIsolationism() const { return isolationism; }
	[[nodiscard]] auto getPossibleDaimyo() const { return possibleDaimyo; }
	[[nodiscard]] auto getPossibleShogun() const { return possibleShogun; }

	[[nodiscard]] auto getFlags() const { return flags; }
	[[nodiscard]] bool hasFlag(const std::string&) const;
	[[nodiscard]] bool hasModifier(const std::string&) const;

	void filterLeaders();
	[[nodiscard]] const auto& getHistoricalLeaders() const { return historicalLeaders; }
	[[nodiscard]] const auto& getMilitaryLeaders() const { return militaryLeaders; }
	[[nodiscard]] const auto& getArmies() const { return armies; }

	[[nodiscard]] const auto& getGovernment() const { return government; }
	[[nodiscard]] const auto& getRelations() const { return relations; }
	[[nodiscard]] bool hasNationalIdea(const std::string&) const;
	[[nodiscard]] const auto& getNationalIdeas() const { return nationalIdeas; }

	[[nodiscard]] auto getLegitimacy() const { return legitimacyEquivalent; }
	[[nodiscard]] auto getStability() const { return stability; }
	[[nodiscard]] auto getAverageAutonomy() const { return averageAutonomy; }
	[[nodiscard]] auto getAbsolutism() const { return absolutism; }
	[[nodiscard]] auto getArmyTradition() const { return armyTradition; }
	[[nodiscard]] auto getNavyTradition() const { return navyTradition; }
	[[nodiscard]] auto getArmyProfessionalism() const { return armyProfessionalism; }

	[[nodiscard]] auto isColony() const { return colony; }
	[[nodiscard]] const auto& getOverLord() const { return overlord; }
	[[nodiscard]] auto getLibertyDesire() const { return libertyDesire; }

	[[nodiscard]] const auto& getReforms() const { return governmentReforms; }
	[[nodiscard]] bool hasReform(const std::string&) const;
	[[nodiscard]] const auto& getPolicies() const { return policies; }
	[[nodiscard]] const auto& getEmbracedInstitutions() const { return embracedInstitutions; }
	[[nodiscard]] int numEmbracedInstitutions() const;

	[[nodiscard]] auto getManufactoryCount() const { return mfgCount + mfgTransfer; }
	void buildManufactoryCount(const std::map<std::string, std::shared_ptr<Country>>& theCountries);
	void increaseMfgTransfer(const int increase) { mfgTransfer += increase; }
	[[nodiscard]] double getManufactoryDensity() const;

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] std::string getName(const std::string& language) const;
	[[nodiscard]] std::string getAdjective(const std::string& language) const;
	void setLocalizationName(const std::string& language, const std::string& incName);
	void setLocalizationAdjective(const std::string& language, const std::string& incAdjective);

	[[nodiscard]] const auto& getNationalColors() const { return nationalColors; }

	[[nodiscard]] const auto& getHistoricalEntry() const { return historicalEntry; }

	// processing
	[[nodiscard]] bool cultureSurvivesInCores(const std::map<std::string, std::shared_ptr<Country>>& theCountries);
	[[nodiscard]] double getCountryWeight() const;
	void updateRegimentTypes(const UnitTypeLoader& unitTypeLoader);
	void eatCountry(const std::shared_ptr<Country>& target);
	void takeArmies(const std::shared_ptr<Country>& target);
	void clearArmies();

  private:
	void registerKeys();

	void determineJapaneseRelations();
	void clearProvinces();
	void clearCores();

	std::string tag;
	std::vector<std::shared_ptr<Province>> provinces;
	std::vector<std::shared_ptr<Province>> cores;

	bool inHRE = false;
	bool holyRomanEmperor = false;
	bool celestialEmperor = false;
	bool revolutionary = true;

	int capital = 0; // provinceID
	double score = 0.0;
	double admTech = 0.0;
	double dipTech = 0.0;
	double milTech = 0.0;

	std::string techGroup;
	std::string primaryCulture;
	std::string religion;
	std::string historicalPrimaryCulture; // we use these 2 when we're lacking a proper entry.
	std::string historicalReligion;		  // we use these 2 when we're lacking a proper entry.

	int isolationism = 1; // the isolationism of the country (for Shinto nations with Mandate of Heaven)
	bool possibleDaimyo = false;
	bool possibleShogun = false;

	std::set<std::string> flags;
	std::set<std::string> modifiers;

	std::set<int> activeLeaderIDs;
	std::vector<Leader> historicalLeaders; // Historical leader information
	std::vector<Leader> militaryLeaders;	// filtered active leaders
	std::vector<EU4Army> armies;				// and navies

	std::string government = "monarchy";
	std::map<std::string, EU4RelationDetails> relations;
	std::set<std::string> nationalIdeas;

	double legitimacyEquivalent = 100; // country at 100 doesn't store the value at all for any of the legitimacy-type values
	double stability = 0;
	double averageAutonomy = 0;
	double absolutism = 0;
	double armyTradition = 0;
	double navyTradition = 0;
	double armyProfessionalism = 0;

	bool colony = false;
	std::string overlord;
	double libertyDesire = 0.0;

	std::set<std::string> governmentReforms;
	std::set<std::string> policies;
	std::vector<bool> embracedInstitutions;

	int mfgCount = 0;
	int mfgTransfer = 0;

	// Localization attributes - unsure what we're using yet.
	std::string name;
	std::string adjective;
	std::map<std::string, std::string> namesByLanguage;		// language, name
	std::map<std::string, std::string> adjectivesByLanguage; // language, adj

	// Colors
	NationalSymbol nationalColors;

	// Botanical Expedition
	HistoricalEntry historicalEntry;
};
} // namespace EU4

#endif // EU4_COUNTRY_H
