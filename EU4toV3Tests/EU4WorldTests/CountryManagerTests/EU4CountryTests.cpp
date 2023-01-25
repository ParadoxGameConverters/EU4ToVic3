#include "CountryManager/EU4Country.h"
#include "CountryManager/EU4CountryManager.h"
#include "ProvinceManager/ProvinceManager.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::ElementsAre;
using testing::UnorderedElementsAre;
#include "UnitTypeLoader/UnitTypeLoader.h"

TEST(EU4World_EU4CountryTests, countryTagIsStored)
{
	std::stringstream input;
	const EU4::Country country("TAG", input);

	EXPECT_EQ("TAG", country.getTag());
}

TEST(EU4World_EU4CountryTests, ProvincesAndCoresCanBeStoredAndRetrieved)
{
	std::stringstream input;
	EU4::Country country("TAG", input);

	std::stringstream provinceStream;
	provinceStream << "-1={}\n";
	provinceStream << "-2={}\n";
	EU4::ProvinceManager provinceManager;
	provinceManager.loadProvinces(provinceStream);

	const auto& province1 = provinceManager.getProvince(1);
	const auto& province2 = provinceManager.getProvince(2);

	EXPECT_TRUE(country.getProvinces().empty());
	country.addProvince(province1);
	country.addProvince(province2);
	EXPECT_EQ(2, country.getProvinces().size());

	EXPECT_TRUE(country.getCores().empty());
	country.addCore(province1);
	country.addCore(province2);
	EXPECT_EQ(2, country.getCores().size());
}

TEST(EU4World_EU4CountryTests, StatusesCanBeToggled)
{
	std::stringstream input;
	EU4::Country country("TAG", input);

	EXPECT_FALSE(country.isInHRE());
	EXPECT_FALSE(country.isHREmperor());
	EXPECT_FALSE(country.isCelestialEmperor());
	EXPECT_FALSE(country.isRevolutionary());

	country.setInHRE(true);
	country.setEmperor(true);
	country.setCelestialEmperor(true);
	country.setRevolutionary(true);

	EXPECT_TRUE(country.isInHRE());
	EXPECT_TRUE(country.isHREmperor());
	EXPECT_TRUE(country.isCelestialEmperor());
	EXPECT_TRUE(country.isRevolutionary());
}

TEST(EU4World_EU4CountryTests, capitalScoreAndTechCanBeRead)
{
	std::stringstream deadInput;
	const EU4::Country deadCountry("GAT", deadInput);

	EXPECT_EQ(0, deadCountry.getCapital());
	EXPECT_EQ(0, deadCountry.getScore());
	EXPECT_EQ(0, deadCountry.getAdmTech());
	EXPECT_EQ(0, deadCountry.getDipTech());
	EXPECT_EQ(0, deadCountry.getMilTech());
	EXPECT_TRUE(deadCountry.getTechGroup().empty());

	std::stringstream input;
	input << "capital=9\n";
	input << "age_score={1 2 3 4}\n";
	input << "technology = {\n";
	input << "   adm_tech=1\n";
	input << "   dip_tech=2\n";
	input << "   mil_tech=3\n";
	input << "}\n";
	input << "technology_group=western\n";
	const EU4::Country country("TAG", input);

	EXPECT_EQ(9, country.getCapital());
	EXPECT_EQ(10, country.getScore());
	EXPECT_EQ(1, country.getAdmTech());
	EXPECT_EQ(2, country.getDipTech());
	EXPECT_EQ(3, country.getMilTech());
	EXPECT_EQ("western", country.getTechGroup());
}

TEST(EU4World_EU4CountryTests, RivalsCanBeRead)
{
	std::stringstream input;
	input << "rival = {\n";
	input << "   country = TA1\n";
	input << "}\n";
	input << "rival = {\n";
	input << "   country = TA2\n";
	input << "}\n";
	const EU4::Country country("TAG", input);

	EXPECT_THAT(country.getRivals(), UnorderedElementsAre("TA1", "TA2"));
}

TEST(EU4World_EU4CountryTests, cultureAndReligionCanBeRead)
{
	std::stringstream deadInput;
	const EU4::Country deadCountry("GAT", deadInput);

	EXPECT_TRUE(deadCountry.getPrimaryCulture().empty());
	EXPECT_TRUE(deadCountry.getReligion().empty());

	std::stringstream input;
	input << "primary_culture=theCulture\n";
	input << "religion=theReligion\n";
	const EU4::Country country("TAG", input);

	EXPECT_EQ("theCulture", country.getPrimaryCulture());
	EXPECT_EQ("theReligion", country.getReligion());
}

