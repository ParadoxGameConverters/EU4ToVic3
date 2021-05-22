#include "NationMerger/MergeBlock.h"
#include "gtest/gtest.h"

TEST(EU4World_MergeBlockTests, primitivesDefaultToDefaults)
{
	std::stringstream input;
	const EU4::MergeBlock block(input);

	ASSERT_TRUE(block.getMaster().empty());
	ASSERT_TRUE(block.getSlaves().empty());
	ASSERT_FALSE(block.getMerge());
}

TEST(EU4World_MergeBlockTests, mergeBlockCanBeLoaded)
{
	std::stringstream input;
	input << "master = FRA\n";
	input << "slave = ALE\n";
	input << "slave = ALS\n";
	input << "merge = yes\n";

	const EU4::MergeBlock block(input);
	ASSERT_EQ("FRA", block.getMaster());
	ASSERT_EQ(2, block.getSlaves().size());
	ASSERT_TRUE(block.getSlaves().count("ALE"));
	ASSERT_TRUE(block.getSlaves().count("ALS"));
	ASSERT_TRUE(block.getMerge());
}
