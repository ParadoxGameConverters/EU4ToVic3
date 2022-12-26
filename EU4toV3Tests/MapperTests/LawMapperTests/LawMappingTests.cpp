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
