#include "NationMerger/NationMergeParser.h"
#include "gtest/gtest.h"

TEST(EU4World_NationMergeParserTests, primitivesDefaultToDefaults)
{
	std::stringstream input;
	const EU4::NationMergeParser parser(input);

	ASSERT_FALSE(parser.getMergeDaimyos());
	ASSERT_TRUE(parser.getMergeBlocks().empty());
}

TEST(EU4World_NationMergeParserTests, mergeDaimyosCanBeSet)
{
	std::stringstream input;
	input << "merge_daimyos = yes\n";
	const EU4::NationMergeParser parser(input);

	ASSERT_TRUE(parser.getMergeDaimyos());
}

TEST(EU4World_NationMergeParserTests, mergeBlocksCanBeLoaded)
{
	std::stringstream input;
	input << "france = {\n";
	input << "\tmaster = FRA\n";
	input << "\tslave = ALE\n";
	input << "\tslave = ALS\n";
	input << "\tmerge = yes\n";
	input << "}\n";
	input << "\tireland = {\n";
	input << "\tmaster = IRE\n";
	input << "\tslave = ULS\n";
	input << "\tslave = TYR\n";
	input << "\tmerge = no\n";
	input << "}\n";
	const EU4::NationMergeParser parser(input);

	const auto& france = parser.getMergeBlocks()[0];
	const auto& ireland = parser.getMergeBlocks()[1];

	ASSERT_EQ(2, parser.getMergeBlocks().size());
	ASSERT_EQ("FRA", france.getMaster());
	ASSERT_EQ("IRE", ireland.getMaster());
}
