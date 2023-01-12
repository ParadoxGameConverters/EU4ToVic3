#include "Loaders/VanillaBuildingLoader/VanillaBuildingEntries.h"
#include "gtest/gtest.h"

TEST(V3World_VanillaBuildingEntriesTests, DefaultsDefaultToDefaults)
{
	const V3::VanillaBuildingEntries entries;

	EXPECT_TRUE(entries.getBuildingElements().empty());
}

TEST(V3World_VanillaBuildingEntriesTests, entriesCanBeLoaded)
{
	std::stringstream input;
	input << "region_state:AUS = {\n";
	input << "	create_building = {\n";
	input << "		building = a\n";
	input << "	}\n";
	input << "}\n";
	input << "region_state:SAR = {\n";
	input << "	create_building = {\n";
	input << "		building = b\n";
	input << "	}\n";
	input << "	create_building = {\n";
	input << "		building = c\n";
	input << "	}\n";
	input << "}\n";
	const V3::VanillaBuildingEntries entries(input);

	ASSERT_EQ(2, entries.getBuildingElements().size());
	const auto& aus = entries.getBuildingElements().at("AUS");
	const auto& sar = entries.getBuildingElements().at("SAR");

	EXPECT_EQ("create_building = {\n\t\tbuilding = a\n\t}", aus[0]);

	EXPECT_EQ("create_building = {\n\t\tbuilding = b\n\t}", sar[0]);
	EXPECT_EQ("create_building = {\n\t\tbuilding = c\n\t}", sar[1]);
}
