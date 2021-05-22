#include "AgreementMapper/AgreementMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_AgreementMapperTests, agreementsCanBeMatched)
{
	std::stringstream input;
	input << "colonies = { colony }\n";
	input << "onesiders = { guarantee }\n";
	input << "doublesiders = { royal_marriage }\n";
	input << "tributaries = { tributary_state }\n";
	input << "vassals = { vassal }\n";
	const mappers::AgreementMapper mapper(input);

	ASSERT_TRUE(mapper.isAgreementInColonies("colony"));
	ASSERT_TRUE(mapper.isAgreementInOnesiders("guarantee"));
	ASSERT_TRUE(mapper.isAgreementInDoublesiders("royal_marriage"));
	ASSERT_TRUE(mapper.isAgreementInTributaries("tributary_state"));
	ASSERT_TRUE(mapper.isAgreementInVassals("vassal"));
}

TEST(Mappers_AgreementMapperTests, agreementsCanBeMisMatched)
{
	std::stringstream input;
	const mappers::AgreementMapper mapper(input);

	ASSERT_FALSE(mapper.isAgreementInColonies("colony"));
	ASSERT_FALSE(mapper.isAgreementInOnesiders("guarantee"));
	ASSERT_FALSE(mapper.isAgreementInDoublesiders("royal_marriage"));
	ASSERT_FALSE(mapper.isAgreementInTributaries("tributary_state"));
	ASSERT_FALSE(mapper.isAgreementInVassals("vassal"));
}
