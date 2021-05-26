#include "ReligionLoader/ReligionLoader.h"
#include "ReligionLoader/ReligionParser.h"
#include "gtest/gtest.h"

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

	EXPECT_EQ(1, theReligions.getAllReligions().size());
	EXPECT_TRUE(theReligions.getAllReligions().contains("religion"));
}

TEST(EU4World_ReligionLoaderTests, multipleReligionCanBeImported)
{
	std::stringstream input;
	input << "religion_group = {\n";
	input << "\treligion = {}\n";
	input << "\tanother_religion = {}\n";
	input << "}";
	EU4::ReligionLoader theReligions;
	theReligions.loadReligions(input);

	EXPECT_EQ(theReligions.getAllReligions().size(), 2);
	EXPECT_TRUE(theReligions.getAllReligions().contains("religion"));
	EXPECT_TRUE(theReligions.getAllReligions().contains("another_religion"));
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

	EXPECT_EQ(theReligions.getAllReligions().size(), 2);
	EXPECT_TRUE(theReligions.getAllReligions().contains("religion"));
	EXPECT_TRUE(theReligions.getAllReligions().contains("another_religion"));
}
