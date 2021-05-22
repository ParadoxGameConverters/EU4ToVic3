#include "gtest/gtest.h"
#include "Country/EU4ActivePolicy.h"
#include <sstream>



TEST(EU4World_Country_EU4ActivePolicyTests, policyDefaultsToEmpty)
{
	std::stringstream input;

	EU4::EU4ActivePolicy thePolicy(input);
	ASSERT_TRUE(thePolicy.getPolicy().empty());
}


TEST(EU4World_Country_EU4ActivePolicyTests, policyCanBeImported)
{
	std::stringstream input;
	input << "policy=\"agricultural_cultivations\"";

	EU4::EU4ActivePolicy thePolicy(input);
	ASSERT_EQ(thePolicy.getPolicy(), "agricultural_cultivations");
}


