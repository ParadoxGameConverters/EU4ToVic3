#include "CountryManager/EU4ActivePolicy.h"
#include "gtest/gtest.h"

TEST(EU4World_EU4ActivePolicyTests, policyDefaultsToEmpty)
{
	std::stringstream input;
	const EU4::EU4ActivePolicy thePolicy(input);

	EXPECT_TRUE(thePolicy.getPolicy().empty());
}

TEST(EU4World_EU4ActivePolicyTests, policyCanBeImported)
{
	std::stringstream input;
	input << "policy=\"agricultural_cultivations\"";
	const EU4::EU4ActivePolicy thePolicy(input);

	EXPECT_EQ(thePolicy.getPolicy(), "agricultural_cultivations");
}
