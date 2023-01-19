#include "Loaders/VanillaCountryHistoryLoader/VanillaCountryHistoryEntries.h"
#include "gtest/gtest.h"

TEST(V3World_VanillaCountryHistoryEntriesTests, DefaultsDefaultToDefaults)
{
	const V3::VanillaCountryHistoryEntries entries;

	EXPECT_TRUE(entries.getCountryHistoryElements().empty());
}

TEST(V3World_VanillaCountryHistoryEntriesTests, entriesCanBeLoaded)
{
	std::stringstream input;
	input << "c:GBR = {\n";
	input << "	set_tax_level = medium\n";
	input << "	ig:ig_devout = {\n";
	input << "		set_interest_group_name = ig_sunni_madrasahs\n";
	input << "	}\n";
	input << "}\n";
	input << "c:AUS = {\n";
	input << "	add_technology_researched = academia\n";
	input << "}\n";
	input << "c:AUS = {\n";
	input << "	activate_law = law_type:law_state_religion\n";
	input << "}\n";
	const V3::VanillaCountryHistoryEntries entries(input);

	ASSERT_EQ(2, entries.getCountryHistoryElements().size());
	const auto& gbr = entries.getCountryHistoryElements().at("GBR");
	const auto& aus = entries.getCountryHistoryElements().at("AUS");

	EXPECT_EQ("set_tax_level = medium", gbr[0]);
	EXPECT_EQ("ig:ig_devout = {\n\t\tset_interest_group_name = ig_sunni_madrasahs\n\t}", gbr[1]);

	EXPECT_EQ("add_technology_researched = academia", aus[0]);
	EXPECT_EQ("activate_law = law_type:law_state_religion", aus[1]);
}
