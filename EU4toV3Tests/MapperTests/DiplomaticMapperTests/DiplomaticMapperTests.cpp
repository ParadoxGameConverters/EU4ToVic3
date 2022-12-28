#include "DiplomaticMapper/DiplomaticMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_DiplomaticMapperTests, agreementsCanBeLoadedAndMatched)
{
	mappers::DiplomaticMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/diplomatic_map.txt");

	EXPECT_TRUE(mapper.isAgreementInDominions("private_enterprise"));
	EXPECT_TRUE(mapper.isAgreementInProtectorates("colony"));
	EXPECT_TRUE(mapper.isAgreementInDefensivePacts("guarantee"));
	EXPECT_TRUE(mapper.isAgreementInTributaries("tributary_state"));
	EXPECT_TRUE(mapper.isAgreementInPersonalUnions("personal_union"));
	EXPECT_TRUE(mapper.isAgreementInPuppets("march"));
	EXPECT_TRUE(mapper.isAgreementInVassals("vassal"));
	EXPECT_TRUE(mapper.isAgreementInDoubleRelationshipBoosts("royal_marriage"));
	EXPECT_TRUE(mapper.isAgreementInDoubleDefensivePacts("alliance"));
}

TEST(Mappers_DiplomaticMapperTests, agreementsCanBeMisMatched)
{
	const mappers::DiplomaticMapper mapper;

	EXPECT_FALSE(mapper.isAgreementInDominions("private_enterprise"));
	EXPECT_FALSE(mapper.isAgreementInProtectorates("colony"));
	EXPECT_FALSE(mapper.isAgreementInDefensivePacts("guarantee"));
	EXPECT_FALSE(mapper.isAgreementInTributaries("tributary_state"));
	EXPECT_FALSE(mapper.isAgreementInPersonalUnions("personal_union"));
	EXPECT_FALSE(mapper.isAgreementInPuppets("march"));
	EXPECT_FALSE(mapper.isAgreementInVassals("vassal"));
	EXPECT_FALSE(mapper.isAgreementInDoubleRelationshipBoosts("royal_marriage"));
	EXPECT_FALSE(mapper.isAgreementInDoubleDefensivePacts("alliance"));
}
