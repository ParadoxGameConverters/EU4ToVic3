#include "ReligionMapper/ReligionMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_ReligionMappingTests, DefaultsDefaultToDefaults)
{
	std::stringstream input;
	const mappers::ReligionMapping mapping(input);

	EXPECT_TRUE(mapping.getV3Religion().empty());
	EXPECT_TRUE(mapping.getEU4Religions().empty());
}

TEST(Mappers_ReligionMappingTests, RulesCanBeLoaded)
{
	std::stringstream input;
	input << "vic3 = vic eu4 = eu1 eu4 = eu1 eu4 = eu2";
	const mappers::ReligionMapping mapping(input);

	EXPECT_EQ("vic", mapping.getV3Religion());
	EXPECT_THAT(mapping.getEU4Religions(), testing::UnorderedElementsAre("eu1", "eu2"));
}
