#include "DiplomacyParser/EU4Agreement.h"
#include "gtest/gtest.h"

TEST(EU4World_EU4AgreementTests, primitivesDefaultToDefaults)
{
	std::stringstream input;
	const EU4::EU4Agreement agreement(input);

	EXPECT_TRUE(agreement.getAgreementType().empty());
	EXPECT_TRUE(agreement.getOriginTag().empty());
	EXPECT_TRUE(agreement.getTargetTag().empty());
	EXPECT_EQ(date("1.1.1"), agreement.getStartDate());
	EXPECT_EQ(date("1.1.1"), agreement.getEndDate());
}

TEST(EU4World_EU4AgreementTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "type=vassal\n";
	input << "first=BYZ\n";
	input << "second=TUR\n";
	input << "start_date=1444.11.11\n";
	input << "end_date=1445.1.1\n";
	const EU4::EU4Agreement agreement(input);

	EXPECT_EQ("vassal", agreement.getAgreementType());
	EXPECT_EQ("BYZ", agreement.getOriginTag());
	EXPECT_EQ("TUR", agreement.getTargetTag());
	EXPECT_EQ(date("1444.11.11"), agreement.getStartDate());
	EXPECT_EQ(date("1445.1.1"), agreement.getEndDate());
}
