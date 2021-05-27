#include "ReligionLoader/ReligionLoader.h"
#include "ReligionLoader/ReligionParser.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::UnorderedElementsAre;

TEST(EU4World_ReligionLoaderTests, religionsDefaultToEmpty)
{
	std::stringstream input;
	EU4::ReligionLoader theReligions;
	theReligions.loadReligions(input);

	EXPECT_TRUE(theReligions.getAllReligions().empty());
}

TEST(EU4World_ReligionLoaderTests, religionCanBeImported)
{
	std::stringstream input;
	input << "religion_group = {\n";
	input << "\treligion = {}\n";
	input << "}";
	EU4::ReligionLoader theReligions;
	theReligions.loadReligions(input);

	EXPECT_THAT(theReligions.getAllReligions(), UnorderedElementsAre("religion"));
}

TEST(EU4World_ReligionLoaderTests, multipleReligionsCanBeImported)
{
	std::stringstream input;
	input << "religion_group = {\n";
	input << "\treligion = {}\n";
	input << "\tanother_religion = {}\n";
	input << "}";
	EU4::ReligionLoader theReligions;
	theReligions.loadReligions(input);

	EXPECT_THAT(theReligions.getAllReligions(), UnorderedElementsAre("religion", "another_religion"));
}

TEST(EU4World_ReligionLoaderTests, multipleReligionGroupsCanBeImported)
{
	std::stringstream input;
	input << "religion_group = {\n";
	input << "\treligion = {}\n";
	input << "}\n";
	input << "another_religion_group = {\n";
	input << "\tanother_religion = {}\n";
	input << "}";
	EU4::ReligionLoader theReligions;
	theReligions.loadReligions(input);

	EXPECT_THAT(theReligions.getAllReligions(), UnorderedElementsAre("religion", "another_religion"));
}
