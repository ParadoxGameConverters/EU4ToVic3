#include "ReligionLoader/ReligionParser.h"
#include "gtest/gtest.h"

TEST(EU4World_ReligionParserTests, religionsDefaultToEmpty)
{
	std::stringstream input;
	const EU4::ReligionParser religionGroup(input);
	const auto& theReligions = religionGroup.getReligions();

	EXPECT_TRUE(theReligions.empty());
}

TEST(EU4World_ReligionParserTests, religionsCanBeImported)
{
	std::stringstream input;
	// These keys is not relevant but all unnecessary named keys must be specifically ignored.
	input << "defender_of_faith = yes\n";
	input << "can_form_personal_unions = yes\n";
	input << "center_of_religion = 385 # Mecca\n";
	input << "flags_with_emblem_percentage = 33\n";
	input << "flag_emblem_index_range = { 110 110 }\n";
	input << "ai_will_propagate_through_trade = yes\n";
	input << "religious_schools = { big block of bull }\n";
	input << "zoroastrian = {\n";		 // <--
	input << "\tcolor = { 1 2 3 }\n"; // <--
	input << "\trest = irrelevant\n";
	input << "}\n";
	input << "harmonized_modifier = harmonized_zoroastrian_group\n"; // also not relevant.
	input << "crusade_name = HOLY_WAR\n";

	const EU4::ReligionParser religionGroup(input);
	ASSERT_EQ(1, religionGroup.getReligions().size());

	const auto& religion = religionGroup.getReligions()[0];

	EXPECT_EQ("zoroastrian", religion.name);
	EXPECT_EQ(commonItems::Color(std::array{1, 2, 3}), religion.color);
}
