#include "CountryManager/CountryGovernment/EU4GovernmentSection.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::UnorderedElementsAre;

TEST(EU4World_EU4GovernmentSectionTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const EU4::GovernmentSection government(input);

	EXPECT_TRUE(government.getGovernment().empty());
	EXPECT_TRUE(government.getGovernmentReforms().empty());
}

TEST(EU4World_EU4GovernmentSectionTests, reformsCanBeLoaded)
{
	std::stringstream input;
	input << "government = the_government\n";
	input << "reform_stack = {\n";
	input << "\treforms = {\n";
	input << "\t\ta_reform the_reform the_the_reform\n";
	input << "\t}\n";
	input << "}\n";
	const EU4::GovernmentSection government(input);

	EXPECT_EQ("the_government", government.getGovernment());
	EXPECT_THAT(government.getGovernmentReforms(), UnorderedElementsAre("a_reform", "the_reform", "the_the_reform"));
}
