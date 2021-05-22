#include "ColonialFlags/ColonialFlag.h"
#include "gtest/gtest.h"

TEST(Mappers_ColonialFlagTests, primitivesDefaultToDefaults)
{
	std::stringstream input;
	const mappers::ColonialFlag flag(input, "region");

	ASSERT_TRUE(flag.getName().empty());
	ASSERT_TRUE(flag.getOverlord().empty());
	ASSERT_EQ("region", flag.getRegion());
	ASSERT_FALSE(flag.isUnique());
}

TEST(Mappers_ColonialFlagTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "name = \"New Spain\" unique = true\n";
	const mappers::ColonialFlag flag(input, "region");

	ASSERT_EQ("new spain", flag.getName());
	ASSERT_TRUE(flag.isUnique());
}

TEST(Mappers_ColonialFlagTests, nameIsLowerCased)
{
	std::stringstream input;
	input << "name = \"NEW SPAIN\" unique = true\n";
	const mappers::ColonialFlag flag(input, "region");

	ASSERT_EQ("new spain", flag.getName());
}

TEST(Mappers_ColonialFlagTests, overlordCanBeSet)
{
	std::stringstream input;
	input << "name = \"New Spain\" unique = true\n";
	mappers::ColonialFlag flag(input, "region");

	flag.setOverlord("SPA");

	ASSERT_EQ("SPA", flag.getOverlord());
}
