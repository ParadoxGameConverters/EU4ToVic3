#include "CountryManager/EU4Country.h"
#include "ProvinceManager/ProvinceManager.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::UnorderedElementsAre;

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
	EXPECT_THAT(country.getProvinces(), UnorderedElementsAre(province1, province2));

	EXPECT_TRUE(country.getCores().empty());
	country.addCore(province1);
	country.addCore(province2);
	EXPECT_THAT(country.getCores(), UnorderedElementsAre(province1, province2));
}

TEST(EU4World_EU4CountryTests, StatusesCanBeToggled)
{
	std::stringstream input;
	EU4::Country country("TAG", input);

	EXPECT_FALSE(country.getInHRE());
	EXPECT_FALSE(country.getHolyRomanEmperor());
	EXPECT_FALSE(country.getCelestialEmperor());
	EXPECT_FALSE(country.isRevolutionary());

	country.setInHRE(true);
	country.setEmperor(true);
	country.setCelestialEmperor(true);
	country.setRevolutionary(true);

	EXPECT_TRUE(country.getInHRE());
	EXPECT_TRUE(country.getHolyRomanEmperor());
	EXPECT_TRUE(country.getCelestialEmperor());
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
	const auto& leaders = country.getMilitaryLeaders();

	EXPECT_EQ(2, leaders.size());

	const auto& leader1 = leaders[0];
	const auto& leader2 = leaders[1];

	EXPECT_EQ("Sir. Still Alive", leader1.getName());
	EXPECT_FALSE(leader1.isLand());
	EXPECT_EQ(6, leader1.getManeuver());

	EXPECT_EQ("His Majesty Still Kicking", leader2.getName());
	EXPECT_TRUE(leader2.isLand());
	EXPECT_EQ(4, leader2.getFire());
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
	input <<	"		type = \"chevauchee\"\n";
	input << "	}\n";
	input << "}\n";	

	const EU4::Country country("TAG", input);
	const auto& army = country.getArmies()[0];
	
	EXPECT_TRUE("Armee royale", army.getName());
	EXPECT_FALSE(army.getArmyFloats());
	EXPECT_EQ(183, army.getLocation());
	EXPECT_EQ(1, army.getRegiments().size());

	const auto& regiment = army.getRegiments()[0];
	
	EXPECT_EQ("Reims's 1st Regiment", regiment.getName());
	EXPECT_EQ(180, regiment.getHome());
	EXPECT_EQ("chevauchee", regiment.getType());
}
