#include "Country/EU4ReformStackSection.h"
#include "gtest/gtest.h"

TEST(EU4World_Country_EU4ReformStackSectionTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const EU4::ReformStackSection reforms(input);

	ASSERT_TRUE(reforms.getReforms().empty());
}

TEST(EU4World_Country_EU4ReformStackSectionTests, reformsCanBeLoaded)
{
	std::stringstream input;
	input << "reforms = {\n";
	input << "\ta_reform the_reform the_the_reform\n";
	input << "}\n";
	const EU4::ReformStackSection reforms(input);

	ASSERT_EQ(3, reforms.getReforms().size());
	ASSERT_TRUE(reforms.getReforms().count("a_reform"));
	ASSERT_TRUE(reforms.getReforms().count("the_reform"));
	ASSERT_TRUE(reforms.getReforms().count("the_the_reform"));
}
