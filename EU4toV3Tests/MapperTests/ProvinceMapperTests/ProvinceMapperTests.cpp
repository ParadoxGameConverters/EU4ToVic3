#include "ProvinceMapper/ProvinceMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_ProvinceMapperTests, mappingsCanBeLoadedAndCreated)
{
	mappers::ProvinceMapper provinceMapper;
	provinceMapper.loadProvinceMappings("TestFiles/configurables/province_mappings.txt");

	EXPECT_EQ(4, provinceMapper.getMappings().size());

	EXPECT_THAT(provinceMapper.getEU4Provinces("xA2E8C5"), testing::UnorderedElementsAre(1096));
	EXPECT_THAT(provinceMapper.getEU4Provinces("x90A080"), testing::UnorderedElementsAre(366, 367));
	EXPECT_THAT(provinceMapper.getV3Provinces(1096), testing::UnorderedElementsAre("xA2E8C5", "x4B7FA5", "xE8E7A2", "xA2C3E8"));
	EXPECT_THAT(provinceMapper.getV3Provinces(366), testing::UnorderedElementsAre("x90A080", "x9B6D87", "x769456"));
}

TEST(Mappers_ProvinceMapperTests, mappingsForObsoleteVersionsAreIgnored)
{
	mappers::ProvinceMapper provinceMapper;
	provinceMapper.loadProvinceMappings("TestFiles/configurables/province_mappings.txt");

	EXPECT_TRUE(provinceMapper.getEU4Provinces("x111111").empty());
	EXPECT_TRUE(provinceMapper.getV3Provinces(1).empty());
}


TEST(Mappers_ProvinceMapperTests, mappingsForDroppedEU4ProvincesAreNotCreated)
{
	mappers::ProvinceMapper provinceMapper;
	provinceMapper.loadProvinceMappings("TestFiles/configurables/province_mappings.txt");

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	EXPECT_TRUE(provinceMapper.getV3Provinces(368).empty());

	std::cout.rdbuf(cout_buffer);
}

TEST(Mappers_ProvinceMapperTests, mappingsForDroppedV3ProvincesAreNotCreated)
{
	mappers::ProvinceMapper provinceMapper;
	provinceMapper.loadProvinceMappings("TestFiles/configurables/province_mappings.txt");

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	EXPECT_TRUE(provinceMapper.getEU4Provinces("x655491").empty());

	std::cout.rdbuf(cout_buffer);
}
