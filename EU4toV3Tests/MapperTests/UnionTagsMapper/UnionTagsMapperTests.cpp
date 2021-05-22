#include "UnionTagsMapper/UnionTagsMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_UnionTagsMapperTests, unionfullTagCanBeMatched)
{
	std::stringstream input;
	input << "union_tags = { D00 ARA } ";
	const mappers::UnionTagsMapper mapper(input);

	ASSERT_TRUE(mapper.isTagUnionfull("D00"));
	ASSERT_TRUE(mapper.isTagUnionfull("ARA"));
}

TEST(Mappers_UnionTagsMapperTests, mismatchReturnsFalse)
{
	std::stringstream input;
	input << "union_tags = { D00 ARA } ";
	const mappers::UnionTagsMapper mapper(input);

	ASSERT_FALSE(mapper.isTagUnionfull("D01"));
	ASSERT_FALSE(mapper.isTagUnionfull("ARB"));
}
