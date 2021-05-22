#include "ColonialTags/ColonialTag.h"
#include "gtest/gtest.h"

TEST(Mappers_ColonialTagTests, primitivesDefaultToDefaults)
{
	std::stringstream input;
	const mappers::ColonialTag tag(input);

	ASSERT_TRUE(tag.getColonyTag().tag.empty());
	ASSERT_TRUE(tag.getColonyTag().EU4Regions.empty());
	ASSERT_TRUE(tag.getColonyTag().V2Regions.empty());
	ASSERT_TRUE(tag.getColonyTag().cultureGroups.empty());
}

TEST(Mappers_ColonialTagTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "tag = TAG eu4region = e4r1 eu4region = e4r2 v2region = v2r1 v2region = v2r2 cultureGroup = cg1 cultureGroup = cg2\n";
	const mappers::ColonialTag tag(input);

	ASSERT_EQ("TAG", tag.getColonyTag().tag);
	ASSERT_TRUE(tag.getColonyTag().EU4Regions.contains("e4r1"));
	ASSERT_TRUE(tag.getColonyTag().EU4Regions.contains("e4r2"));
	ASSERT_TRUE(tag.getColonyTag().V2Regions.contains("v2r1"));
	ASSERT_TRUE(tag.getColonyTag().V2Regions.contains("v2r2"));
	ASSERT_TRUE(tag.getColonyTag().cultureGroups.contains("cg1"));
	ASSERT_TRUE(tag.getColonyTag().cultureGroups.contains("cg2"));
}

TEST(Mappers_ColonialTagTests, matchReturnsTrueForAllCorrectQualifiers)
{
	std::stringstream input;
	input << "tag = ORA eu4region = testRegion1 v2region = testRegion2 cultureGroup = beneluxian\n";
	const mappers::ColonialTag tag(input);

	ASSERT_TRUE(tag.getColonyTag().match("testRegion1", "testRegion2", "beneluxian"));
}

TEST(Mappers_ColonialTagTests, matchReturnsFalseForAnyIncorrectQualifiers)
{
	std::stringstream input;
	input << "tag = ORA eu4region = testTegion1 v2region = testRegion2 cultureGroup = beneluxian\n";
	const mappers::ColonialTag tag(input);

	ASSERT_FALSE(tag.getColonyTag().match("testRegion", "testRegion2", "beneluxian"));
	ASSERT_FALSE(tag.getColonyTag().match("testRegion1", "testRegion", "beneluxian"));
	ASSERT_FALSE(tag.getColonyTag().match("testRegion1", "testRegion2", "beneluxian1"));
}

TEST(Mappers_ColonialTagTests, matchReturnsTrueForExcessiveQualifiers)
{
	std::stringstream input;
	input << "tag = ORA cultureGroup = beneluxian\n";
	const mappers::ColonialTag tag(input);

	ASSERT_TRUE(tag.getColonyTag().match("south_africa_region", "ENG_2087", "beneluxian"));
	ASSERT_TRUE(tag.getColonyTag().match("south_africa_region", "", "beneluxian"));
	ASSERT_TRUE(tag.getColonyTag().match("", "ENG_2087", "beneluxian"));
	ASSERT_TRUE(tag.getColonyTag().match("", "", "beneluxian"));
}

TEST(Mappers_ColonialTagTests, matchReturnsFalseForInsufficientQualifiers)
{
	std::stringstream input;
	input << "tag = ORA eu4region = south_africa_region v2region = ENG_2087 cultureGroup = beneluxian\n";
	const mappers::ColonialTag tag(input);

	ASSERT_FALSE(tag.getColonyTag().match("south_africa_region", "", ""));
	ASSERT_FALSE(tag.getColonyTag().match("", "ENG_2087", ""));
	ASSERT_FALSE(tag.getColonyTag().match("", "", "beneluxian"));
	ASSERT_FALSE(tag.getColonyTag().match("south_africa_region", "", "beneluxian"));
	ASSERT_FALSE(tag.getColonyTag().match("south_africa_region", "ENG_2087", ""));
	ASSERT_FALSE(tag.getColonyTag().match("", "ENG_2087", "beneluxian"));
}
