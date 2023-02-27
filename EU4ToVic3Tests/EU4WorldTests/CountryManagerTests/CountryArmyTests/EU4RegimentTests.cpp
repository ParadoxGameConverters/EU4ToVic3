#include "CountryManager/CountryArmy/EU4Regiment.h"
#include "gtest/gtest.h"

TEST(EU4World_EU4RegimentTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const EU4::EU4Regiment regiment(input);

	EXPECT_TRUE(regiment.getType().empty());
	EXPECT_TRUE(regiment.getName().empty());
	EXPECT_EQ(0, regiment.getHome());
	EXPECT_EQ(0, regiment.getStrength());
}

TEST(EU4World_EU4RegimentTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "name=\"Penobscott's 1st Regiment\"\n";
	input << "home=971\n";
	input << "type=\"native_indian_archer\"\n";
	const EU4::EU4Regiment regiment(input);

	EXPECT_EQ("native_indian_archer", regiment.getType());
	EXPECT_EQ("Penobscott's 1st Regiment", regiment.getName());
	EXPECT_EQ(971, regiment.getHome());
	EXPECT_EQ(0, regiment.getStrength()); // Strengths are assigned by Army after loading.
}
