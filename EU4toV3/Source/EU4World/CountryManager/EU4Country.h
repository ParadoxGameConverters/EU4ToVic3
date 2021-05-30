#ifndef EU4_COUNTRY_H
#define EU4_COUNTRY_H
#include "CountryArmy/EU4Army.h"
#include "CountryLeader/EU4Leader.h"
#include "../ProvinceManager/EU4Province.h"
#include "CountryRelations/EU4RelationDetails.h"
#include "NationalSymbol/EU4NationalSymbol.h"
#include "GameVersion.h"
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

	void readFromCommonCountry(const std::string& fileName,
		 const std::string& fullFileName); // Add any additional information available from the specified country file.
	void setLocalizationName(const std::string& language, const std::string& incName);
	void setLocalizationAdjective(const std::string& language, const std::string& incAdjective);
	void addProvince(const std::shared_ptr<Province>& province);
	void addCore(const std::shared_ptr<Province>& core);
	void setInHRE(const bool _inHRE) { inHRE = _inHRE; }
	void setEmperor(const bool _emperor) { holyRomanEmperor = _emperor; }
	void setCelestialEmperor(const bool _celestialEmperor) { celestialEmperor = _celestialEmperor; }
	void eatCountry(const std::shared_ptr<Country>& target);
	void setColonialRegion(const std::string& region) { colonialRegion = region; }
	void takeArmies(const std::shared_ptr<Country>& target);
	void clearArmies();
	void viveLaRevolution(const bool revolting) { revolutionary = revolting; }
	void setTag(const std::string& _tag) { tag = _tag; }
	void filterLeaders();
	void updateRegimentTypes(const UnitTypeLoader& unitTypeLoader);
	void buildManufactoryCount(const std::map<std::string, std::shared_ptr<Country>>& theCountries);
	void increaseMfgTransfer(const int increase) { mfgTransfer += increase; }

	[[nodiscard]] const auto& getTag() const { return tag; }
	[[nodiscard]] auto getCapital() const { return capital; }
	[[nodiscard]] auto getInHRE() const { return inHRE; }
	[[nodiscard]] auto getHolyRomanEmperor() const { return holyRomanEmperor; }
	[[nodiscard]] auto getCelestialEmperor() const { return celestialEmperor; }
	[[nodiscard]] const auto& getTechGroup() const { return techGroup; }
	[[nodiscard]] auto getIsolationism() const { return isolationism; }
	[[nodiscard]] const auto& getPrimaryCulture() const { return primaryCulture; }
	[[nodiscard]] const auto& getReligion() const { return religion; }
	[[nodiscard]] auto getScore() const { return score; }
	[[nodiscard]] auto getStability() const { return stability; }
	[[nodiscard]] auto getAverageAutonomy() const { return averageAutonomy; }
	[[nodiscard]] auto getAdmTech() const { return admTech; }
	[[nodiscard]] auto getDipTech() const { return dipTech; }
	[[nodiscard]] auto getMilTech() const { return milTech; }
	[[nodiscard]] auto getFlags() const { return flags; }
	[[nodiscard]] auto getPossibleDaimyo() const { return possibleDaimyo; }
	[[nodiscard]] auto getPossibleShogun() const { return possibleShogun; }
	[[nodiscard]] const auto& getGovernment() const { return government; }
	[[nodiscard]] auto isCustom() const { return customNation; }
	[[nodiscard]] auto isColony() const { return colony; }
	[[nodiscard]] auto getLibertyDesire() const { return libertyDesire; }
	[[nodiscard]] auto isRevolutionary() const { return revolutionary; }
	[[nodiscard]] auto getManufactoryCount() const { return mfgCount + mfgTransfer; }
	[[nodiscard]] const auto& getRandomName() const { return randomName; }
	[[nodiscard]] const auto& getName() const { return name; }

	[[nodiscard]] const auto& getReforms() const { return governmentReforms; }
	[[nodiscard]] const auto& getRelations() const { return relations; }
	[[nodiscard]] const auto& getArmies() const { return armies; }
	[[nodiscard]] const auto& getColonialRegion() const { return colonialRegion; }
	[[nodiscard]] const auto& getNationalIdeas() const { return nationalIdeas; }
	[[nodiscard]] const auto& getHistoricalLeaders() const { return historicalLeaders; }
	[[nodiscard]] const auto& getMilitaryLeaders() const { return militaryLeaders; }
	[[nodiscard]] const auto& getHistoricalEntry() const { return historicalEntry; }
	[[nodiscard]] const auto& getNationalColors() const { return nationalColors; }
	[[nodiscard]] const auto& getEmbracedInstitutions() const { return embracedInstitutions; }
	[[nodiscard]] const auto& getProvinces() const { return provinces; }
	[[nodiscard]] const auto& getCores() const { return cores; }
	[[nodiscard]] const auto& exportNationalIdeas() const { return nationalIdeas; }

	[[nodiscard]] std::string getName(const std::string& language) const;
	[[nodiscard]] std::string getAdjective(const std::string& language) const;
	[[nodiscard]] auto getLegitimacy() const { return legitimacyEquivalent; }
	[[nodiscard]] bool cultureSurvivesInCores(const std::map<std::string, std::shared_ptr<Country>>& theCountries);
	[[nodiscard]] double getCountryWeight() const;
	[[nodiscard]] bool hasModifier(const std::string&) const;
	[[nodiscard]] bool hasNationalIdea(const std::string&) const;
	[[nodiscard]] bool hasFlag(const std::string&) const;
	[[nodiscard]] bool hasReform(const std::string&) const;
	[[nodiscard]] int numEmbracedInstitutions() const;
	[[nodiscard]] double getManufactoryDensity() const;
	[[nodiscard]] const auto& getPolicies() const { return policies; }
	[[nodiscard]] double getAbsolutism() const { return absolutism; }
	[[nodiscard]] double getArmyTradition() const { return armyTradition; }
	[[nodiscard]] double getNavyTradition() const { return navyTradition; }
	[[nodiscard]] double getArmyProfessionalism() const { return armyProfessionalism; }

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
	int capital = 0; // provinceID
	std::string techGroup;
	std::vector<bool> embracedInstitutions;
	int isolationism = 1; // the isolationism of the country (for Shinto nations with Mandate of Heaven)
	std::string primaryCulture;
	std::string historicalPrimaryCulture;
	std::string religion; // the accepted religion of this country
	std::string historicalReligion;
	double score = 0.0;
	double admTech = 0.0; // the admin tech of this nation
	double dipTech = 0.0; // the diplo tech of this nation
	double milTech = 0.0; // the mil tech of this nation

	std::set<std::string> flags;
	std::set<std::string> modifiers;
	bool possibleDaimyo = false;
	bool possibleShogun = false;
	std::vector<Leader> historicalLeaders; // Historical leader information
	std::set<int> activeLeaderIDs;
	std::vector<Leader> militaryLeaders; // filtered active leaders
	std::string government = "monarchy";
	std::map<std::string, EU4RelationDetails> relations;
	std::vector<EU4Army> armies; // and navies
	std::set<std::string> nationalIdeas;
	double legitimacyEquivalent = 100; // country at 100 doesn't store the value at all for any of the legitimacy-type values
	double stability = 0;
	double averageAutonomy = 0;
	bool customNation = false; // whether or not this is a custom or random nation
	bool colony = false;
	std::string overlord;
	std::string colonialRegion;
	double libertyDesire = 0.0;
	std::string randomName; // name of nation in Random World (for old games), name of custom nation (for newer), not used for 1.30+.
	bool revolutionary = false;
	std::set<std::string> governmentReforms;
	int mfgCount = 0;
	int mfgTransfer = 0;
	std::set<std::string> policies;
	double absolutism = 0;
	double armyTradition = 0;
	double navyTradition = 0;
	double armyProfessionalism = 0;

	// Localization attributes
	std::string name; // the name of this country - used to export to disk as name.txt, must be normalized!
	std::string adjective;

	// Colors
	NationalSymbol nationalColors;

	std::map<std::string, std::string> namesByLanguage;		// language, name
	std::map<std::string, std::string> adjectivesByLanguage; // language, adj

	HistoricalEntry historicalEntry;
};
} // namespace EU4

#endif // EU4_COUNTRY_H
