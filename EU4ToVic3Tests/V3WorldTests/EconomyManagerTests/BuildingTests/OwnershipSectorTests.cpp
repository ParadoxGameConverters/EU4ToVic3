#include "EconomyManager/Building/Ownership/OwnershipSector.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_OwnershipTests, ParserLoadsInValues)
{
	std::stringstream input;

	input << "ownership = { \n";
	input << "building_x = { \n";
	input << "weight = 0.4 \n";
	input << "colonial = 0.2 \n";
	input << "financial = 0.3 \n";
	input << "} \n";
	input << "local = 0.2 \n";
	input << "national = 0.2 \n";
	input << "national_service = 0.2 \n";
	input << "} \n";
	input << "building_1 \n";
	input << "building_2 \n";

	V3::OwnershipSector ownershipSector(input);

	const auto& data = ownershipSector.getOwnerships().at("building_x");

	EXPECT_EQ("building_x", data.type);
	EXPECT_EQ(0.4, data.weight);
	EXPECT_EQ(0.2, data.colonialFrac);
	EXPECT_EQ(0.3, data.financialCenterFrac);
}