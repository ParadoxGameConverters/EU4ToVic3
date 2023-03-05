#include "BuildingCostLoader/BuildingCostLoader.h"
#include "ProvinceManager/EU4Province.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::UnorderedElementsAre;

TEST(EU4World_ProvinceTests, IDProperlyInterpreted)
{
	std::stringstream input;
	const EU4::Province theProvince("-42", input);

	EXPECT_EQ(42, theProvince.getID());
}

TEST(EU4World_ProvinceTests, primitivesDefaultToDefaults)
{
	std::stringstream input;
	const EU4::Province theProvince("-1", input);

	EXPECT_TRUE(theProvince.getName().empty());
	EXPECT_TRUE(theProvince.getOwnerTag().empty());
	EXPECT_TRUE(theProvince.getControllerTag().empty());
	EXPECT_TRUE(theProvince.getCulture().empty());
	EXPECT_TRUE(theProvince.getReligion().empty());

	EXPECT_FALSE(theProvince.inHre());
	EXPECT_FALSE(theProvince.isTradeCompany());
	EXPECT_FALSE(theProvince.isTerritorialCore());

	EXPECT_EQ(0.0, theProvince.getBaseTax());
	EXPECT_EQ(0.0, theProvince.getBaseProduction());
	EXPECT_EQ(0.0, theProvince.getBaseManpower());

	EXPECT_TRUE(theProvince.getCores().empty());
	EXPECT_TRUE(theProvince.getBuildings().empty());
}

TEST(EU4World_ProvinceTests, primitivesCanBeSet)
{
	std::stringstream input;
	input << "name = theName\n";
	input << "owner = TAG\n";
	input << "controller = TAG2\n";
	input << "culture = theCulture\n";
	input << "religion = theReligion\n";

	input << "hre = yes\n";
	input << "active_trade_company = yes\n";
	input << "territorial_core = irrelevant\n";

	input << "base_tax = 3.000\n";
	input << "base_production = 4.000\n";
	input << "base_manpower = 5.000\n";

	const EU4::Province theProvince("-1", input);

	EXPECT_EQ("theName", theProvince.getName());
	EXPECT_EQ("TAG", theProvince.getOwnerTag());
	EXPECT_EQ("TAG2", theProvince.getControllerTag());
	EXPECT_EQ("theCulture", theProvince.getCulture());
	EXPECT_EQ("theReligion", theProvince.getReligion());

	EXPECT_TRUE(theProvince.isTerritorialCore());
	EXPECT_TRUE(theProvince.isTradeCompany());
	EXPECT_TRUE(theProvince.inHre());

	EXPECT_DOUBLE_EQ(3.0, theProvince.getBaseTax());
	EXPECT_DOUBLE_EQ(4.0, theProvince.getBaseProduction());
	EXPECT_DOUBLE_EQ(5.0, theProvince.getBaseManpower());
}

TEST(EU4World_ProvinceTests, provinceWithoutFullHistoryInitializesHistory)
{
	std::stringstream input;
	input << "culture = cul\n";
	input << "religion = rel\n";
	const EU4::Province theProvince("-1", input);

	const auto& cultureHistory = theProvince.getProvinceHistory().getCultureHistory();
	const auto& religionHistory = theProvince.getProvinceHistory().getReligionHistory();

	EXPECT_THAT(cultureHistory, UnorderedElementsAre(std::pair<date, std::string>(date("1.1.1"), "cul")));
	EXPECT_THAT(religionHistory, UnorderedElementsAre(std::pair<date, std::string>(date("1.1.1"), "rel")));
}

TEST(EU4World_ProvinceTests, coresCanBeSet)
{
	std::stringstream input;
	input << "cores={\n";
	input << "\t\"TAG\"\n";
	input << "}\n";
	const EU4::Province theProvince("-1", input);

	EXPECT_THAT(theProvince.getCores(), UnorderedElementsAre("TAG"));
}

TEST(EU4World_ProvinceTests, inHreCanRemainNegative)
{
	std::stringstream input;
	input << "hre=no\n";
	const EU4::Province theProvince("-1", input);

	EXPECT_FALSE(theProvince.inHre());
}

