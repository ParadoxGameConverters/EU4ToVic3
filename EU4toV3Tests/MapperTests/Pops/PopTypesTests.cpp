#include "Pops/PopTypes.h"
#include "gtest/gtest.h"

TEST(Mappers_Pops_PopTypesTests, popsDefaultToEmpty)
{
	std::stringstream input;
	const mappers::PopTypes mapper(input);

	ASSERT_TRUE(mapper.getPopTypes().empty());
}

TEST(Mappers_Pops_PopTypesTests, popsCanBeLoaded)
{
	std::stringstream input;
	input << "capitalist = { culture = c1 religion = r1 size = 150.00 }\n";
	input << "slave = { culture = c2 religion = r2 size = 15000.00 }\n";
	const mappers::PopTypes mapper(input);

	ASSERT_EQ(2, mapper.getPopTypes().size());
	ASSERT_EQ(150, mapper.getPopTypes()[0].size);
	ASSERT_EQ(15000, mapper.getPopTypes()[1].size);
}
