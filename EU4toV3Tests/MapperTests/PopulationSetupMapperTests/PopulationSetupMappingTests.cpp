#include "PopulationSetupMapper/PopulationSetupMapping.h"
#include "gtest/gtest.h"

TEST(Mappers_PopulationSetupMappingTests, DefaultsDefaultToDefaults)
{
	std::stringstream input;
	const mappers::PopulationSetupMapping mapping(input);

	EXPECT_FALSE(mapping.getLiteracy());
	EXPECT_FALSE(mapping.getAverageDev());
	EXPECT_TRUE(mapping.getEffect().empty());
}

TEST(Mappers_PopulationSetupMappingTests, mappingCanBeLoaded)
{
	std::stringstream input;
	input << "effect = theEffect literacy = 0.98 averagedev = 45.98\n";

	const mappers::PopulationSetupMapping mapping(input);
	EXPECT_DOUBLE_EQ(0.98, *mapping.getLiteracy());
	EXPECT_DOUBLE_EQ(45.98, *mapping.getAverageDev());
	EXPECT_EQ("theEffect", mapping.getEffect());
}
