#include "Country/EU4CustomColors.h"
#include "gtest/gtest.h"

TEST(EU4World_Country_EU4CustomColorsTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const EU4::CustomColors colors(input);

	ASSERT_EQ(0, colors.getCustomColors().color);
	ASSERT_EQ(0, colors.getCustomColors().flag);
	ASSERT_EQ(0, colors.getCustomColors().symbolIndex);
	ASSERT_FALSE(colors.getCustomColors().flagColors);
}

TEST(EU4World_Country_EU4CustomColorsTests, primitivesCanBeLoadedAndIndexIsIncreased)
{
	std::stringstream input;
	input << "flag = 7\n";
	input << "color = 23\n";
	input << "symbol_index = 34\n";
	input << "flag_colors = {12 34 56}\n";
	const EU4::CustomColors colors(input);

	ASSERT_EQ(8, colors.getCustomColors().flag);
	ASSERT_EQ(24, colors.getCustomColors().color);
	ASSERT_EQ(35, colors.getCustomColors().symbolIndex);
	ASSERT_TRUE(colors.getCustomColors().flagColors);
}

TEST(EU4World_Country_EU4CustomColorsTests, negativeFlagIndexIsSetTo1)
{
	std::stringstream input;
	input << "flag = -2\n";
	const EU4::CustomColors colors(input);

	ASSERT_EQ(1, colors.getCustomColors().flag);
}