TEST(EU4World_ProvinceTests, inHreCanBeTrue)
{
	std::stringstream input;
	input << "hre=yes\n";
	const EU4::Province theProvince("-1", input);

	EXPECT_TRUE(theProvince.inHre());
}

TEST(EU4World_ProvinceTests, hasBuildingDefaultsToFalse)
{
	std::stringstream input;
	const EU4::Province theProvince("-1", input);

	EXPECT_FALSE(theProvince.hasBuilding("theBuilding"));
}

TEST(EU4World_ProvinceTests, hasBuildingCanBeTrue)
{
	std::stringstream input;
	input << "buildings={\n";
	input << "\ttheBuilding=yes\n";
	input << "}\n";
	const EU4::Province theProvince("-1", input);

	EXPECT_TRUE(theProvince.hasBuilding("theBuilding"));
}

TEST(EU4World_ProvinceTests, hasGreatProjectDefaultsToFalse)
{
	std::stringstream input;
	const EU4::Province theProvince("-1", input);

	EXPECT_FALSE(theProvince.hasGreatProject("test_canal"));
}

TEST(EU4World_ProvinceTests, hasGreatProjectFindsProjects)
{
	std::stringstream input;
	input << "great_projects={\n";
	input << "\t\"test_canal\"\n";
	input << "}\n";
	const EU4::Province theProvince("-1", input);

	EXPECT_TRUE(theProvince.hasGreatProject("test_canal"));
}

TEST(EU4World_ProvinceTests, provinceWeightWithoutOwnerIsZero)
{
	std::stringstream input;
	input << "base_tax = 3\n";
	input << "base_production = 4\n";
	input << "base_manpower = 5\n";
	EU4::Province theProvince("-1", input);

	const EU4::BuildingCostLoader buildings;

	theProvince.determineProvinceWeight(buildings);

	EXPECT_EQ(0.0, theProvince.getProvinceWeight());
}

TEST(EU4World_ProvinceTests, provinceWeightWithoutBuildingsIsCombinedDevelopment)
{
	std::stringstream input;
	input << "owner = TST\n";
	input << "base_tax = 3\n";
	input << "base_production = 4\n";
	input << "base_manpower = 5\n";
	EU4::Province theProvince("-1", input);

	const EU4::BuildingCostLoader buildings;

	theProvince.determineProvinceWeight(buildings);

	EXPECT_NEAR(12, theProvince.getProvinceWeight(), 0.001);
}

TEST(EU4World_ProvinceTests, provinceWeightIncreasesByOnePercentOfBuildingsCost)
{
	std::stringstream input;
	input << "owner = TST\n";
	input << "base_tax = 3\n";
	input << "base_production = 4\n";
	input << "base_manpower = 5\n";
	input << "buildings = {\n";
	input << "\tshack = lovely\n";
	input << "\twatermill = nice\n";
	input << "}\n";
	EU4::Province theProvince("-1", input);

	std::stringstream buildingStream;
	buildingStream << "shack = { cost = 100 }\n";
	buildingStream << "watermill = { cost = 700 }\n"; // sums to 8 in total
	EU4::BuildingCostLoader buildings;
	buildings.loadBuildingCosts(buildingStream);

	theProvince.determineProvinceWeight(buildings);

	EXPECT_NEAR(20.0, theProvince.getProvinceWeight(), 0.001);
}

TEST(EU4World_ProvinceTests, investmentFactorForNoOwnerIsZero)
{
	std::stringstream input;
	input << "base_tax = 3\n";
	input << "base_production = 4\n";
	input << "base_manpower = 5\n";
	input << "history = {\n";
	input << "\tbase_tax = 1\n";
	input << "\tbase_production = 2\n";
	input << "\tbase_manpower = 1\n";
	input << "}\n";

	EU4::Province theProvince("-1", input);

	std::stringstream buildingStream;
	EU4::BuildingCostLoader buildings;
	buildings.loadBuildingCosts(buildingStream);

	theProvince.determineProvinceWeight(buildings);

	EXPECT_EQ(0.0, theProvince.getInvestmentFactor());
}

