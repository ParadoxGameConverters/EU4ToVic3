#include "CountryManager/EU4CountryModifier.h"
#include "gtest/gtest.h"

TEST(EU4World_EU4CountryModifierTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const EU4::EU4CountryModifier modifier(input);

	EXPECT_TRUE(modifier.getModifier().empty());
}

TEST(EU4World_Country_EU4CountryModifierTests, modifierCanBeLoaded)
{
	std::stringstream input;
	input << "modifier = a_modifier\n";
	const EU4::EU4CountryModifier modifier(input);

	EXPECT_EQ("a_modifier", modifier.getModifier());
}
