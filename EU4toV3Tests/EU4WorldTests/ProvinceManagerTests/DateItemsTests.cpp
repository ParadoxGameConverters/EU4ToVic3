#include "ProvinceManager/DateItems.h"
#include "gtest/gtest.h"

TEST(EU4World_DateItemsTests, emptyItemsAreNotImported)
{
	std::stringstream input;
	const EU4::DateItems dateItems(input);

	EXPECT_TRUE(dateItems.getDateChanges().empty());
}

TEST(EU4World_DateItemsTests, itemsCanBeImported)
{
	std::stringstream input;
	input << "owner = SWE\n";
	input << "religion = catholic\n";
	input << "culture = swedish\n";

	const EU4::DateItems dateItems(input);

	EXPECT_EQ(3, dateItems.getDateChanges().size());
	EXPECT_EQ("owner", dateItems.getDateChanges()[0].changeType);
	EXPECT_EQ("SWE", dateItems.getDateChanges()[0].changeValue);
	EXPECT_EQ("religion", dateItems.getDateChanges()[1].changeType);
	EXPECT_EQ("catholic", dateItems.getDateChanges()[1].changeValue);
	EXPECT_EQ("culture", dateItems.getDateChanges()[2].changeType);
	EXPECT_EQ("swedish", dateItems.getDateChanges()[2].changeValue);
}
