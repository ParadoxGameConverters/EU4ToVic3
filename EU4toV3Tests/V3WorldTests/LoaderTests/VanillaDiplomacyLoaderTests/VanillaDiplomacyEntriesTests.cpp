#include "Loaders/VanillaDiplomacyLoader/VanillaDiplomacyEntries.h"
#include "gtest/gtest.h"

TEST(V3World_VanillaDiplomacyEntriesTests, DefaultsDefaultToDefaults)
{
	const V3::VanillaDiplomacyEntries entries;

	EXPECT_TRUE(entries.getAgreementEntries().empty());
	EXPECT_TRUE(entries.getRelationEntries().empty());
	EXPECT_TRUE(entries.getTruceEntries().empty());
}

TEST(V3World_VanillaDiplomacyEntriesTests, entriesCanBeLoaded)
{
	std::stringstream input;
	input << "c:GBR = {\n";
	input << "	create_diplomatic_pact = { country = c:USA type = defensive_pact }\n";
	input << "	set_relations = { country = c:BUR value = -30 }\n";
	input << "	create_truce = { country = c:BEL months = 7 }\n ";
	input << "}\n";
	input << "c:AUS = {\n";
	input << "	create_diplomatic_pact = { country = c:MOD type = customs_union }\n";
	input << "}\n";
	input << "c:AUS = {\n";
	input << "	set_relations = { country = c:KRA value = -31 }\n";
	input << "	create_truce = { country = c:EGY months = 8 }\n ";
	input << "}\n";
	const V3::VanillaDiplomacyEntries entries(input);

	ASSERT_EQ(2, entries.getAgreementEntries().size());
	const auto& agr1 = entries.getAgreementEntries()[0];
	const auto& agr2 = entries.getAgreementEntries()[1];

	EXPECT_EQ("USA", agr1.getTargetTag());
	EXPECT_EQ("customs_union", agr2.getAgreementType());

	ASSERT_EQ(2, entries.getRelationEntries().size());
	const auto& rel1 = entries.getRelationEntries()[0];
	const auto& rel2 = entries.getRelationEntries()[1];

	EXPECT_EQ("BUR", rel1.getTargetTag());
	EXPECT_EQ(-31, rel2.getRelationValue());

	ASSERT_EQ(2, entries.getTruceEntries().size());
	const auto& tru1 = entries.getTruceEntries()[0];
	const auto& tru2 = entries.getTruceEntries()[1];

	EXPECT_EQ("BEL", tru1.getTargetTag());
	EXPECT_EQ(8, tru2.getDuration());
}
