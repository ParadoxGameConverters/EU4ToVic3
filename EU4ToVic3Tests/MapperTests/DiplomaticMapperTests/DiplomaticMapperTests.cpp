#include "DiplomaticMapper/DiplomaticMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_DiplomaticMapperTests, agreementsCanBeLoadedAndMatched)
{
	mappers::DiplomaticMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/diplomatic_map.txt");

	EXPECT_EQ("dominion", *mapper.getAgreementType("private_enterprise"));
	EXPECT_EQ("customs_union", *mapper.getAgreementType("transfer_trade_power"));
	EXPECT_EQ(10, mapper.getRelationshipBoost("private_enterprise"));
	EXPECT_EQ(25, mapper.getRelationshipBoost("transfer_trade_power"));
}

TEST(Mappers_DiplomaticMapperTests, mismatchedAgreementsReturnNullAndZero)
{
	const mappers::DiplomaticMapper mapper;

	EXPECT_FALSE(mapper.getAgreementType("nonsense"));
	EXPECT_EQ(0, mapper.getRelationshipBoost("nonsense"));
}
