#include "ProvinceManager/DateItems.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::UnorderedElementsAre;

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

	EXPECT_THAT(dateItems.getDateChanges(),
		 UnorderedElementsAre(EU4::DateChange("owner", "SWE"), EU4::DateChange("religion", "catholic"), EU4::DateChange("culture", "swedish")));
}
