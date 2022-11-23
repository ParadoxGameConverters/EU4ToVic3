#include "ReligionMapper/ReligionMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_ReligionMapperTests, religionsCanBeRetrieved)
{
	mappers::ReligionMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/religion_map.txt");

	EXPECT_EQ("catholic", *mapper.getV3Religion("catholic"));
	EXPECT_EQ("catholic", *mapper.getV3Religion("insular_celtic"));
	EXPECT_EQ("protestant", *mapper.getV3Religion("protestant"));
	EXPECT_EQ("protestant", *mapper.getV3Religion("reformed"));
	EXPECT_EQ("protestant", *mapper.getV3Religion("anglican"));
}

TEST(Mappers_ReligionMapperTests, MisMapReturnsNullopt)
{
	mappers::ReligionMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/religion_map.txt");

	EXPECT_EQ(std::nullopt, mapper.getV3Religion("unmapped"));
}