TEST(EU4World_EU4CountryTests, cultureAndReligionCanBeReadFromHistoryIfLacking)
{
	std::stringstream input;
	input << "history={\n";
	input << "	primary_culture=theCulture\n";
	input << "	religion=theReligion\n";
	input << "}\n";
	const EU4::Country country("TAG", input);

	EXPECT_EQ("theCulture", country.getPrimaryCulture());
	EXPECT_EQ("theReligion", country.getReligion());
}

TEST(EU4World_EU4CountryTests, cultureAndReligionSupercedeHistory)
{
	std::stringstream input;
	input << "primary_culture=theCulture\n";
	input << "religion=theReligion\n";
	input << "history={\n";
	input << "	primary_culture=theCulture2\n";
	input << "	religion=theReligion2\n";
	input << "}\n";
	const EU4::Country country("TAG", input);

	EXPECT_EQ("theCulture", country.getPrimaryCulture());
	EXPECT_EQ("theReligion", country.getReligion());
}

TEST(EU4World_EU4CountryTests, JapaneseAffairsCanBeSet)
{
	std::stringstream daimyoInput;
	daimyoInput << "isolationism=4\n";
	daimyoInput << "government={\n";
	daimyoInput << "	reform_stack={\n";
	daimyoInput << "		reforms={ daimyo }\n";
	daimyoInput << "	}\n";
	daimyoInput << "}\n";
	const EU4::Country daimyo("TAG", daimyoInput);

	EXPECT_EQ(4, daimyo.getIsolationism());
	EXPECT_TRUE(daimyo.isPossibleDaimyo());
	EXPECT_FALSE(daimyo.isPossibleShogun());

	std::stringstream indepDaimyoInput;
	indepDaimyoInput << "government={\n";
	indepDaimyoInput << "	reform_stack={\n";
	indepDaimyoInput << "		reforms={ indep_daimyo }\n";
	indepDaimyoInput << "	}\n";
	indepDaimyoInput << "}\n";
	const EU4::Country indepDaimyo("TAG2", indepDaimyoInput);

	EXPECT_TRUE(indepDaimyo.isPossibleDaimyo());
	EXPECT_FALSE(indepDaimyo.isPossibleShogun());

	std::stringstream shogunInput;
	shogunInput << "government={\n";
	shogunInput << "	reform_stack={\n";
	shogunInput << "		reforms={ shogunate }\n";
	shogunInput << "	}\n";
	shogunInput << "}\n";
	const EU4::Country shogun("TAG3", shogunInput);

	EXPECT_FALSE(shogun.isPossibleDaimyo());
	EXPECT_TRUE(shogun.isPossibleShogun());
}

TEST(EU4World_EU4CountryTests, FlagsAndModifiersCanBeSetAndPoked)
{
	std::stringstream input;
	input << "flags={\n";
	input << "	flag1=blah\n";
	input << "	flag2=blah\n";
	input << "}\n";
	input << "hidden_flags={\n";
	input << "	flag3=blah\n";
	input << "	flag4=blah\n";
	input << "}\n";
	input << "variables={\n";
	input << "	flag5=blah\n";
	input << "	flag6=blah\n";
	input << "}\n";
	input << "modifier={\n";
	input << "	modifier=mod1\n";
	input << "}\n";
	input << "modifier={\n";
	input << "	modifier=mod2\n";
	input << "}\n";
	const EU4::Country country("TAG", input);

	EXPECT_THAT(country.getFlags(), UnorderedElementsAre("flag1", "flag2", "flag3", "flag4", "flag5", "flag6"));
	EXPECT_TRUE(country.hasFlag("flag1"));
	EXPECT_THAT(country.getModifiers(), UnorderedElementsAre("mod1", "mod2"));
	EXPECT_TRUE(country.hasModifier("mod1"));
}

