#include "PartyTypes/PartyType.h"
#include "gtest/gtest.h"

TEST(Mappers_PartyTypeTests, primitivesDefaultToDefaults)
{
	std::stringstream input;
	const mappers::PartyType mapper(input);

	ASSERT_FALSE(mapper.getEndDate().isSet());
	ASSERT_FALSE(mapper.getEndDate().isSet());
	ASSERT_TRUE(mapper.getIdeology().empty());
	ASSERT_TRUE(mapper.getName().empty());
}

TEST(Mappers_PartyTypeTests, nameCanBeSet)
{
	std::stringstream input;
	mappers::PartyType mapper(input);

	mapper.setName("Party");
	
	ASSERT_EQ("Party", mapper.getName());
}

TEST(Mappers_PartyTypeTests, ideologyCanBeSet)
{
	std::stringstream input;
	mappers::PartyType mapper(input);

	mapper.setIdeology("conservative");

	ASSERT_EQ("conservative", mapper.getIdeology());
}

TEST(Mappers_PartyTypeTests, datesCanBeSet)
{
	std::stringstream input;
	input << "start_date = 1800.1.1\n";
	input << "end_date = 2000.1.1\n";
	const mappers::PartyType mapper(input);

	ASSERT_EQ(date("1800.1.1"), mapper.getStartDate());
	ASSERT_EQ(date("2000.1.1"), mapper.getEndDate());
}

TEST(Mappers_PartyTypeTests, partyTypeCanBeOutput)
{
	std::stringstream input;
	input << "start_date = 1000.1.1\n";
	input << "end_date = 2000.1.1\n";
	input << "economic_policy = interventionism\n";
	input << "trade_policy = protectionism\n";
	input << "religious_policy = moralism\n";
	input << "citizenship_policy = limited_citizenship\n";
	input << "war_policy = pro_military\n";
	mappers::PartyType mapper(input);
	mapper.setIdeology("conservative");

	std::stringstream output;
	output << mapper;
	
	std::stringstream expected;
	expected << "\tstart_date = 1000.1.1\n";
	expected << "\tend_date = 2000.1.1\n\n";
	expected << "\tideology = conservative\n\n";
	expected << "\tcitizenship_policy = limited_citizenship\n";
	expected << "\teconomic_policy = interventionism\n";
	expected << "\treligious_policy = moralism\n";
	expected << "\ttrade_policy = protectionism\n";
	expected << "\twar_policy = pro_military\n";

	ASSERT_EQ(expected.str(), output.str());
}
