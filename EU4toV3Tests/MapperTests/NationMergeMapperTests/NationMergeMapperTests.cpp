#include "NationMergeMapper/NationMergeMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::UnorderedElementsAre;

TEST(Mappers_NationMergeMapperTests, primitivesDefaultToDefaults)
{
	std::stringstream input;
	mappers::NationMergeMapper mapper;
	mapper.loadNationMerge(input);

	EXPECT_FALSE(mapper.getMergeDaimyos());
	EXPECT_TRUE(mapper.getMergeBlocks().empty());
}

TEST(Mappers_NationMergeMapperTests, mergeDaimyosCanBeSet)
{
	std::stringstream input;
	input << "merge_daimyos = yes\n";
	mappers::NationMergeMapper mapper;
	mapper.loadNationMerge(input);

	EXPECT_TRUE(mapper.getMergeDaimyos());
}

TEST(Mappers_NationMergeMapperTests, mergeBlocksCanBeLoaded)
{
	std::stringstream input;
	input << "france = {\n";
	input << "	master = FRA\n";
	input << "	slave = ALE\n";
	input << "	slave = ALS\n";
	input << "	merge = yes\n";
	input << "}\n";
	input << "ireland = {\n";
	input << "	master = IRE\n";
	input << "	slave = ULS\n";
	input << "	slave = TYR\n";
	input << "	merge = no\n";
	input << "}\n";
	mappers::NationMergeMapper mapper;
	mapper.loadNationMerge(input);

	ASSERT_EQ(2, mapper.getMergeBlocks().size());
	const auto& france = mapper.getMergeBlocks()[0];
	const auto& ireland = mapper.getMergeBlocks()[1];

	EXPECT_EQ("FRA", france.getMaster());
	EXPECT_THAT(france.getSlaves(), UnorderedElementsAre("ALE", "ALS"));
	EXPECT_TRUE(france.shouldMerge());

	EXPECT_EQ("IRE", ireland.getMaster());
	EXPECT_THAT(ireland.getSlaves(), UnorderedElementsAre("ULS", "TYR"));
	EXPECT_FALSE(ireland.shouldMerge());
}
