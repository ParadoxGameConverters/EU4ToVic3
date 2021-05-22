#include "Country/EU4ActiveIdeas.h"
#include "gtest/gtest.h"

TEST(EU4World_Country_EU4ActiveIdeasTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const EU4::EU4ActiveIdeas ideas(input);

	ASSERT_TRUE(ideas.getActiveIdeas().empty());
}

TEST(EU4World_Country_EU4ActiveIdeasTests, ideasDevelopedTo7OrMoreAreLoaded)
{
	std::stringstream input;
	input << "idea1 = 4\n";
	input << "idea2 = 7\n";
	input << "idea3 = 14\n"; // mods?
	const EU4::EU4ActiveIdeas ideas(input);

	ASSERT_EQ(2, ideas.getActiveIdeas().size());
	ASSERT_TRUE(ideas.getActiveIdeas().count("idea2"));
	ASSERT_TRUE(ideas.getActiveIdeas().count("idea3"));
}
