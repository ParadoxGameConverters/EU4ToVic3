#include "LawMapper/LawMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_LawMappingTests, DefaultsDefaultToDefaults)
{
	std::stringstream input;
	const auto mapping = mappers::LawMapping(input);

	EXPECT_TRUE(mapping.getForms().empty());
	EXPECT_TRUE(mapping.getIdeas().empty());
	EXPECT_TRUE(mapping.getReforms().empty());
}

TEST(Mappers_LawMappingTests, MappingCanBeLoaded)
{
	std::stringstream input;
	input << "forms = { form1 form2 form3 }\n";
	input << "ideas = { idea1 idea2 idea3 }\n";
	input << "reforms = { reform1 reform2 reform3 }\n";
	const auto mapping = mappers::LawMapping(input);

	EXPECT_THAT(mapping.getForms(), testing::UnorderedElementsAre("form1", "form2", "form3"));
	EXPECT_THAT(mapping.getIdeas(), testing::UnorderedElementsAre("idea1", "idea2", "idea3"));
	EXPECT_THAT(mapping.getReforms(), testing::UnorderedElementsAre("reform1", "reform2", "reform3"));
}

TEST(Mappers_LawMappingTests, MappingCanBeScored)
{
	std::stringstream input;
	input << "forms = { form1 form2 form3 }\n";
	input << "ideas = { idea1 idea2 idea3 }\n";
	input << "reforms = { reform1 reform2 reform3 }\n";
	const auto mapping = mappers::LawMapping(input);

	const std::string form = "form2";													 // score = 0.01 because of fallback match
	const std::set<std::string> ideas = {"idea1", "idea2", "idea4"};			 // 2 matches, score = 2.01
	const std::set<std::string> reforms = {"reform1", "reform5", "reform2"}; // 2 matches, score = 4.01

	const auto score = mapping.getScore(form, ideas, reforms);

	EXPECT_DOUBLE_EQ(4.01, score);
}

TEST(Mappers_LawMappingTests, NoMatchesScoresZero)
{
	std::stringstream input;
	input << "forms = {}\n";
	input << "ideas = {}\n";
	input << "reforms = {}\n";
	const auto mapping = mappers::LawMapping(input);

	const std::string form = "form2";
	const std::set<std::string> ideas = {"idea1", "idea2", "idea4"};
	const std::set<std::string> reforms = {"reform1", "reform5", "reform2"};

	const auto score = mapping.getScore(form, ideas, reforms);

	EXPECT_EQ(0, score);
}
