#include "Regions/Area.h"
#include "gtest/gtest.h"

TEST(EU4World_Regions_AreaTests, blankAreasLoadWithNoProvinces)
{
	std::stringstream input;
	const EU4::Area newArea(input);

	ASSERT_TRUE(newArea.getProvinces().empty());
}

TEST(EU4World_Regions_AreaTests, provincesCanBeLoaded)
{
	std::stringstream input;
	input << "{ 1 2 3 }";

	const EU4::Area newArea(input);

	ASSERT_FALSE(newArea.getProvinces().empty());
	ASSERT_EQ(3, newArea.getProvinces().size());
	ASSERT_TRUE(newArea.getProvinces().contains(1));
	ASSERT_TRUE(newArea.getProvinces().contains(2));
	ASSERT_TRUE(newArea.getProvinces().contains(3));
}

TEST(EU4World_Regions_AreaTests, colorIsIgnored)
{
	std::stringstream input;
	input << "={\n";
	input << "color = { 7 8 9 }\n";
	input << "1 2 3\n";
	input << "}\n";
	const EU4::Area newArea(input);

	ASSERT_FALSE(newArea.getProvinces().empty());
	ASSERT_EQ(3, newArea.getProvinces().size());
	ASSERT_TRUE(newArea.getProvinces().contains(1));
	ASSERT_TRUE(newArea.getProvinces().contains(2));
	ASSERT_TRUE(newArea.getProvinces().contains(3));
}
