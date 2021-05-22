#include "Pops/PopInstance.h"
#include "gtest/gtest.h"

TEST(Mappers_Pops_PopInstanceTests, primitivesDefaultToDefaults)
{
	std::stringstream input;
	const mappers::PopInstance pop(input, "capitalist");

	ASSERT_TRUE(pop.getPopDetails().religion.empty());
	ASSERT_TRUE(pop.getPopDetails().culture.empty());
	ASSERT_EQ(0, pop.getPopDetails().size);
	ASSERT_EQ("capitalist", pop.getPopDetails().popType);
}

TEST(Mappers_Pops_PopInstanceTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "culture = culture1\n";
	input << "religion = religion1\n";
	input << "size = 150.00\n";
	const mappers::PopInstance pop(input, "capitalist");

	ASSERT_EQ("religion1", pop.getPopDetails().religion);
	ASSERT_EQ("culture1", pop.getPopDetails().culture);
	ASSERT_EQ(150, pop.getPopDetails().size);
	ASSERT_EQ("capitalist", pop.getPopDetails().popType);
}
