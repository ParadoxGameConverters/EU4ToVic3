#include "ProvinceMapper/ProvinceMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_ProvinceMappingTests, MappingsDefaultToEmpty)
{
	std::stringstream input;
	const mappers::ProvinceMapping theMapper(input);

	EXPECT_TRUE(theMapper.getEU4Provinces().empty());
	EXPECT_TRUE(theMapper.getV3Provinces().empty());
}

TEST(Mappers_ProvinceMappingTests, EU4ProvincesCanBeAdded)
{
	std::stringstream input;
	input << "eu4 = 2 eu4 = 1";
	const mappers::ProvinceMapping theMapper(input);

	EXPECT_THAT(theMapper.getEU4Provinces(), testing::UnorderedElementsAre(1, 2));
}

TEST(Mappers_ProvinceMappingTests, Vic33ProvincesCanBeAdded)
{
	std::stringstream input;
	input << "vic3 = x654321 vic3 = x123456";
	const mappers::ProvinceMapping theMapper(input);

	EXPECT_THAT(theMapper.getV3Provinces(), testing::UnorderedElementsAre("x654321", "x123456"));
}
