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
class Country: commonItems::parser
{
  public:
	Country() = default;
	Country(std::string countryTag, std::istream& theStream);

	// Tag and links
	[[nodiscard]] const auto& getTag() const { return tag; }
	void setTag(const auto& theTag) { tag = theTag; }
	[[nodiscard]] const auto& getProvinces() const { return provinces; }
	[[nodiscard]] const auto& getCores() const { return cores; }
	void addProvince(const std::shared_ptr<Province>& province);
	void removeProvince(int provinceID);
	void addCore(const std::shared_ptr<Province>& core);

	// statuses
	[[nodiscard]] auto isInHRE() const { return inHRE; }
	[[nodiscard]] auto isHREmperor() const { return holyRomanEmperor; }
	[[nodiscard]] auto isCelestialEmperor() const { return celestialEmperor; }
	[[nodiscard]] auto isRevolutionary() const { return revolutionary; }
	[[nodiscard]] auto isGP() const { return GP; }
	void setInHRE(const bool _inHRE) { inHRE = _inHRE; }
	void setCelestialEmperor(const bool _celestialEmperor) { celestialEmperor = _celestialEmperor; }
	void setEmperor(const bool _emperor) { holyRomanEmperor = _emperor; }
	void setRevolutionary(const bool _rev) { revolutionary = _rev; }
	void setGP() { GP = true; }

	// in-game capital, score and tech
	[[nodiscard]] auto getCapital() const { return capital; }
	[[nodiscard]] auto getScore() const { return score; }
	[[nodiscard]] auto getAdmTech() const { return admTech; }
	void setAdmTech(double tech) { admTech = tech; }
	[[nodiscard]] auto getDipTech() const { return dipTech; }
	void setDipTech(double tech) { dipTech = tech; }
	[[nodiscard]] auto getMilTech() const { return milTech; }
	void setMilTech(double tech) { milTech = tech; }
	[[nodiscard]] const auto& getTechGroup() const { return techGroup; }
	void setTechGroup(const std::string& group) { techGroup = group; }
	[[nodiscard]] double getAverageDevelopment() const;

	// culture religion
	[[nodiscard]] const auto& getPrimaryCulture() const { return primaryCulture; }
	void setPrimaryCulture(const std::string& culture) { primaryCulture = culture; }
	[[nodiscard]] const auto& getReligion() const { return religion; }
	void setReligion(const std::string& rel) { religion = rel; }

	// japan
	[[nodiscard]] auto getIsolationism() const { return isolationism; }
	[[nodiscard]] auto isPossibleDaimyo() const { return possibleDaimyo; }
	[[nodiscard]] auto isPossibleShogun() const { return possibleShogun; }

	// flags and variables
	[[nodiscard]] auto getFlags() const { return flags; }
	void addFlag(const std::string& theFlag) { flags.emplace(theFlag); }
	void setFlags(const std::set<std::string>& fla) { flags = fla; }
	[[nodiscard]] bool hasFlag(const std::string&) const;
	[[nodiscard]] auto getModifiers() const { return modifiers; }
	void setModifiers(const std::set<std::string>& mods) { modifiers = mods; }
	[[nodiscard]] bool hasModifier(const std::string&) const;
	[[nodiscard]] bool isHuman() const { return isHuman; }

	// characters and armies
	[[nodiscard]] const auto& getCharacters() const { return filteredCharacters; }
	[[nodiscard]] const auto& getArmies() const { return armies; }

	// relations
	[[nodiscard]] const auto& getRelations() const { return relations; }
	void addRelation(const std::string& targetTag, const EU4RelationDetails& relation) { relations.emplace(targetTag, relation); }
	[[nodiscard]] const auto& getRivals() const { return rivals; }

	// country stats
	[[nodiscard]] auto getLegitimacy() const { return legitimacyEquivalent; }
	[[nodiscard]] auto getStability() const { return stability; }
	[[nodiscard]] auto getAverageAutonomy() const { return averageAutonomy; }
	[[nodiscard]] auto getAbsolutism() const { return absolutism; }
	[[nodiscard]] auto getArmyTradition() const { return armyTradition; }
	[[nodiscard]] auto getNavyTradition() const { return navyTradition; }
	[[nodiscard]] auto getArmyProfessionalism() const { return armyProfessionalism; }

	// dependent stuff
	[[nodiscard]] auto isColony() const { return colony; }
	[[nodiscard]] auto isTradeCompany() const { return tradeCompany; }
	void setTradeCompany() { tradeCompany = true; }
	[[nodiscard]] const auto& getOverLord() const { return overlord; }
	void setOverLord(const std::string& overLordTag) { overlord = overLordTag; }
	[[nodiscard]] auto getLibertyDesire() const { return libertyDesire; }

