#include "Wars/EU4WarDetails.h"
#include "gtest/gtest.h"

TEST(EU4World_EU4WarDetailsTests, primitivesDefaultToDefaults)
{
	const EU4::WarDetails details;

	ASSERT_EQ(0, details.targetProvinceID);
	ASSERT_TRUE(details.warGoalType.empty());
	ASSERT_TRUE(details.targetTag.empty());
	ASSERT_TRUE(details.warGoalClass.empty());
	ASSERT_FALSE(details.startDate.isSet());
}

TEST(EU4World_EU4WarDetailsTests, detailsCanBeAddedMultipleTimes)
{
	EU4::WarDetails details;
	std::stringstream input;
	input << "type = serious_war\n";
	input << "province = 14\n";
	details.addDetails(input);

	std::stringstream input2;
	input2 << "tag = FRA\n";
	details.addDetails(input2);

	ASSERT_EQ(14, details.targetProvinceID);
	ASSERT_EQ("serious_war", details.warGoalType);
	ASSERT_EQ("FRA", details.targetTag);
}

TEST(EU4World_EU4WarDetailsTests, startDateCatchesOnlyFirstDate)
{
	EU4::WarDetails details;
	std::stringstream input;
	input << "7.7.7 = { something happened }\n";
	input << "8.8.8 = { someone bailed }\n";
	input << "9.9.9 = { further developments }\n";
	details.addDetails(input);

	ASSERT_EQ(date("7.7.7"), details.startDate);
}
