#include "NationMergeMapper/MergeBlock.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::UnorderedElementsAre;

TEST(Mappers_MergeBlockTests, primitivesDefaultToDefaults)
{
	std::stringstream input;
	const mappers::MergeBlock block(input);

	EXPECT_TRUE(block.getMaster().empty());
	EXPECT_TRUE(block.getSlaves().empty());
	EXPECT_FALSE(block.shouldMerge());
}

TEST(Mappers_MergeBlockTests, mergeBlockCanBeLoaded)
{
	std::stringstream input;
	input << "master = FRA\n";
	input << "slave = ALE\n";
	input << "slave = ALS\n";
	input << "merge = yes\n";

	const mappers::MergeBlock block(input);
	EXPECT_EQ("FRA", block.getMaster());
	EXPECT_THAT(block.getSlaves(), UnorderedElementsAre("ALE", "ALS"));
	EXPECT_TRUE(block.shouldMerge());
}
