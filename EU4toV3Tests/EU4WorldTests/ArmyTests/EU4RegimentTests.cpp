#include "Army/EU4Regiment.h"
#include "gtest/gtest.h"

TEST(EU4World_EU4RegimentTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	EU4::EU4Regiment regiment(input);

	ASSERT_TRUE(regiment.getType().empty());
	ASSERT_TRUE(regiment.getName().empty());
	ASSERT_EQ(0, regiment.getHome());
	ASSERT_EQ(0, regiment.getTypeStrength());
	ASSERT_TRUE(regiment.getCategory().empty());
}

TEST(EU4World_EU4RegimentTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "name=\"Penobscott's 1st Regiment\"\n";
	input << "home=971\n";
	input << "type=\"native_indian_archer\"\n";
	EU4::EU4Regiment regiment(input);

	ASSERT_EQ("native_indian_archer", regiment.getType());
	ASSERT_EQ("Penobscott's 1st Regiment", regiment.getName());
	ASSERT_EQ(971, regiment.getHome());
	ASSERT_TRUE(regiment.getCategory().empty()); // Categories are assigned by Army after loading.
}
