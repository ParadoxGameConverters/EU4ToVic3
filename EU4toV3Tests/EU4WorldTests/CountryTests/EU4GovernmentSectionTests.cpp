#include "Country/EU4GovernmentSection.h"
#include "gtest/gtest.h"

TEST(EU4World_Country_EU4GovernmentSectionTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const EU4::GovernmentSection government(input);

	ASSERT_TRUE(government.getGovernment().empty());
	ASSERT_TRUE(government.getGovernmentReforms().empty());
}

TEST(EU4World_Country_EU4GovernmentSectionTests, reformsCanBeLoaded)
{
	std::stringstream input;
	input << "government = the_government\n";
	input << "reform_stack = {\n";
	input << "\treforms = {\n";
	input << "\t\ta_reform the_reform the_the_reform\n";
	input << "\t}\n";
	input << "}\n";
	const EU4::GovernmentSection government(input);

	ASSERT_EQ("the_government", government.getGovernment());
	ASSERT_EQ(3, government.getGovernmentReforms().size());
	ASSERT_TRUE(government.getGovernmentReforms().count("a_reform"));
	ASSERT_TRUE(government.getGovernmentReforms().count("the_reform"));
	ASSERT_TRUE(government.getGovernmentReforms().count("the_the_reform"));
}