	// government, reforms, ideas, institutions
	[[nodiscard]] const auto& getGovernment() const { return government; }
	[[nodiscard]] auto getGovernmentRank() const { return governmentRank; }
	void setGovernmentRank(int rank) { governmentRank = rank; }
	[[nodiscard]] const auto& getReforms() const { return governmentReforms; }
	[[nodiscard]] bool hasReform(const std::string&) const;
	void addReform(const std::string& theReform) { governmentReforms.emplace(theReform); }
	void setReforms(const std::set<std::string>& forms) { governmentReforms = forms; }
	[[nodiscard]] const auto& getPolicies() const { return policies; }
	[[nodiscard]] const auto& getEmbracedInstitutions() const { return embracedInstitutions; }
	void setEmbracedInstitutions(const std::vector<bool>& institutions) { embracedInstitutions = institutions; }
	[[nodiscard]] int getNumEmbracedInstitutions() const;
	[[nodiscard]] bool hasNationalIdea(const std::string&) const;
	[[nodiscard]] const auto& getNationalIdeas() const { return nationalIdeas; }
	void setNationalIdeas(const std::set<std::string>& ideas) { nationalIdeas = ideas; }

	// manufactories
	[[nodiscard]] auto getManufactoryCount() const { return mfgCount + mfgTransfer; }
	void buildManufactoryCount(const std::map<std::string, std::shared_ptr<Country>>& theCountries);
	[[nodiscard]] double getManufactoryDensity() const;

	// localizations
	[[nodiscard]] std::string getName(const std::string& language) const;
	[[nodiscard]] std::string getName() const { return name; }
	[[nodiscard]] std::string getAdjective(const std::string& language) const;
	[[nodiscard]] std::string getAdjective() const { return adjective; }
	[[nodiscard]] const auto& getAllNameLocalizations() const { return namesByLanguage; }
	[[nodiscard]] const auto& getAllAdjectiveLocalizations() const { return adjectivesByLanguage; }
	void setLocalizationName(const std::string& language, const std::string& incName);
	void setLocalizationAdjective(const std::string& language, const std::string& incAdjective);

	// colors
	[[nodiscard]] const auto& getNationalColors() const { return nationalColors; }
	void setNationalColors(const NationalSymbol& symbol) { nationalColors = symbol; }
	void setMapColor(const commonItems::Color& color) { nationalColors.setMapColor(color); }

	// processing
	[[nodiscard]] bool cultureSurvivesInCores(const std::map<std::string, std::shared_ptr<Country>>& theCountries) const;
	[[nodiscard]] double getCountryWeight() const;
	void updateRegimentTypes(const UnitTypeLoader& unitTypeLoader);
	void eatCountry(const std::shared_ptr<Country>& target);
	void filterActiveCharacters();

  private:
	void registerKeys();

	// various processing functions
	void determineJapaneseRelations();
	void clearProvinces();
	void clearCores();
	void increaseMfgTransfer(const int increase) { mfgTransfer += increase; }
	void takeArmies(const std::shared_ptr<Country>& target);
	void clearArmies();

	std::string tag;
	std::map<int, std::shared_ptr<Province>> provinces;
	std::map<int, std::shared_ptr<Province>> cores;

	bool inHRE = false;
	bool holyRomanEmperor = false;
	bool celestialEmperor = false;
	bool revolutionary = false;
	bool GP = false;

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

	int monarchID = 0;
	int heirID = 0;
	int consortID = 0;
	std::set<int> activeLeaderIDs;
	std::vector<Character> historicalCharacters; // Historical character information
	std::vector<Character> filteredCharacters;	// filtered active characters

	std::vector<EU4Army> armies; // and navies

	std::map<std::string, EU4RelationDetails> relations;
	std::set<std::string> rivals;

	double legitimacyEquivalent = 100; // country at 100 doesn't store the value at all for any of the legitimacy-type values
	double stability = 0;
	double averageAutonomy = 0;
	double absolutism = 0;
	double armyTradition = 0;
	double navyTradition = 0;
	double armyProfessionalism = 0;

	bool colony = false;
	bool tradeCompany = false;
	bool isHuman = false;
	std::string overlord;
	double libertyDesire = 0.0;

	std::string government = "monarchy";
	int governmentRank = 0;
	std::set<std::string> governmentReforms;
	std::set<std::string> policies;
	std::vector<bool> embracedInstitutions;
	std::set<std::string> nationalIdeas;

	int mfgCount = 0;
	int mfgTransfer = 0;

	// Localization attributes - unsure what we're using yet.
	std::string name;
	std::string adjective;
	std::map<std::string, std::string> namesByLanguage;		// language, name
	std::map<std::string, std::string> adjectivesByLanguage; // language, adj

	// Colors
	NationalSymbol nationalColors;
};
} // namespace EU4

#endif // EU4_COUNTRY_H
