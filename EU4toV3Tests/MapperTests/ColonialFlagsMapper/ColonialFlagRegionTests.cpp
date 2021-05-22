#include "ColonialFlags/ColonialFlagRegion.h"
#include "gtest/gtest.h"

TEST(Mappers_ColonialFlagRegionTests, flagsDefaultToEmpty)
{
	std::stringstream input;
	const mappers::ColonialFlagRegion region(input, "region");

	ASSERT_TRUE(region.getRegionalFlags().empty());
}

TEST(Mappers_ColonialFlagRegionTests, flagsCanBeLoaded)
{
	std::stringstream input;
	input << "flag = { name = \"Alaska\" }\n";
	input << "flag = { name = \"Alyeska\" unique = true }\n";
	const mappers::ColonialFlagRegion region(input, "region");

	ASSERT_EQ(2, region.getRegionalFlags().size());
	ASSERT_EQ("alaska", region.getRegionalFlags().at("alaska").getName());
	ASSERT_EQ("alyeska", region.getRegionalFlags().at("alyeska").getName());
	ASSERT_TRUE(region.getRegionalFlags().at("alyeska").isUnique());
}

TEST(Mappers_ColonialFlagRegionTests, regionsIsPassedDownToFlags)
{
	std::stringstream input;
	input << "flag = { name = \"Alaska\" }\n";
	input << "flag = { name = \"Alyeska\" unique = true }\n";
	const mappers::ColonialFlagRegion region(input, "region");

	ASSERT_EQ("region", region.getRegionalFlags().at("alaska").getRegion());
	ASSERT_EQ("region", region.getRegionalFlags().at("alyeska").getRegion());
}
