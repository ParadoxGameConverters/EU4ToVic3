#include "Mappers/SuperGroupMapper/SuperGroupMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_SuperGroupMapperTests, superGroupsCanBeLoadedAndPinged)
{
	std::stringstream input;
	input << "sgroup1 = { sregion1 = {assimilation = 2}}\n";
	input << "sgroup2 = { sregion2 = {assimilation = 4}}\n";
	mappers::SuperGroupMapper superGroupMapper;
	superGroupMapper.loadSuperGroups(input);

	EXPECT_NEAR(0.004375, superGroupMapper.getAssimilationFactor("sregion1"), 0.0001);
	EXPECT_NEAR(0.00683594, superGroupMapper.getAssimilationFactor("sregion2"), 0.0001);

	EXPECT_EQ("sgroup1", superGroupMapper.getGroupForSuperRegion("sregion1"));
	EXPECT_EQ("sgroup2", superGroupMapper.getGroupForSuperRegion("sregion2"));
}

TEST(Mappers_SuperGroupMapperTests, superGroupNameMismatchReturnsNullopt)
{
	std::stringstream input;
	input << "sgroup1 = { sregion1 = {assimilation = 5}}\n";
	input << "sgroup2 = { sregion2 = {assimilation = 8}}\n";
	mappers::SuperGroupMapper superGroupMapper;
	superGroupMapper.loadSuperGroups(input);

	EXPECT_EQ(std::nullopt, superGroupMapper.getGroupForSuperRegion("sregion3"));
}

TEST(Mappers_SuperGroupMapperTests, superGroupAssimilationMismatchReturnsDefaultAssimilation)
{
	std::stringstream input;
	input << "sgroup1 = { sregion1 = {assimilation = 5}}\n";
	input << "sgroup2 = { sregion2 = {assimilation = 8}}\n";
	mappers::SuperGroupMapper superGroupMapper;
	superGroupMapper.loadSuperGroups(input);

	EXPECT_DOUBLE_EQ(0.0025, superGroupMapper.getAssimilationFactor("sregion3"));
}
