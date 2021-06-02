#include "ColonialRegionLoader/ColonialRegionLoader.h"
#include "gtest/gtest.h"

TEST(EU4World_ColonialRegionLoaderTests, provincesCanBeMatched)
{
	std::stringstream input;
	input << "colonial_alaska = {\n";
	input << "\ttax_income = 2\n";
	input << "\tprovinces = {\n";
	input << "\t\t1 2 3\n";
	input << "\t}\n";
	input << "}\n";
	input << "colonial_placeholder = {\n";
	input << "\tcolor = { 200 190 20 }\n ";
	input << "}\n";
	EU4::ColonialRegionLoader regions;
	regions.loadColonialRegions(input);

	EXPECT_EQ("colonial_alaska", regions.getColonialRegionForProvince(1));
	EXPECT_EQ("colonial_alaska", regions.getColonialRegionForProvince(2));
	EXPECT_EQ("colonial_alaska", regions.getColonialRegionForProvince(3));
}

TEST(EU4World_ColonialRegionLoaderTests, provincesCanBeMisMatched)
{
	std::stringstream input;
	input << "colonial_alaska = {\n";
	input << "\ttax_income = 2\n";
	input << "\tprovinces = {\n";
	input << "\t\t1 2 3\n";
	input << "\t}\n";
	input << "}\n";
	input << "colonial_placeholder = {\n";
	input << "\tcolor = { 200 190 20 }\n ";
	input << "}\n";
	EU4::ColonialRegionLoader regions;
	regions.loadColonialRegions(input);

	EXPECT_EQ(std::nullopt, regions.getColonialRegionForProvince(4));
}