TEST(EU4World_EU4CountryTests, LeadersCanBeReadAndProcessed)
{
	std::stringstream input;
	input << "history={\n";
	input << "	1265.1.1={\n";
	input << "		leader={\n";
	input << "			name=\"Sir. Long Dead\"\n";
	input << "			id={\n";
	input << "				id=1\n";
	input << "			}\n";
	input << "		}\n";
	input << "	}\n";
	input << "	1765.1.1={\n";
	input << "		leader={\n";
	input << "			name=\"Sir. Still Alive\"\n";
	input << "			type=admiral\n";
	input << "			manuever=6\n";
	input << "			id={\n";
	input << "				id=2\n";
	input << "			}\n";
	input << "		}\n";
	input << "	}\n";
	input << "	1768.1.1={\n";
	input << "		monarch={\n";
	input << "			leader={\n";
	input << "				name=\"His Majesty Still Kicking\"\n";
	input << "				type=general\n";
	input << "				fire=4\n";
	input << "				id={\n";
	input << "					id=3\n";
	input << "				}\n";
	input << "			}\n";
	input << "		}\n";
	input << "	}\n";
	input << "}\n";
	input << "leader={\n";
	input << "	id=2\n";
	input << "}\n";
	input << "leader={\n";
	input << "	id=3\n";
	input << "}\n";

	const EU4::Country country("TAG", input);
	const auto& leaders = country.getCharacters();

	ASSERT_EQ(2, leaders.size());

	const auto& leader1 = leaders[0];
	const auto& leader2 = leaders[1];

	EXPECT_EQ("His Majesty Still Kicking", leader1.leaderName);
	EXPECT_TRUE(leader1.ruler);
	EXPECT_EQ(4, leader1.fire);

	EXPECT_EQ("Sir. Still Alive", leader2.leaderName);
	EXPECT_EQ(6, leader2.maneuver);
}

TEST(EU4World_EU4CountryTests, DuplicateLeadersAreDitched)
{
	std::stringstream input;
	input << "history={\n";
	input << "	1751.1.1={\n";
	input << "		heir={\n";
	input << "			name=\"I was heir\"\n";
	input << "			id={\n";
	input << "				id=1\n";
	input << "			}\n";
	input << "			monarch_name=\"I'll be monarch\"\n";
	input << "		}\n";
	input << "	}\n";
	input << "	1768.1.1={\n";
	input << "		monarch_heir={\n";
	input << "			name=\"Now I'm monarch\"\n";
	input << "			id={\n";
	input << "				id=1\n";
	input << "			}\n";
	input << "		}\n";
	input << "	}\n";
	input << "}\n";
	input << "monarch={\n";
	input << "	id=1\n";
	input << "}\n";

	const EU4::Country country("TAG", input);
	const auto& leaders = country.getCharacters();

	ASSERT_EQ(1, leaders.size());

	const auto& leader = leaders[0];

	EXPECT_EQ("Now I'm monarch", leader.name);
	EXPECT_TRUE(leader.ruler);
}

TEST(EU4World_EU4CountryTests, ArmiesCanBeLoaded)
{
	std::stringstream input;
	input << "army={\n";
	input << "	name = \"Armee royale\"\n";
	input << "	location = 183\n";
	input << "	regiment = {\n";
	input << "		name=\"Reims's 1st Regiment\"\n";
	input << "		home = 180\n";
	input << "		type = \"chevauchee\"\n";
	input << "	}\n";
	input << "}\n";

	const EU4::Country country("TAG", input);

	ASSERT_EQ(1, country.getArmies().size());
	const auto& army = country.getArmies()[0];

	EXPECT_EQ("Armee royale", army.getName());
	EXPECT_FALSE(army.getArmyFloats());
	EXPECT_EQ(183, army.getLocation());

	ASSERT_EQ(1, army.getRegiments().size());
	const auto& regiment = army.getRegiments()[0];

	EXPECT_EQ("Reims's 1st Regiment", regiment.getName());
	EXPECT_EQ(180, regiment.getHome());
	EXPECT_EQ("chevauchee", regiment.getType());
}

TEST(EU4World_EU4CountryTests, RelationsCanBeLoaded)
{
	std::stringstream input;
	input << "active_relations={\n";
	input << "	C01={\n";
	input << "		cached_sum = -45\n";
	input << "		attitude = hostile\n";
	input << "	}\n";
	input << "	C02={\n";
	input << "		military_access=something\n";
	input << "		attitude = neutral\n";
	input << "	}\n";
	input << "}\n";

	const EU4::Country country("TAG", input);

	ASSERT_EQ(2, country.getRelations().size());
	const auto& C01relation = country.getRelations().at("C01");
	const auto& C02relation = country.getRelations().at("C02");

	EXPECT_FALSE(C01relation.hasMilitaryAccess());
	EXPECT_EQ(-45, C01relation.getRelations());
	EXPECT_EQ("hostile", C01relation.getAttitude());

	EXPECT_TRUE(C02relation.hasMilitaryAccess());
	EXPECT_EQ(0, C02relation.getRelations());
	EXPECT_EQ("neutral", C02relation.getAttitude());
}

