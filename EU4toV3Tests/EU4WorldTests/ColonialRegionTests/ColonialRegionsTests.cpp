#include "ColonialRegions/ColonialRegions.h"
#include "gtest/gtest.h"

TEST(EU4World_ColonialRegionsTests, provincesCanBeMatched)
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

	const auto regions = EU4::ColonialRegions(input);

	ASSERT_EQ("colonial_alaska", regions.getColonialRegionForProvince(1));
	ASSERT_EQ("colonial_alaska", regions.getColonialRegionForProvince(2));
	ASSERT_EQ("colonial_alaska", regions.getColonialRegionForProvince(3));
}

TEST(EU4World_ColonialRegionsTests, provincesCanBeMisMatched)
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

	const auto regions = EU4::ColonialRegions(input);

	ASSERT_EQ(std::nullopt, regions.getColonialRegionForProvince(4));
}
