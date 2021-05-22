#include "RegimentCosts/RegimentCostsMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_RegimentCostsMapperTests, undefinedRegimentCostReturnsNullopt)
{
	std::stringstream input;
	const mappers::RegimentCostsMapper mapper(input);

	ASSERT_EQ(std::nullopt, mapper.getCostForRegimentType("some_regiment"));
}

TEST(Mappers_RegimentCostsMapperTests, RegimentCostCanBeReturned)
{
	std::stringstream input;
	input << "infantry = 100\n";
	input << "artilleryonly = 200\n";
	const mappers::RegimentCostsMapper mapper(input);

	ASSERT_EQ(100, mapper.getCostForRegimentType("infantry"));
	ASSERT_EQ(200, mapper.getCostForRegimentType("artilleryonly"));
}
