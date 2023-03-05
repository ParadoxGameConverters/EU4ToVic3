#include "ColonialRegionMapper/ColonialRegionMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_ColonialRegionMappingTests, DefaultsDefaultToDefaults)
{

	std::stringstream input;
	const mappers::ColonialRegionMapping colonialRegionMapping(input);

	EXPECT_TRUE(colonialRegionMapping.getRegions().empty());
	EXPECT_TRUE(colonialRegionMapping.getAloneName().empty());
	EXPECT_TRUE(colonialRegionMapping.getSplitName().empty());
}

TEST(Mappers_ColonialRegionMappingTests, RulesCanBeLoaded)
{
	std::stringstream input;
	input << R"(region = region_new_england region = STATE_CALIFORNIA region = STATE_WASHINGTON alone_name = "Appalachian" split_name = "$PARENT$ - American")";
	const mappers::ColonialRegionMapping colonialRegionMapping(input);

	EXPECT_THAT(colonialRegionMapping.getRegions(), testing::UnorderedElementsAre("region_new_england", "STATE_CALIFORNIA", "STATE_WASHINGTON"));
	EXPECT_EQ("Appalachian", colonialRegionMapping.getAloneName());
	EXPECT_EQ("$PARENT$ - American", colonialRegionMapping.getSplitName());
}
