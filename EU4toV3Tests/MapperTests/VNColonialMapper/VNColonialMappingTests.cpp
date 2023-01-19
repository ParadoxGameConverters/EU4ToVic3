#include "VNColonialMapper/VNColonialMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_VNColonialMappingTests, MappingDefaultsToDefaults)
{
	std::stringstream input;
	const mappers::VNColonialMapping theMapping(input);

	EXPECT_TRUE(theMapping.getName().empty());
	EXPECT_TRUE(theMapping.getOwner().empty());
	EXPECT_TRUE(theMapping.getDecolonizeBlocker().empty());
	EXPECT_TRUE(theMapping.getKeyProvince().empty());
	EXPECT_TRUE(theMapping.getStates().empty());
	EXPECT_TRUE(theMapping.getSubjects().empty());
	EXPECT_TRUE(theMapping.getSubjectType().empty());
}

TEST(Mappers_VNColonialMappingTests, StateMappingCanBeLoaded)
{
	std::stringstream input;
	input << R"(name = "Greenland" key = x8B7790 states = { STATE_ICELAND STATE_GREENLAND } owner = DEN decolonize = FRA )";
	const mappers::VNColonialMapping theMapping(input);

	EXPECT_EQ("Greenland", theMapping.getName());
	EXPECT_EQ("DEN", theMapping.getOwner());
	EXPECT_EQ("FRA", theMapping.getDecolonizeBlocker());
	EXPECT_EQ("x8B7790", theMapping.getKeyProvince());
	EXPECT_THAT(theMapping.getStates(), testing::UnorderedElementsAre("STATE_ICELAND", "STATE_GREENLAND"));
}

TEST(Mappers_VNColonialMappingTests, SubjectMappingCanBeLoaded)
{
	std::stringstream input;
	input << R"(name = "Australia" key = xE0E0C0 subjects = { NSW WAS SAS TAS } type = puppet )";
	const mappers::VNColonialMapping theMapping(input);

	EXPECT_EQ("Australia", theMapping.getName());
	EXPECT_EQ("xE0E0C0", theMapping.getKeyProvince());
	EXPECT_THAT(theMapping.getSubjects(), testing::UnorderedElementsAre("NSW", "WAS", "SAS", "TAS"));
	EXPECT_EQ("puppet", theMapping.getSubjectType());
}
