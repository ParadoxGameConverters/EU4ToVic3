#include "Geography/TerrainDataMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_Geography_TerrainDataMapperTests, terrainCanBePinged)
{
	std::stringstream input;
	input << "1 = arctic\n";
	input << "2 = goldilocks\n";
	const mappers::TerrainDataMapper mapper(input);

	ASSERT_EQ("goldilocks", *mapper.getTerrainForID(2));
}

TEST(Mappers_Geography_TerrainDataMapperTests, mispingReturnsNullopt)
{
	std::stringstream input;
	input << "1 = arctic\n";
	input << "2 = goldilocks\n";
	const mappers::TerrainDataMapper mapper(input);

	ASSERT_EQ(std::nullopt, mapper.getTerrainForID(3));
}
