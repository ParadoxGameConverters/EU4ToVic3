#include "PartyTypes/PartyTypeMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_PartyTypeMapperTests, partyTypeCanBeRetrieved)
{
	std::stringstream input;
	input << "conservative = { start_date = 1000.1.1 }\n";
	input << "liberal = { start_date = 1680.1.1 }\n";
	const mappers::PartyTypeMapper mapper(input);

	ASSERT_EQ(date("1000.1.1"), mapper.getPartyTypeByIdeology("conservative")->getStartDate());
	ASSERT_EQ(date("1680.1.1"), mapper.getPartyTypeByIdeology("liberal")->getStartDate());
}

TEST(Mappers_PartyTypeMapperTests, mismatchReturnsNullopt)
{
	std::stringstream input;
	input << "conservative = { start_date = 1000.1.1 }\n";
	input << "liberal = { start_date = 1680.1.1 }\n";
	const mappers::PartyTypeMapper mapper(input);

	ASSERT_EQ(std::nullopt, mapper.getPartyTypeByIdeology("reactionary"));
}
