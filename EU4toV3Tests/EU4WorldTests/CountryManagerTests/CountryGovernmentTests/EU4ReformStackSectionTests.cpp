#include "CountryManager/CountryGovernment/EU4ReformStackSection.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::UnorderedElementsAre;

TEST(EU4World_EU4ReformStackSectionTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const EU4::ReformStackSection reforms(input);

	EXPECT_TRUE(reforms.getReforms().empty());
}

TEST(EU4World_EU4ReformStackSectionTests, reformsCanBeLoaded)
{
	std::stringstream input;
	input << "reforms = {\n";
	input << "\ta_reform the_reform the_the_reform\n";
	input << "}\n";
	const EU4::ReformStackSection reforms(input);

	EXPECT_THAT(reforms.getReforms(), UnorderedElementsAre("a_reform", "the_reform", "the_the_reform"));
}