TEST(EU4World_EU4CountryTests, CountryStatsCanBeLoaded)
{
	std::stringstream input;
	input << "legitimacy=48\n";
	input << "stability=2\n";
	input << "average_autonomy=0.13\n";
	input << "absolutism=98\n";
	input << "army_tradition=45.2\n";
	input << "navy_tradition=15.2\n";
	input << "army_professionalism=75.4\n";

	const EU4::Country country("TAG", input);

	EXPECT_NEAR(48, country.getLegitimacy(), 0.0001);
	EXPECT_NEAR(2, country.getStability(), 0.0001);
	EXPECT_NEAR(0.13, country.getAverageAutonomy(), 0.0001);
	EXPECT_NEAR(98, country.getAbsolutism(), 0.0001);
	EXPECT_NEAR(45.2, country.getArmyTradition(), 0.0001);
	EXPECT_NEAR(15.2, country.getNavyTradition(), 0.0001);
	EXPECT_NEAR(75.4, country.getArmyProfessionalism(), 0.0001);
}

TEST(EU4World_EU4CountryTests, StabilityDefaultsTo0)
{
	std::stringstream input;
	const EU4::Country country("TAG", input);

	EXPECT_EQ(0, country.getStability());
}

TEST(EU4World_EU4CountryTests, LegitimacyDefaultsTo100)
{
	std::stringstream input;
	const EU4::Country country("TAG", input);

	EXPECT_NEAR(100, country.getLegitimacy(), 0.0001);
}

TEST(EU4World_EU4CountryTests, ColonyDefaultsToFalse)
{
	std::stringstream input;
	const EU4::Country country("TAG", input);

	EXPECT_FALSE(country.isColony());
}

TEST(EU4World_EU4CountryTests, ColonyCanBeSet)
{
	std::stringstream input;
	input << "colonial_parent=SWE\n";
	const EU4::Country country("TAG", input);

	EXPECT_TRUE(country.isColony());
}

TEST(EU4World_EU4CountryTests, DependentStuffDefaultsToEmptyZero)
{
	std::stringstream input;
	const EU4::Country country("TAG", input);

	EXPECT_TRUE(country.getOverLord().empty());
	EXPECT_EQ(0, country.getLibertyDesire());
}

TEST(EU4World_EU4CountryTests, DependentStuffCanBeSet)
{
	std::stringstream input;
	input << "overlord=SWE\n";
	input << "liberty_desire=6.600\n";
	const EU4::Country country("TAG", input);

	EXPECT_EQ("SWE", country.getOverLord());
	EXPECT_NEAR(6.6, country.getLibertyDesire(), 0.0001);
}

TEST(EU4World_EU4CountryTests, GovernmentAndReformsCanBeSetAndPinged)
{
	std::stringstream input;
	input << "government_rank = 2\n";
	input << "government = {\n";
	input << "	government = monarchy\n";
	input << "	reform_stack={\n";
	input << "		reforms = { \"monarchy_mechanic\" \"plutocratic_reform\" \"enforce_privileges_reform\" }\n";
	input << "	}\n";
	input << "}\n";
	const EU4::Country country("TAG", input);

	EXPECT_EQ(2, country.getGovernmentRank());
	EXPECT_EQ("monarchy", country.getGovernment());
	EXPECT_THAT(country.getReforms(), UnorderedElementsAre("monarchy_mechanic", "plutocratic_reform", "enforce_privileges_reform"));
	EXPECT_TRUE(country.hasReform("plutocratic_reform"));
	EXPECT_FALSE(country.hasReform("nonsense"));
}

TEST(EU4World_EU4CountryTests, PoliciesCanBeLoaded)
{
	std::stringstream input;
	input << "active_policy = {\n";
	input << "	policy = pol1\n";
	input << "}\n";
	input << "active_policy = {\n";
	input << "	policy = pol2\n";
	input << "}\n";
	const EU4::Country country("TAG", input);

	EXPECT_THAT(country.getPolicies(), UnorderedElementsAre("pol1", "pol2"));
}

