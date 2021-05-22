#include "Religions/Religions.h"
#include "gtest/gtest.h"

TEST(EU4World_ReligionsTests, religionsDefaultToEmpty)
{
	std::stringstream input;
	const EU4::Religions theReligions(input);

	ASSERT_TRUE(theReligions.getAllReligions().empty());
}

TEST(EU4World_ReligionsTests, religionCanBeImported)
{
	std::stringstream input;
	input << "religion_group = {\n";
	input << "\treligion = {}\n";
	input << "}";
	const EU4::Religions theReligions(input);

	ASSERT_EQ(1, theReligions.getAllReligions().size());
	ASSERT_TRUE(theReligions.getAllReligions().contains("religion"));
}

TEST(EU4World_ReligionsTests, multipleReligionCanBeImported)
{
	std::stringstream input;
	input << "religion_group = {\n";
	input << "\treligion = {}\n";
	input << "\tanother_religion = {}\n";
	input << "}";
	const EU4::Religions theReligions(input);

	ASSERT_EQ(theReligions.getAllReligions().size(), 2);
	ASSERT_TRUE(theReligions.getAllReligions().contains("religion"));
	ASSERT_TRUE(theReligions.getAllReligions().contains("another_religion"));
}

TEST(EU4World_ReligionsTests, multipleReligionGroupsCanBeImported)
{
	std::stringstream input;
	input << "religion_group = {\n";
	input << "\treligion = {}\n";
	input << "}\n";
	input << "another_religion_group = {\n";
	input << "\tanother_religion = {}\n";
	input << "}";
	const EU4::Religions theReligions(input);

	ASSERT_EQ(theReligions.getAllReligions().size(), 2);
	ASSERT_TRUE(theReligions.getAllReligions().contains("religion"));
	ASSERT_TRUE(theReligions.getAllReligions().contains("another_religion"));
}
