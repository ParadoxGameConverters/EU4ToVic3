#include "Diplomacy/EU4Diplomacy.h"
#include "gtest/gtest.h"

TEST(EU4World_EU4DiplomacyTests, primitivesDefaultToEmpty)
{
	std::stringstream input;
	const EU4::EU4Diplomacy diplomacy(input);

	ASSERT_TRUE(diplomacy.getAgreements().empty());
}

TEST(EU4World_EU4DiplomacyTests, dependencyAgreementsAreLoadedDirectly)
{
	std::stringstream input;
	input << "dependency = {\n";
	input << "type=tributary\n";
	input << "first=BYZ\n";
	input << "second=TUR\n";
	input << "start_date=1444.11.11\n";
	input << "}\n";
	input << "dependency = {\n";
	input << "type=vassal\n";
	input << "first=BYZ\n";
	input << "second=TUR\n";
	input << "start_date=1445.11.11\n";
	input << "}\n";

	const EU4::EU4Diplomacy diplomacy(input);
	const auto& agreement1 = diplomacy.getAgreements()[0];
	const auto& agreement2 = diplomacy.getAgreements()[1];

	ASSERT_EQ(2, diplomacy.getAgreements().size());
	ASSERT_EQ("tributary", agreement1.getAgreementType());
	ASSERT_EQ("BYZ", agreement1.getOriginTag());
	ASSERT_EQ("TUR", agreement1.getTargetTag());
	ASSERT_EQ(date("1444.11.11"), agreement1.getStartDate());
	ASSERT_EQ("vassal", agreement2.getAgreementType());
	ASSERT_EQ("BYZ", agreement2.getOriginTag());
	ASSERT_EQ("TUR", agreement2.getTargetTag());
	ASSERT_EQ(date("1445.11.11"), agreement2.getStartDate());
}

TEST(EU4World_EU4DiplomacyTests, nonDependencyAgreementsHaveTypeOverridden)
{
	std::stringstream input;
	input << "royal_marriage = {\n";
	input << "type=irrelevant\n";
	input << "}\n";
	input << "guarantee = {\n";
	input << "type=irrelevant\n";
	input << "}\n";
	input << "alliance = {\n";
	input << "type=irrelevant\n";
	input << "}\n";

	const EU4::EU4Diplomacy diplomacy(input);
	const auto& agreement1 = diplomacy.getAgreements()[0];
	const auto& agreement2 = diplomacy.getAgreements()[1];
	const auto& agreement3 = diplomacy.getAgreements()[2];

	ASSERT_EQ("royal_marriage", agreement1.getAgreementType());
	ASSERT_EQ("guarantee", agreement2.getAgreementType());
	ASSERT_EQ("alliance", agreement3.getAgreementType());
}

TEST(EU4World_EU4DiplomacyTests, colonialAgreementsHaveTypeOverridden)
{
	std::stringstream input;
	input << "is_colonial = {\n";
	input << "type=irrelevant\n";
	input << "}\n";

	const EU4::EU4Diplomacy diplomacy(input);
	const auto& agreement1 = diplomacy.getAgreements()[0];

	ASSERT_EQ("colonial", agreement1.getAgreementType());
}

TEST(EU4World_EU4DiplomacyTests, oldDependencyTypeAgreementsHaveTypeOverriddenAsVassal)
{
	std::stringstream input;
	input << "vassal = {\n";
	input << "type=irrelevant\n";
	input << "}\n";
	input << "protectorate = {\n";
	input << "type=irrelevant\n";
	input << "}\n";
	input << "is_march = {\n";
	input << "type=irrelevant\n";
	input << "}\n";
	input << "sphere = {\n";
	input << "type=irrelevant\n";
	input << "}\n";
	input << "union = {\n";
	input << "type=irrelevant\n";
	input << "}\n";

	const EU4::EU4Diplomacy diplomacy(input);
	const auto& agreement1 = diplomacy.getAgreements()[0];
	const auto& agreement2 = diplomacy.getAgreements()[1];
	const auto& agreement3 = diplomacy.getAgreements()[2];
	const auto& agreement4 = diplomacy.getAgreements()[3];
	const auto& agreement5 = diplomacy.getAgreements()[4];

	ASSERT_EQ("vassal", agreement1.getAgreementType());
	ASSERT_EQ("vassal", agreement2.getAgreementType());
	ASSERT_EQ("vassal", agreement3.getAgreementType());
	ASSERT_EQ("vassal", agreement4.getAgreementType());
	ASSERT_EQ("vassal", agreement5.getAgreementType());
}