TEST(EU4World_EU4CountryTests, InstitutionsCanBeLoadedAndPinged)
{
	std::stringstream input;
	input << "institutions = {\n";
	input << "	0 1 1 0 0 0 0\n";
	input << "}\n";
	const EU4::Country country("TAG", input);

	EXPECT_THAT(country.getEmbracedInstitutions(), ElementsAre(false, true, true, false, false, false, false));
	EXPECT_EQ(2, country.getNumEmbracedInstitutions());
}

TEST(EU4World_EU4CountryTests, IdeasCanBeLoadedAndPinged)
{
	std::stringstream input;
	input << "active_idea_groups = {\n";
	input << "	religious_ideas = 7\n";
	input << "	offensive_ideas = 7\n";
	input << "	defensive_ideas = 3\n"; // <- dropped
	input << "	trade_ideas = 1\n";		// <- dropped
	input << "}\n";
	const EU4::Country country("TAG", input);

	EXPECT_THAT(country.getNationalIdeas(), UnorderedElementsAre("religious_ideas", "offensive_ideas"));
	EXPECT_TRUE(country.hasNationalIdea("religious_ideas"));
	EXPECT_TRUE(country.hasNationalIdea("offensive_ideas"));
	EXPECT_FALSE(country.hasNationalIdea("defensive_ideas"));
	EXPECT_FALSE(country.hasNationalIdea("trade_ideas"));
}

TEST(EU4World_EU4CountryTests, ManufactoriesCanBeTallied)
{
	std::stringstream provinceInput;
	provinceInput << "buildings={\n";
	provinceInput << "	weapons=yes\n"; // We're simulating a province with 2 manufactories. In reality there should be just 1 per province.
	provinceInput << "	furnace=yes\n"; // this one counts as 3
	provinceInput << "}\n";
	auto province = std::make_shared<EU4::Province>("-1", provinceInput);

	std::stringstream countryManagerInput;
	countryManagerInput << "TAG = {}\n";
	EU4::CountryManager countryManager;
	countryManager.loadCountries(countryManagerInput);

	const auto& country = countryManager.getCountry("TAG");
	country->addProvince(province);
	country->buildManufactoryCount(countryManager.getCountries()); // it needs all countries for manufactory power transfer to overlord. Lee later tests.

	EXPECT_EQ(4, country->getManufactoryCount());
	EXPECT_EQ(4, country->getManufactoryDensity()); // 4 manufactories per 1 province.
}

TEST(EU4World_EU4CountryTests, ManufactoriesCanBeTransferredToOverlord)
{
	std::stringstream provinceInput; // we'll be sharing this province among both countries
	provinceInput << "buildings={\n";
	provinceInput << "	weapons=yes\n"; // We're simulating a province with 2 manufactories. In reality there should be just 1 per province.
	provinceInput << "	furnace=yes\n"; // this one counts as 3
	provinceInput << "}\n";
	auto province = std::make_shared<EU4::Province>("-1", provinceInput);

	std::stringstream countryManagerInput;
	countryManagerInput << "TAG = { overlord = TA2 }\n";
	countryManagerInput << "TA2 = {}\n";
	EU4::CountryManager countryManager;
	countryManager.loadCountries(countryManagerInput);

	const auto& underling = countryManager.getCountry("TAG");
	const auto& overlord = countryManager.getCountry("TA2");
	underling->addProvince(province);
	overlord->addProvince(province);
	underling->buildManufactoryCount(countryManager.getCountries());
	overlord->buildManufactoryCount(countryManager.getCountries());

	EXPECT_EQ(2, underling->getManufactoryCount());	  // sent half to overlord
	EXPECT_EQ(4, underling->getManufactoryDensity()); // density is unaffected as they are physically in underling's borders
	EXPECT_EQ(6, overlord->getManufactoryCount());	  // got half from underling
	EXPECT_EQ(4, overlord->getManufactoryDensity());  // density is unaffected.
}

TEST(EU4World_EU4CountryTests, NamesAndAdjectivesCanBeLoadedAndPoked)
{
	std::stringstream input;
	input << "name = \"the name\"\n";
	input << "adjective = \"the adjective\"\n";
	EU4::Country country("TAG", input);

	country.setLocalizationName("french", "le country");
	country.setLocalizationName("english", "the country");
	country.setLocalizationAdjective("french", "le country's");
	country.setLocalizationAdjective("english", "the country's");

	EXPECT_EQ("le country", country.getName("french"));
	EXPECT_EQ("the country", country.getName("english"));
	EXPECT_EQ("le country's", country.getAdjective("french"));
	EXPECT_EQ("the country's", country.getAdjective("english"));
}

