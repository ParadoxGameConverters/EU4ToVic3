#include "BlockedTechSchools/BlockedTechSchools.h"
#include "gtest/gtest.h"

TEST(Mappers_BlockedSchoolsTests, techSchoolCanBeBlocked)
{
	std::stringstream input;
	input << "blocked = a_blocked_tech_school";
	const mappers::BlockedTechSchools mapper(input);

	ASSERT_TRUE(mapper.isTechSchoolBlocked("a_blocked_tech_school"));
}

TEST(Mappers_BlockedSchoolsTests, techSchoolCannotBeFound)
{
	std::stringstream input;
	input << "blocked = another_school\n";
	input << "blocked = third_school\n";
	const mappers::BlockedTechSchools mapper(input);

	ASSERT_FALSE(mapper.isTechSchoolBlocked("a_blocked_tech_school"));
}
