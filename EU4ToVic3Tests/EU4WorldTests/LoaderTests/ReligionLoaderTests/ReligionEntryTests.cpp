#include "ReligionLoader/ReligionEntry.h"
#include "gtest/gtest.h"

TEST(EU4World_ReligionEntryTests, EntryDefaultsToNoColor)
{
	std::stringstream input;
	const EU4::ReligionEntry entry(input);

	EXPECT_FALSE(entry.getColor());
}

TEST(EU4World_ReligionEntryTests, colorCanBeImported)
{
	std::stringstream input;
	input << "color = { 1 2 3 }";

	const EU4::ReligionEntry entry(input);
	ASSERT_TRUE(entry.getColor());

	EXPECT_EQ(commonItems::Color(std::array{1, 2, 3}), entry.getColor());
}