TEST(EU4World_EU4CountryTests, NamesAndAdjectivesUseEnglishForFallback)
{
	std::stringstream input;
	input << "name = \"the name\"\n";
	input << "adjective = \"the adjective\"\n";
	EU4::Country country("TAG", input);

	country.setLocalizationName("french", "le country");
	country.setLocalizationName("english", "the country");
	country.setLocalizationAdjective("french", "le country's");
	country.setLocalizationAdjective("english", "the country's");

	EXPECT_EQ("the country", country.getName("german"));
	EXPECT_EQ("the country's", country.getAdjective("portugese"));
}

TEST(EU4World_EU4CountryTests, NamesAndAdjectivesUseBaseGameForFallbackForEnglishForFallback)
{
	std::stringstream input;
	input << "name = \"the name\"\n";
	input << "adjective = \"the adjective\"\n";
	EU4::Country country("TAG", input);

	country.setLocalizationName("french", "le country");
	country.setLocalizationAdjective("french", "le country's");

	// these 2 are set
	EXPECT_EQ("le country", country.getName("french"));
	EXPECT_EQ("le country's", country.getAdjective("french"));

	// these lack english locs for fallback and aren't set.
	EXPECT_EQ("the name", country.getName("english"));
	EXPECT_EQ("the adjective", country.getAdjective("russian"));
	EXPECT_EQ("the name", country.getName("german"));
	EXPECT_EQ("the adjective", country.getAdjective("portugese"));
}

TEST(EU4World_EU4CountryTests, NationalColorsCanBeLoadedInTheirFullGlory)
{
	std::stringstream input;
	input << "colors={\n";
	input << "	map_color = { 1 2 3 }\n";
	input << "	revolutionary_colors = { 4 5 6 }\n";
	input << "	custom_colors = {\n";
	input << "		flag = 7\n";
	input << "		color = 23\n";
	input << "		symbol_index = 34\n";
	input << "		flag_colors = {12 34 56}\n";
	input << "	}\n";
	input << "}\n";
	const EU4::Country country("TAG", input);

	const auto& symbol = country.getNationalColors();

	EXPECT_EQ("= rgb { 1 2 3 }", symbol.getMapColor()->outputRgb());
	EXPECT_EQ("= rgb { 4 5 6 }", symbol.getRevolutionaryColor()->outputRgb());
	EXPECT_EQ(8, symbol.getCustomColors()->flagIndex);
	EXPECT_EQ(24, symbol.getCustomColors()->colorIndex);
	EXPECT_EQ(35, symbol.getCustomColors()->symbolIndex);
	EXPECT_EQ("= rgb { 12 34 56 }", symbol.getCustomColors()->flagColors->outputRgb());
}

TEST(EU4World_EU4CountryTests, CultureInCoresCanBePingedForCoresUnderForeignRule)
{
	std::stringstream provinceInput; // this province belongs to TAG, but it's a core of GAT and has their primary culture.
	provinceInput << "culture = cul\n";
	provinceInput << "religion = rel\n";
	provinceInput << "owner = TAG\n";
	auto province = std::make_shared<EU4::Province>("-1", provinceInput);

	std::stringstream countryManagerInput;
	countryManagerInput << "TAG = { primary_culture = cul2 }\n"; // they don't share the culture with the province
	countryManagerInput << "GAT = { primary_culture = cul }\n";
	EU4::CountryManager countryManager;
	countryManager.loadCountries(countryManagerInput);

	const auto& tag = countryManager.getCountry("TAG");
	const auto& gat = countryManager.getCountry("GAT");
	tag->addProvince(province);
	gat->addCore(province);

	DatingData datingData;
	datingData.startEU4Date = date("1444.11.11");
	province->buildPopRatios(datingData);

	EXPECT_TRUE(gat->cultureSurvivesInCores(countryManager.getCountries()));
}

TEST(EU4World_EU4CountryTests, CultureInCoresCanBePingedForCoresUnderSameCultureRule)
{
	std::stringstream provinceInput; // this province belongs to TAG, but it's a core of GAT and has their primary culture.
	provinceInput << "culture = cul\n";
	provinceInput << "religion = rel\n";
	provinceInput << "owner = TAG\n";
	auto province = std::make_shared<EU4::Province>("-1", provinceInput);

	std::stringstream countryManagerInput;
	countryManagerInput << "TAG = { primary_culture = cul }\n"; // they share the culture with Gat's province so it belongs to them, actually.
	countryManagerInput << "GAT = { primary_culture = cul }\n";
	EU4::CountryManager countryManager;
	countryManager.loadCountries(countryManagerInput);

	const auto& tag = countryManager.getCountry("TAG");
	const auto& gat = countryManager.getCountry("GAT");
	tag->addProvince(province);
	gat->addCore(province);

	DatingData datingData;
	datingData.startEU4Date = date("1444.11.11");
	province->buildPopRatios(datingData);

	EXPECT_FALSE(gat->cultureSurvivesInCores(countryManager.getCountries()));
}

