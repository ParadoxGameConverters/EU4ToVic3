#include "ProvinceMapper/ProvinceMappingsVersion.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_ProvinceMappingsVersionTests, mappingsDefaultToEmpty)
{
	std::stringstream input;
	mappers::ProvinceMappingsVersion theMappingVersion(input);

	EXPECT_TRUE(theMappingVersion.getMappings().empty());
}

TEST(Mappers_ProvinceMappingsVersionTests, commentMappingsAreNotLoaded)
{
	std::stringstream input;
	input << "link = { comment = \"+++EUROPE+++\" }\n";
	mappers::ProvinceMappingsVersion theMappingVersion(input);

	EXPECT_TRUE(theMappingVersion.getMappings().empty());
}

TEST(Mappers_ProvinceMappingsVersionTests, emptyMappingsAreNotLoaded)
{
	std::stringstream input;
	input << "link = { }\n";
	mappers::ProvinceMappingsVersion theMappingVersion(input);

	EXPECT_TRUE(theMappingVersion.getMappings().empty());
}

TEST(Mappers_ProvinceMappingsVersionTests, MappingsCanBeLoaded)
{
	std::stringstream input;
	input << "link = { eu4 = 1 eu4 = 2 vic3 = x123456 }\n";
	input << "link = { eu4 = 3 vic3 = xABCDEF vic3 = xFEDCBA}\n";

	mappers::ProvinceMappingsVersion theMappingVersion(input);

	EXPECT_EQ(2, theMappingVersion.getMappings().size());

	const auto& mapping1 = theMappingVersion.getMappings()[0];
	const auto& mapping2 = theMappingVersion.getMappings()[1];

	EXPECT_THAT(mapping1.getEU4Provinces(), testing::UnorderedElementsAre(1, 2));
	EXPECT_THAT(mapping1.getV3Provinces(), testing::UnorderedElementsAre("x123456"));
	EXPECT_THAT(mapping2.getEU4Provinces(), testing::UnorderedElementsAre(3));
	EXPECT_THAT(mapping2.getV3Provinces(), testing::UnorderedElementsAre("xABCDEF", "xFEDCBA"));
}