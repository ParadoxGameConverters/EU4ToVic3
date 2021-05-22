#include "PartyNames/PartyName.h"
#include "gtest/gtest.h"

TEST(Mappers_PartyNameTests, namesDefaultToEmpty)
{
	std::stringstream input;
	const mappers::PartyName mapper(input);

	ASSERT_TRUE(mapper.getLanguageToNameMap().empty());
}

TEST(Mappers_PartyNameTests, namesCanBeLoaded)
{
	std::stringstream input;
	input << "english = \"Party\"\n";
	input << "croatian = \"Stranka\"\n";
	input << "serbian = \"Partija\"\n";
	const mappers::PartyName mapper(input);

	ASSERT_EQ(3, mapper.getLanguageToNameMap().size());
	ASSERT_EQ("Party", mapper.getLanguageToNameMap().at("english"));
	ASSERT_EQ("Stranka", mapper.getLanguageToNameMap().at("croatian"));
	ASSERT_EQ("Partija", mapper.getLanguageToNameMap().at("serbian"));
}
