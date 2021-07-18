#include "CountryManager/NationalSymbol/EU4CustomColors.h"
#include "gtest/gtest.h"

TEST(EU4World_EU4CustomColorsTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const EU4::CustomColors colors(input);

	const auto& customColorBlock = colors.getCustomColorsBlock();

	EXPECT_EQ(0, customColorBlock.colorIndex);
	EXPECT_EQ(0, customColorBlock.symbolIndex);
	EXPECT_EQ(0, customColorBlock.flagIndex);
	EXPECT_FALSE(customColorBlock.flagColors);
}

TEST(EU4World_EU4CustomColorsTests, primitivesCanBeLoadedAndIndexIsIncreased)
{
	std::stringstream input;
	input << "flag = 7\n";
	input << "color = 23\n";
	input << "symbol_index = 34\n";
	input << "flag_colors = {12 34 56}\n";
	const EU4::CustomColors colors(input);
	const auto& customColorBlock = colors.getCustomColorsBlock();

	EXPECT_EQ(8, customColorBlock.flagIndex);
	EXPECT_EQ(24, customColorBlock.colorIndex);
	EXPECT_EQ(35, customColorBlock.symbolIndex);
	EXPECT_TRUE(customColorBlock.flagColors);
}

TEST(EU4World_EU4CustomColorsTests, negativeFlagIndexIsSetTo1)
{
	std::stringstream input;
	input << "flag = -2\n";
	const EU4::CustomColors colors(input);

	EXPECT_EQ(1, colors.getCustomColorsBlock().flagIndex);
}
