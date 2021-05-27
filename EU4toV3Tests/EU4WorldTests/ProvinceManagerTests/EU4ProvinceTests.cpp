#include "ProvinceManager/EU4Province.h"
#include "gtest/gtest.h"

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
	EXPECT_FALSE(theProvince.isTerritorialCore());

	EXPECT_EQ(0.0, theProvince.getBaseTax());
	EXPECT_EQ(0.0, theProvince.getBaseProduction());
	EXPECT_EQ(0.0, theProvince.getBaseManpower());
	EXPECT_EQ(0.0, theProvince.getStartingTax());
	EXPECT_EQ(0.0, theProvince.getStartingProduction());
	EXPECT_EQ(0.0, theProvince.getStartingManpower());

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
	EXPECT_TRUE(theProvince.inHre());

	EXPECT_DOUBLE_EQ(3.0, theProvince.getBaseTax());
	EXPECT_DOUBLE_EQ(4.0, theProvince.getBaseProduction());
	EXPECT_DOUBLE_EQ(5.0, theProvince.getBaseManpower());
}

TEST(EU4World_ProvinceTests, coresCanBeSet)
{
	std::stringstream input;
	input << "cores={\n";
	input << "\t\"TAG\"\n";
	input << "}\n";
	const EU4::Province theProvince("-1", input);

	EXPECT_TRUE(theProvince.getCores().contains("TAG"));
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
