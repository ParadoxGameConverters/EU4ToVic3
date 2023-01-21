#include "CoastalMapper/CoastalMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_CoastalMapperTests, MismatchReturnsEmpty)
{
	mappers::CoastalMapper mapper;
	mapper.loadAdjacencies("TestFiles/configurables/province_adjacencies.txt");

	EXPECT_TRUE(mapper.getAdjacencies("mismatch").empty());
}

TEST(Mappers_CoastalMapperTests, AdjacenciesCanBeRetrieved)
{
	mappers::CoastalMapper mapper;
	mapper.loadAdjacencies("TestFiles/configurables/province_adjacencies.txt");

	EXPECT_THAT(mapper.getAdjacencies("x00004B"), testing::UnorderedElementsAre("x005ED2", "x184708", "x371070"));
	EXPECT_THAT(mapper.getAdjacencies("x000090"), testing::UnorderedElementsAre("x638C4B", "xAD371C", "xBF2D59"));
}