#include "Provinces/DateItems.h"
#include "gtest/gtest.h"

TEST(EU4World_Province_DateItemsTests, emptyItemsAreNotImported)
{
	std::stringstream input;
	const EU4::DateItems dateItems(input);

	ASSERT_TRUE(dateItems.getDateChanges().empty());
}

TEST(EU4World_Province_DateItemsTests, itemsCanBeImported)
{
	std::stringstream input;
	input << "owner = SWE\n";
	input << "religion = catholic\n";
	input << "culture = swedish\n";

	const EU4::DateItems dateItems(input);

	ASSERT_EQ(3, dateItems.getDateChanges().size());
	ASSERT_EQ("owner", dateItems.getDateChanges()[0].changeType);
	ASSERT_EQ("SWE", dateItems.getDateChanges()[0].changeValue);
	ASSERT_EQ("religion", dateItems.getDateChanges()[1].changeType);
	ASSERT_EQ("catholic", dateItems.getDateChanges()[1].changeValue);
	ASSERT_EQ("culture", dateItems.getDateChanges()[2].changeType);
	ASSERT_EQ("swedish", dateItems.getDateChanges()[2].changeValue);
}
