#include "Titles/TitleMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_TitleMapperTests, doesTitleExistCanBeChecked)
{
	std::stringstream input;
	input << "link = { name = test title = c_test region = e_persia }\n";
	const mappers::TitleMapper mapper(input);

	ASSERT_TRUE(mapper.doesTitleExist("c_test"));
	ASSERT_FALSE(mapper.doesTitleExist("mismatch"));
}

TEST(Mappers_TitleMapperTests, getTitleForNameReturnsTitle)
{
	std::stringstream input;
	input << "link = { name = test title = c_test region = e_persia }\n";
	const mappers::TitleMapper mapper(input);

	ASSERT_EQ("c_test", mapper.getTitleForName("test"));
	ASSERT_EQ(std::nullopt, mapper.getTitleForName("mismatch"));
}

TEST(Mappers_TitleMapperTests, getRandomIndianTitleReturnsNulloptforNoCandidates)
{
	std::stringstream input;
	mappers::TitleMapper mapper(input);

	ASSERT_EQ(std::nullopt, mapper.getRandomIndianTitle());
}

TEST(Mappers_TitleMapperTests, getRandomIndianTitleReturnsAMatch)
{
	std::stringstream input;
	input << "link = { name = test title = c_test region = e_deccan }\n";
	mappers::TitleMapper mapper(input);

	ASSERT_TRUE(mapper.getRandomIndianTitle());
	ASSERT_EQ("c_test", *mapper.getRandomIndianTitle());
}

TEST(Mappers_TitleMapperTests, getRandomIslamicTitleReturnsNulloptforNoCandidates)
{
	std::stringstream input;
	mappers::TitleMapper mapper(input);

	ASSERT_EQ(std::nullopt, mapper.getRandomIslamicTitle());
}

TEST(Mappers_TitleMapperTests, getRandomIslamicTitleReturnsIfPossible)
{
	std::stringstream input;
	input << "link = { name = test title = c_test region = e_persia }\n";
	mappers::TitleMapper mapper(input);

	ASSERT_TRUE(mapper.getRandomIslamicTitle());
	ASSERT_EQ("c_test", *mapper.getRandomIslamicTitle());
}