TEST(EU4World_EU4CountryTests, CountryWeightSumsProvinceWeight)
{
	std::stringstream province1Input;
	province1Input << "owner = TAG\n";
	province1Input << "base_tax = 3\n";
	province1Input << "base_production = 4\n";
	province1Input << "base_manpower = 5\n";
	auto province1 = std::make_shared<EU4::Province>("-1", province1Input);

	std::stringstream province2Input;
	province2Input << "owner = TAG\n";
	province2Input << "base_tax = 13\n";
	province2Input << "base_production = 14\n";
	province2Input << "base_manpower = 15\n";
	auto province2 = std::make_shared<EU4::Province>("-2", province2Input);

	const EU4::BuildingCostLoader buildings;

	province1->determineProvinceWeight(buildings);
	province2->determineProvinceWeight(buildings);

	std::stringstream input;
	EU4::Country country("TAG", input);

	country.addProvince(province1);
	country.addProvince(province2);

	EXPECT_NEAR(12, province1->getProvinceWeight(), 0.0001);
	EXPECT_NEAR(42, province2->getProvinceWeight(), 0.0001);
	EXPECT_NEAR(54, country.getCountryWeight(), 0.0001);
}

TEST(EU4World_EU4CountryTests, CountryReturnsAverageProvinceDevelopment)
{
	std::stringstream province1Input;
	province1Input << "owner = TAG\n";
	province1Input << "base_tax = 3\n";
	province1Input << "base_production = 4\n";
	province1Input << "base_manpower = 5\n";
	auto province1 = std::make_shared<EU4::Province>("-1", province1Input);

	std::stringstream province2Input;
	province2Input << "owner = TAG\n";
	province2Input << "base_tax = 13\n";
	province2Input << "base_production = 14\n";
	province2Input << "base_manpower = 15\n";
	auto province2 = std::make_shared<EU4::Province>("-2", province2Input);

	std::stringstream input;
	EU4::Country country("TAG", input);

	country.addProvince(province1);
	country.addProvince(province2);

	EXPECT_NEAR(27, country.getAverageDevelopment(), 0.0001); // 12 + 42 = 54, 54 / 2 = 27
}

TEST(EU4World_EU4CountryTests, AverageProvinceDevelopmentForNoProvincesIsZero)
{
	std::stringstream input;
	EU4::Country country("TAG", input);

	EXPECT_DOUBLE_EQ(0, country.getAverageDevelopment());
}

TEST(EU4World_EU4CountryTests, UpdateRegimentTypesUpdatesallArmies)
{
	EU4::UnitTypeLoader unitTypeLoader;
	std::stringstream unitInput;
	unitInput << "type = heavy_ship maneuver = 60\n";
	unitTypeLoader.loadUnitType("the-manowar", unitInput);
	std::stringstream unitInput2;
	unitInput2 << "type = transport maneuver = 4\n";
	unitTypeLoader.loadUnitType("the-transport", unitInput2);
	std::stringstream unitInput3;
	unitInput3 << "type = infantry maneuver = 2\n";
	unitTypeLoader.loadUnitType("native_indian_archer", unitInput3);
	std::stringstream unitInput4;
	unitInput4 << "type = cavalry maneuver = 4\n";
	unitTypeLoader.loadUnitType("native_indian_horseperson", unitInput4);

	std::stringstream input;
	input << "navy={\n";
	input << "	name = \"1st Fleet\"\n";
	input << "	location = 11\n";
	input << "	ship = {name = \"Dulecha\" home = 1211 type = \"the-manowar\"}\n";
	input << "	ship = {name = \"Koneba\" home = 1211 type = \"the-manowar\"}\n";
	input << "	ship = {name = \"Dallol\" home = 1211 type = \"the-transport\"}\n";
	input << "}\n";
	input << "army={\n";
	input << "	name = \"1st Army\"\n";
	input << "	location = 971\n";
	input << "	regiment = {name = \"Penobscott's 1st Regiment\" home = 971 type = \"native_indian_archer\"}\n";
	input << "	regiment = {name = \"Penobscott's 3st Regiment\" home = 971 type = \"native_indian_archer\"}\n";
	input << "	regiment = {name = \"Penobscott's 2st Regiment\" home = 971 type = \"native_indian_horseperson\"}\n";
	input << "}\n";

	EU4::Country country("TAG", input);

	country.updateRegimentTypes(unitTypeLoader);

	const auto& navy = country.getArmies()[0];
	const auto& army = country.getArmies()[1];

	EXPECT_EQ(120, navy.getTotalTypeStrength("heavy_ship")); // two heavy ships worth 60 each
	EXPECT_EQ(4, navy.getTotalTypeStrength("transport"));		// one transport worth 4.
	EXPECT_EQ(4, army.getTotalTypeStrength("infantry"));		// two inf regiments worth 2 each
	EXPECT_EQ(4, army.getTotalTypeStrength("cavalry"));		// one cav regiment worth 4
}

