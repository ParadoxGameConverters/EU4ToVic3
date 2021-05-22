#include "Religions/ReligionGroup.h"
#include "gtest/gtest.h"

TEST(EU4World_Religions_ReligionGroupTests, religionsDefaultToEmpty)
{
	std::stringstream input;
	EU4::ReligionGroup religionGroup(input);
	const auto theReligions = religionGroup.takeReligions();

	ASSERT_TRUE(theReligions.empty());
}

TEST(EU4World_Religions_ReligionGroupTests, religionsCanBeImported)
{
	std::stringstream input;
	// These keys is not relevant but all unnecessary named keys are specifically ignored.
	input << "defender_of_faith = yes\n";
	input << "can_form_personal_unions = yes\n";
	input << "center_of_religion = 385 # Mecca\n";
	input << "flags_with_emblem_percentage = 33\n";
	input << "flag_emblem_index_range = { 110 110 }\n";
	input << "ai_will_propagate_through_trade = yes\n";
	input << "religious_schools = { big block of bull }\n";
	input << "zoroastrian = {\n"; // Name is relevant, content is not.
	input << "\tabsolutely irrelevant\n";
	input << "}\n";
	input << "harmonized_modifier = harmonized_zoroastrian_group\n"; // also not relevant.
	input << "crusade_name = HOLY_WAR\n";

	EU4::ReligionGroup religionGroup(input);
	const auto theReligions = religionGroup.takeReligions();

	ASSERT_TRUE(theReligions.contains("zoroastrian"));
}
