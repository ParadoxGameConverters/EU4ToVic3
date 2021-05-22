#include "DeadDefinitionMapper/DeadDefinitionMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_DeadDefinitionMapperTests, definitionsCanBeMatched)
{
	std::stringstream input;
	input << "link = { tag = AAA primary_culture = dummy religion = pre_schism government = theocracy civilized = no capital = 322 }\n";
	input << "link = { tag = AAC primary_culture = north_german religion = protestant government = presidential_dictatorship civilized = yes capital = 576 }\n";
	const mappers::DeadDefinitionMapper deaded(input);

	ASSERT_EQ("dummy", deaded.getDeadDefinitionForTag("AAA")->culture);
	ASSERT_EQ("pre_schism", deaded.getDeadDefinitionForTag("AAA")->religion);
	ASSERT_EQ("theocracy", deaded.getDeadDefinitionForTag("AAA")->government);
	ASSERT_TRUE(deaded.getDeadDefinitionForTag("AAC")->civilized);
	ASSERT_EQ(576, deaded.getDeadDefinitionForTag("AAC")->capital);
}

TEST(Mappers_DeadDefinitionMapperTests, definitionsReturnNulloptOnMismatch)
{
	std::stringstream input;
	const mappers::DeadDefinitionMapper deaded(input);

	ASSERT_FALSE(deaded.getDeadDefinitionForTag("AAA"));
}