TEST(EU4World_ProvinceTests, investmentWeightWithoutBuildingsCanBeCalculated)
{
	std::stringstream input;
	input << "owner = TST\n";
	input << "base_tax = 1\n";
	input << "base_production = 12\n"; // increased by 10
	input << "base_manpower = 1\n";
	input << "history = {\n";
	input << "\tbase_tax = 1\n";
	input << "\tbase_production = 2\n";
	input << "\tbase_manpower = 1\n";
	input << "}\n";

	EU4::Province theProvince("-1", input);

	std::stringstream buildingStream;
	EU4::BuildingCostLoader buildings;
	buildings.loadBuildingCosts(buildingStream);

	// Increasing development by 10 gives us a factor of 0 (log10(10) - 1) * 10
	theProvince.determineProvinceWeight(buildings);

	EXPECT_EQ(10.0, theProvince.getInvestedWeight());
}

TEST(EU4World_ProvinceTests, investmentWeightUsesOnePercentBuildingsCostAsDev)
{
	std::stringstream input;
	input << "owner = TST\n";
	input << "base_tax = 1\n";
	input << "base_production = 92\n"; // increased by 90
	input << "base_manpower = 1\n";
	input << "history = {\n";
	input << "\tbase_tax = 1\n";
	input << "\tbase_production = 2\n";
	input << "\tbase_manpower = 1\n";
	input << "}\n";
	input << "buildings = {\n";
	input << "\tshack = lovely\n";
	input << "\twatermill = nice\n";
	input << "}\n";
	EU4::Province theProvince("-1", input);

	std::stringstream buildingStream;
	buildingStream << "shack = { cost = 300 }\n";
	buildingStream << "watermill = { cost = 700 }\n"; // sums to 10 in total
	EU4::BuildingCostLoader buildings;
	buildings.loadBuildingCosts(buildingStream);

	// Increasing development by 90 + 10
	theProvince.determineProvinceWeight(buildings);

	EXPECT_NEAR(100.0, theProvince.getInvestedWeight(), 0.001);
}

TEST(EU4World_ProvinceTests, startingCultureCanBeRetrieved)
{
	std::stringstream input;
	input << "culture=TAG1Culture\n";
	input << "history = {\n";
	input << "	culture=origCulture\n";
	input << "	1736.1.1={\n";
	input << "		culture=TAG1Culture\n";
	input << "	}\n";
	input << "}\n";
	EU4::Province province("-1", input);

	EXPECT_EQ("origCulture", province.getStartingCulture());
}

TEST(EU4World_ProvinceTests, culturePercentCanBeRetrieved)
{
	DatingData datingData;
	datingData.startEU4Date = date("1254.11.11");

	std::stringstream input;
	input << "culture=TAG1Culture\n";
	input << "religion=TAG1Religion\n";
	input << "history = {\n";
	input << "	culture=origCulture\n";
	input << "	religion=origReligion\n";
	input << "	1736.1.1={\n";
	input << "		owner=TAG1\n";
	input << "		culture=TAG1Culture\n";
	input << "		religion=TAG1Religion\n";
	input << "	}\n";
	input << "}\n";
	EU4::Province province("-1", input);
	province.setAssimilationFactor(0.0025);
	province.buildPopRatios(datingData);

	EXPECT_NEAR(0.7785, province.getCulturePercent("origCulture"), 0.0001);
	EXPECT_NEAR(0.2214, province.getCulturePercent("TAG1Culture"), 0.0001);
}

TEST(EU4World_ProvinceTests, culturePercentAfterPurgingIs100)
{
	DatingData datingData;
	datingData.startEU4Date = date("1254.11.11");

	std::stringstream input;
	input << "culture=TAG1Culture\n";
	input << "religion=TAG1Religion\n";
	input << "history = {\n";
	input << "	culture=origCulture\n";
	input << "	religion=origReligion\n";
	input << "	1736.1.1={\n";
	input << "		owner=TAG1\n";
	input << "		culture=TAG1Culture\n";
	input << "		religion=TAG1Religion\n";
	input << "	}\n";
	input << "}\n";
	EU4::Province province("-1", input);
	province.purgeHistories();
	province.setAssimilationFactor(0.0025);
	province.buildPopRatios(datingData);

	EXPECT_NEAR(0, province.getCulturePercent("origCulture"), 0.0001);
	EXPECT_NEAR(1, province.getCulturePercent("TAG1Culture"), 0.0001);
}
