#include "DiplomacyParser/DiplomacyParser.h"
#include "gtest/gtest.h"

TEST(EU4World_DiplomacyTests, primitivesDefaultToEmpty)
{
	const EU4::DiplomacyParser diplomacy;

	EXPECT_TRUE(diplomacy.getAgreements().empty());
}

TEST(EU4World_DiplomacyTests, dependencyAgreementsAreLoadedDirectly)
{
	std::stringstream input;
	input << "dependency = {\n";
	input << "	type=tributary\n";
	input << "	first=BYZ\n";
	input << "	second=TUR\n";
	input << "	start_date=1444.11.11\n";
	input << "}\n";
	input << "dependency = {\n";
	input << "	type=vassal\n";
	input << "	first=BYZ\n";
	input << "	second=TUR\n";
	input << "	start_date=1445.11.11\n";
	input << "}\n";
	EU4::DiplomacyParser diplomacy;
	diplomacy.loadDiplomacy(input);

	const auto& agreement1 = diplomacy.getAgreements()[0];
	const auto& agreement2 = diplomacy.getAgreements()[1];

	EXPECT_EQ(2, diplomacy.getAgreements().size());
	EXPECT_EQ("tributary", agreement1.getAgreementType());
	EXPECT_EQ("BYZ", agreement1.getOriginTag());
	EXPECT_EQ("TUR", agreement1.getTargetTag());
	EXPECT_EQ(date("1444.11.11"), agreement1.getStartDate());
	EXPECT_EQ("vassal", agreement2.getAgreementType());
	EXPECT_EQ("BYZ", agreement2.getOriginTag());
	EXPECT_EQ("TUR", agreement2.getTargetTag());
	EXPECT_EQ(date("1445.11.11"), agreement2.getStartDate());
}

TEST(EU4World_DiplomacyTests, nonDependencyAgreementsHaveTypeOverridden)
{
	std::stringstream input;
	input << "royal_marriage = {\n";
	input << "	type=irrelevant\n";
	input << "}\n";
	input << "guarantee = {\n";
	input << "	type=irrelevant\n";
	input << "}\n";
	input << "alliance = {\n";
	input << "	type=irrelevant\n";
	input << "}\n";
	EU4::DiplomacyParser diplomacy;
	diplomacy.loadDiplomacy(input);

	const auto& agreement1 = diplomacy.getAgreements()[0];
	const auto& agreement2 = diplomacy.getAgreements()[1];
	const auto& agreement3 = diplomacy.getAgreements()[2];

	EXPECT_EQ("royal_marriage", agreement1.getAgreementType());
	EXPECT_EQ("guarantee", agreement2.getAgreementType());
	EXPECT_EQ("alliance", agreement3.getAgreementType());
}
