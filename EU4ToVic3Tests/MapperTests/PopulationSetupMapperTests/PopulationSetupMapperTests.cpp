#include "PopulationSetupMapper/PopulationSetupMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_PopulationSetupMapperTests, LiteracyLevelsCanBeDetermined)
{
	mappers::PopulationSetupMapper mapper;
	std::stringstream input;
	input << "link = { effect = level1 literacy = 0.0 }\n";
	input << "link = { effect = level2 literacy = 0.1 }\n";
	input << "link = { effect = level3 literacy = 0.2 }\n";
	input << "link = { effect = level4 literacy = 0.5 }\n";
	input << "link = { effect = level5 literacy = 0.7 }\n";
	mapper.loadMappingRules(input);

	EXPECT_EQ("level1", mapper.getLiteracyEffectForLiteracy(0));
	EXPECT_EQ("level2", mapper.getLiteracyEffectForLiteracy(0.1));
	EXPECT_EQ("level3", mapper.getLiteracyEffectForLiteracy(0.25));
	EXPECT_EQ("level4", mapper.getLiteracyEffectForLiteracy(0.5));
	EXPECT_EQ("level5", mapper.getLiteracyEffectForLiteracy(0.79));
}

TEST(Mappers_PopulationSetupMapperTests, WealthLevelsCanBeDetermined)
{
	mappers::PopulationSetupMapper mapper;
	std::stringstream input;
	input << "link = { effect = level1 averagedev = 0 }\n";
	input << "link = { effect = level2 averagedev = 10 }\n";
	input << "link = { effect = level3 averagedev = 20 }\n";
	input << "link = { effect = level4 averagedev = 50 }\n";
	input << "link = { effect = level5 averagedev = 70 }\n";
	mapper.loadMappingRules(input);

	EXPECT_EQ("level1", mapper.getWealthEffectForDev(0));
	EXPECT_EQ("level1", mapper.getWealthEffectForDev(1));
	EXPECT_EQ("level3", mapper.getWealthEffectForDev(25));
	EXPECT_EQ("level4", mapper.getWealthEffectForDev(50));
	EXPECT_EQ("level5", mapper.getWealthEffectForDev(79));
}