TEST(EU4World_EU4CountryTests, EatCountryWontEatItself)
{
	std::stringstream input;
	auto country = std::make_shared<EU4::Country>("TAG", input);

	std::stringstream province1Input;
	const auto province1 = std::make_shared<EU4::Province>("-1", province1Input);

	country->addProvince(province1);
	country->eatCountry(country);

	EXPECT_FALSE(country->getProvinces().empty()); // province is still inside.
}

TEST(EU4World_EU4CountryTests, EatCountryFullExample)
{
	std::stringstream provincesInput;
	provincesInput << "-1 = { cores={EAT} }\n"; // to eater
	provincesInput << "-2 = { cores={EAT} }\n"; // to eater
	provincesInput << "-3 = { cores={TGT} }\n"; // to target
	EU4::ProvinceManager provinceManager;
	provinceManager.loadProvinces(provincesInput);

	std::stringstream countryManagerInput;
	countryManagerInput << "TGT = { army={} }\n"; // target, tech 0 for simplicity, 1 army, 0 score
	countryManagerInput << "EAT = { navy={} \n";	 // eater, tech 10, 1 navy, 10 score
	countryManagerInput << "	technology = { adm_tech = 10 dip_tech = 10 mil_tech = 10}\n";
	countryManagerInput << " age_score = { 10 }\n";
	countryManagerInput << "}\n";
	EU4::CountryManager countryManager;
	countryManager.loadCountries(countryManagerInput);

	const auto& target = countryManager.getCountry("TGT");
	const auto& eater = countryManager.getCountry("EAT");

	// weight ratio will be 2:1
	target->addCore(provinceManager.getProvince(1));
	target->addProvince(provinceManager.getProvince(1));
	eater->addCore(provinceManager.getProvince(2));
	eater->addProvince(provinceManager.getProvince(2));
	eater->addCore(provinceManager.getProvince(3));
	eater->addProvince(provinceManager.getProvince(3));

	// yum
	eater->eatCountry(target);

	// eater's score has been lowered by 2:1 from 10 to 6.66 since target was at 0
	EXPECT_NEAR(6.6666, eater->getScore(), 0.0001);
	// techs have been lowered by same ratio.
	EXPECT_NEAR(6.6666, eater->getAdmTech(), 0.0001);
	EXPECT_NEAR(6.6666, eater->getDipTech(), 0.0001);
	EXPECT_NEAR(6.6666, eater->getMilTech(), 0.0001);

	// eaten province contains a single core - of the eater
	EXPECT_THAT(provinceManager.getProvince(1)->getCores(), UnorderedElementsAre("EAT"));

	// eaten province has been added to the cores and provinces of eater
	EXPECT_EQ(1, eater->getCores().at(1)->getID());
	EXPECT_EQ(1, eater->getProvinces().at(1)->getID());
	EXPECT_EQ(3, eater->getCores().size());
	EXPECT_EQ(3, eater->getProvinces().size());

	// target has no cores or provinces.
	EXPECT_TRUE(target->getCores().empty());
	EXPECT_TRUE(target->getProvinces().empty());

	// eater has both armies
	EXPECT_EQ(2, eater->getArmies().size());
	EXPECT_TRUE(eater->getArmies()[0].getArmyFloats());
	EXPECT_FALSE(eater->getArmies()[1].getArmyFloats()); // eaten land army

	// target has no armies
	EXPECT_TRUE(target->getArmies().empty());
}
