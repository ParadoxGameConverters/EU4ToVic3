#include "PartyNames/PartyNameMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_PartyNameMapperTests, partiesDefaultToEmpty)
{
	std::stringstream input;
	const mappers::PartyNameMapper mapper(input);

	ASSERT_TRUE(mapper.getPartyToLanguageMap().empty());
}

TEST(Mappers_PartyNameMapperTests, partiesCanBeLoaded)
{
	std::stringstream input;
	input << "conservative = {\n";
	input << "\tenglish = \"Conservative Party\"\n";
	input << "\tcroatian = \"Konzervativna Stranka\"\n";
	input << "\tserbian = \"Konzervativna Partija\"\n";
	input << "}\n";
	input << "liberal = {\n";
	input << "\tenglish = \"Liberal Party\"\n";
	input << "\tcroatian = \"Liberalna Stranka\"\n";
	input << "\tserbian = \"Liberalna Partija\"\n";
	input << "}\n";
	const mappers::PartyNameMapper mapper(input);

	ASSERT_EQ(2, mapper.getPartyToLanguageMap().size());
	ASSERT_TRUE(mapper.getPartyToLanguageMap().contains("conservative"));
	ASSERT_TRUE(mapper.getPartyToLanguageMap().contains("liberal"));
	ASSERT_EQ("Liberalna Stranka", mapper.getPartyToLanguageMap().at("liberal").getLanguageToNameMap().at("croatian"));
}
