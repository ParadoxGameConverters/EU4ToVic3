#include "CultureMapper/WesternizationMapper/WesternizationMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_WesternizationMappingTests, DefaultsDefaultToDefaults)
{
	std::stringstream input;
	const mappers::WesternizationMapping mapping(input);

	EXPECT_EQ(5, mapping.getWesternization());
	EXPECT_EQ(5, mapping.getLiteracy());
	EXPECT_EQ(5, mapping.getIndustry());
	EXPECT_TRUE(mapping.getTraits().empty());
}

TEST(Mappers_WesternizationMappingTests, mappingCanBeLoaded)
{
	std::stringstream input;
	input << "westernization = 1\n";
	input << "literacy = 8\n";
	input << "industry = 0\n";
	input << "traits = {\n";
	input << "	trait1 trait2\n";
	input << "}\n";

	const mappers::WesternizationMapping mapping(input);
	EXPECT_EQ(1, mapping.getWesternization());
	EXPECT_EQ(8, mapping.getLiteracy());
	EXPECT_EQ(0, mapping.getIndustry());
	EXPECT_THAT(mapping.getTraits(), testing::UnorderedElementsAre("trait1", "trait2"));
}
