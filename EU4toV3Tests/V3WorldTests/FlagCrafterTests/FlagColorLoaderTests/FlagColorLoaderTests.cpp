#include "FlagCrafter/FlagColorLoader/FlagColorLoader.h"
#include "gtest/gtest.h"

TEST(Mappers_FlagColorLoaderTests, flagColorsStartEmpty)
{
	std::stringstream input;
	const V3::FlagColorLoader mapper;

	EXPECT_EQ(0, mapper.getNumFlagColors());
}

TEST(Mappers_FlagColorLoaderTests, flagColorsCanBeLoaded)
{
	V3::FlagColorLoader mapper;
	mapper.loadFlagColors("TestFiles/eu4installation/common/custom_country_colors/00_custom_country_colors.txt");

	EXPECT_EQ(3, mapper.getNumFlagColors());
}

TEST(Mappers_FlagColorLoaderTests, colorsCanBeRetrieved)
{
	V3::FlagColorLoader mapper;
	mapper.loadFlagColors("TestFiles/eu4installation/common/custom_country_colors/00_custom_country_colors.txt");

	const auto& color2 = mapper.getFlagColorByIndex(1);

	ASSERT_TRUE(color2);
	EXPECT_EQ("= rgb { 3 4 5 }", color2->outputRgb());
}

TEST(Mappers_FlagColorLoaderTests, mismatchReturnsNullopt)
{
	V3::FlagColorLoader mapper;
	mapper.loadFlagColors("TestFiles/eu4installation/common/custom_country_colors/00_custom_country_colors.txt");

	const auto& color5 = mapper.getFlagColorByIndex(4);
	EXPECT_EQ(std::nullopt, color5);
}
