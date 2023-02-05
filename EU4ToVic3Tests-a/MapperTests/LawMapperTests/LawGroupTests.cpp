#include "LawMapper/LawGroup.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_LawGroupTests, LawGroupDefaultToEmpty)
{
	std::stringstream input;
	const auto group = mappers::LawGroup(input);

	EXPECT_TRUE(group.getLaws().empty());
}

TEST(Mappers_LawGroupTests, LawGroupCanBeLoaded)
{
	std::stringstream input;
	input << "law1 = { ideas = { a } }\n";
	input << "law2 = { forms = { b } }\n";
	const auto laws = mappers::LawGroup(input).getLaws();

	EXPECT_TRUE(laws.contains("law1"));
	EXPECT_TRUE(laws.contains("law2"));
	EXPECT_THAT(laws.at("law1").getIdeas(), testing::UnorderedElementsAre("a"));
	EXPECT_THAT(laws.at("law2").getForms(), testing::UnorderedElementsAre("b"));
}
