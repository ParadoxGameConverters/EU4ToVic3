#include "Diplomacy/EU4Agreement.h"
#include "gtest/gtest.h"

TEST(EU4World_EU4AgreementTests, primitivesDefaultToDefaults)
{
	std::stringstream input;
	const EU4::EU4Agreement agreement(input);

	ASSERT_TRUE(agreement.getAgreementType().empty());
	ASSERT_TRUE(agreement.getOriginTag().empty());
	ASSERT_TRUE(agreement.getTargetTag().empty());
	ASSERT_EQ(date("1.1.1"), agreement.getStartDate());
	ASSERT_EQ(date("1.1.1"), agreement.getEndDate());
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

	ASSERT_EQ("vassal", agreement.getAgreementType());
	ASSERT_EQ("BYZ", agreement.getOriginTag());
	ASSERT_EQ("TUR", agreement.getTargetTag());
	ASSERT_EQ(date("1444.11.11"), agreement.getStartDate());
	ASSERT_EQ(date("1445.1.1"), agreement.getEndDate());
}
