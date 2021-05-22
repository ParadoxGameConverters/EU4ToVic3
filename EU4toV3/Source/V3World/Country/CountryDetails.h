#ifndef COUNTRY_DETAILS_H
#define COUNTRY_DETAILS_H

#include "../../V3World/Party/Party.h"
#include "Parser.h"
#include <set>

namespace V2
{
struct NationalValue
{
	double order = 0;
	double liberty = 0;
	double equality = 0;
};
struct Technology
{
	double army = 0;
	double navy = 0;
	double commerce = 0;
	double industry = 0;
	double culture = 0;
};
struct Reform
{
	// These are default values for dead nations. Extant nations will override them.
	double slavery = -3;
	double upper_house_composition = -2;
	double vote_franchise = -5;
	double voting_system = -5;
	double public_meetings = -5;
	double press_rights = -5;
	double trade_unions = -5;
	double political_parties = -5;
};
struct UpperHouse
{
	double reactionary = 0;
	double liberal = 0;
};

class CountryDetails: commonItems::parser
{
  public:
	CountryDetails() = default;
	explicit CountryDetails(std::string _filename);
	explicit CountryDetails(std::istream& theStream);

	// These values are open to ease management.
	// This is a storage container for V2::Country.
	// Some of these values are loaded from files, others are adjusted on the fly.
	std::string filename;
	std::string graphicalCulture;
	std::string colorString;
	std::vector<Party> parties;
	std::string unitNames;
	std::string primaryCulture = "dummy";
	std::string eu4PrimaryCulture;
	std::set<std::string> acceptedCultures;
	std::set<std::string> eu4acceptedCultures;
	std::string religion = "shamanist";
	std::string government = "absolute_monarchy";
	bool civilized = false;
	bool isReleasableVassal = true;
	std::string nationalValue = "nv_order";
	int capital = 0;
	double prestige = 0;
	double plurality = 0;
	std::string techSchool = "traditional_academic";
	bool inHRE = false;
	bool holyRomanEmperor = false;
	bool celestialEmperor = false;
	double literacy = 0.0;
	std::string rulingParty = "conservative";
	int upperHouseReactionary = 10;
	int upperHouseConservative = 65;
	int upperHouseLiberal = 25;
	int numFactories = 0;
	double consciousness = 0;
	double nonstateConsciousness = 0;
	std::set<std::string> countryFlags;

	NationalValue nationalValues;
	Technology technologies;
	Reform reforms;
	UpperHouse upperHouses;
	double literacyInvestment = 0;

  private:
	void registerKeys();
};
} // namespace V2

#endif // COUNTRY_DETAILS_H
