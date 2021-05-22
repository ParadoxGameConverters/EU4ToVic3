#include "Geography/ClimateMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_Geography_ClimateMapperTests, primitivesDefaultToEmpty)
{
	std::stringstream input;
	const mappers::ClimateMapper mapper(input);

	ASSERT_TRUE(mapper.getClimateMap().empty());
}

TEST(Mappers_Geography_ClimateMapperTests, firstClimateIsSkipped)
{
	std::stringstream input;
	input << "funny_climate = { irrelevant }";
	const mappers::ClimateMapper mapper(input);

	ASSERT_TRUE(mapper.getClimateMap().empty());
}

TEST(Mappers_Geography_ClimateMapperTests, secondClimateIsLoaded)
{
	std::stringstream input;
	input << "funny_climate = { irrelevant }";
	input << "funny_climate = { 1 2 3 }";
	const mappers::ClimateMapper mapper(input);

	ASSERT_EQ(1, mapper.getClimateMap().size());
	const std::vector<int> expected = {1, 2, 3};
	ASSERT_EQ(expected, mapper.getClimateMap().at("funny_climate"));
}

TEST(Mappers_Geography_ClimateMapperTests, multipleClimatesCanBeLoaded)
{
	std::stringstream input;
	input << "funny_climate = { irrelevant }";
	input << "funny_climate = { 1 2 3 }";
	input << "sad_climate = { irrelevant }";
	input << "sad_climate = { 4 5 6 }";
	input << "bored_climate = { irrelevant }";
	input << "bored_climate = { 7 8 9 }";
	const mappers::ClimateMapper mapper(input);

	ASSERT_EQ(3, mapper.getClimateMap().size());
	ASSERT_TRUE(mapper.getClimateMap().contains("funny_climate"));
	ASSERT_TRUE(mapper.getClimateMap().contains("sad_climate"));
	ASSERT_TRUE(mapper.getClimateMap().contains("bored_climate"));
}
