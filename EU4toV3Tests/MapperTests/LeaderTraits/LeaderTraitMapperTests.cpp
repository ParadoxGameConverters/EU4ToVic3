#include "LeaderTraits/LeaderTraitMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_LeaderTraitMapperTests, backgroundCanBeMatched)
{
	std::stringstream input;
	input << "background = {\n";
	input << "\ttrait5 = { fire = 5 }\n"; // won't match
	input << "\ttrait3 = { fire = 3 }\n"; // will match
	input << "\ttrait1 = { fire = 1 }\n"; // won't consider
	input << "}\n";
	const mappers::LeaderTraitMapper mapper(input);

	ASSERT_EQ("trait3", *mapper.getBackground(4, 0, 0, 0));
}

TEST(Mappers_LeaderTraitMapperTests, personalityCanBeMatched)
{
	std::stringstream input;
	input << "personality = {\n";
	input << "\ttrait5 = { fire = 5 }\n";
	input << "\ttrait3 = { fire = 3 }\n";
	input << "\ttrait1 = { fire = 1 }\n";
	input << "}\n";
	const mappers::LeaderTraitMapper mapper(input);

	ASSERT_EQ("trait3", *mapper.getPersonality(4, 0, 0, 0));
}

TEST(Mappers_LeaderTraitMapperTests, mismatchReturnsNullopt)
{
	std::stringstream input;
	input << "personality = {\n";
	input << "\ttrait5 = { fire = 5 }\n";
	input << "\ttrait3 = { fire = 3 }\n";
	input << "\ttrait1 = { fire = 1 }\n";
	input << "}\n";
	input << "background = {\n";
	input << "\ttrait5 = { fire = 5 }\n";
	input << "\ttrait3 = { fire = 3 }\n";
	input << "\ttrait1 = { fire = 1 }\n";
	input << "}\n";
	const mappers::LeaderTraitMapper mapper(input);

	ASSERT_EQ(std::nullopt, mapper.getBackground(0, 0, 0, 0));
	ASSERT_EQ(std::nullopt, mapper.getPersonality(0, 0, 0, 0));
}
