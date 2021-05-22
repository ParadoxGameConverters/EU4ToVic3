#include "FlagColors/FlagColorMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_FlagColorMapperTests, flagColorsStartEmpty)
{
	std::stringstream input;
	const mappers::FlagColorMapper mapper(input);

	ASSERT_EQ(0, mapper.getNumFlagColors());
}

TEST(Mappers_FlagColorMapperTests, flagColorsCanBeLoaded)
{
	std::stringstream input;
	input << "flag_color = { 0 1 2 }\n";
	input << "flag_color = { 3 4 5 }\n";
	input << "flag_color = { 6 7 8 }\n";
	const mappers::FlagColorMapper mapper(input);

	ASSERT_EQ(3, mapper.getNumFlagColors());
}

TEST(Mappers_FlagColorMapperTests, colorsCanBeRetrieved)
{
	std::stringstream input;
	input << "flag_color = { 0 1 2 }\n";
	input << "flag_color = { 3 4 5 }\n";
	input << "flag_color = { 6 7 8 }\n";
	const mappers::FlagColorMapper mapper(input);

	const auto& color2 = mapper.getFlagColorByIndex(1);

	ASSERT_EQ("= rgb { 3 4 5 }", color2->outputRgb());
}

TEST(Mappers_FlagColorMapperTests, mismatchReturnsNullopt)
{
	std::stringstream input;
	input << "flag_color = { 0 1 2 }\n";
	input << "flag_color = { 3 4 5 }\n";
	input << "flag_color = { 6 7 8 }\n";
	const mappers::FlagColorMapper mapper(input);

	const auto& color5 = mapper.getFlagColorByIndex(4);
	ASSERT_EQ(std::nullopt, color5);
}
