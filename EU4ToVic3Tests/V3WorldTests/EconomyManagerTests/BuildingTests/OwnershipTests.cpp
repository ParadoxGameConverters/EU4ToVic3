#include "EconomyManager/Building/Ownership/Ownership.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_OwnershipTests, DefaultsDefaultToDefaults)
{
	std::stringstream input;

	V3::Ownership ownership(input);

	const auto& data = ownership.getOwnership();

	EXPECT_EQ("", data.type);
	EXPECT_EQ(0.0, data.weight);
	EXPECT_EQ(0.0, data.colonialFrac);
	EXPECT_EQ(0.0, data.financialCenterFrac);
	EXPECT_FALSE(data.recognized);
}

TEST(V3World_OwnershipTests, OwnershipCtorLoadsData)
{
	std::stringstream input;

	input << "\t weight = 1 \n";
	input << "\t colonial = 0.2 \n";
	input << "\t financial = 0.3 \n";
	input << "\t recognized = yes \n";

	V3::Ownership ownership(input);

	const auto& data = ownership.getOwnership();

	EXPECT_EQ("", data.type);
	EXPECT_EQ(1, data.weight);
	EXPECT_EQ(0.2, data.colonialFrac);
	EXPECT_EQ(0.3, data.financialCenterFrac);
	EXPECT_TRUE(data.recognized);
}