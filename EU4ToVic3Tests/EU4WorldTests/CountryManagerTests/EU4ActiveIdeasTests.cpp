#include "CountryManager/EU4ActiveIdeas.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::UnorderedElementsAre;

TEST(EU4World_EU4ActiveIdeasTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const EU4::EU4ActiveIdeas ideas(input);

	EXPECT_TRUE(ideas.getActiveIdeas().empty());
}

TEST(EU4World_EU4ActiveIdeasTests, ideasDevelopedTo7OrMoreAreLoaded)
{
	std::stringstream input;
	input << "idea1 = 4\n";
	input << "idea2 = 7\n";
	input << "idea3 = 14\n"; // mods?
	const EU4::EU4ActiveIdeas ideas(input);

	EXPECT_THAT(ideas.getActiveIdeas(), UnorderedElementsAre("idea2", "idea3"));
}
