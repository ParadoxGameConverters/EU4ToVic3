#include "WarParser/WarDetails.h"
#include "gtest/gtest.h"

TEST(EU4World_WarDetailsTests, primitivesDefaultToDefaults)
{
	const EU4::WarDetails details;

	EXPECT_EQ(0, details.targetProvinceID);
	EXPECT_TRUE(details.warGoalType.empty());
	EXPECT_TRUE(details.targetTag.empty());
	EXPECT_TRUE(details.warGoalClass.empty());
	EXPECT_FALSE(details.startDate.isSet());
}

TEST(EU4World_WarDetailsTests, detailsCanBeAddedMultipleTimes)
{
	EU4::WarDetails details;
	std::stringstream input;
	input << "type = serious_war\n";
	input << "province = 14\n";
	details.addDetails(input);

	std::stringstream input2;
	input2 << "tag = FRA\n";
	details.addDetails(input2);

	EXPECT_EQ(14, details.targetProvinceID);
	EXPECT_EQ("serious_war", details.warGoalType);
	EXPECT_EQ("FRA", details.targetTag);
}

TEST(EU4World_WarDetailsTests, startDateCatchesOnlyFirstDate)
{
	EU4::WarDetails details;
	std::stringstream input;
	input << "7.7.7 = { something happened }\n";
	input << "8.8.8 = { someone bailed }\n";
	input << "9.9.9 = { further developments }\n";
	details.addDetails(input);

	EXPECT_EQ(date("7.7.7"), details.startDate);
}
