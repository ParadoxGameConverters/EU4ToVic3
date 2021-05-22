#include "NavalBases/NavalBase.h"
#include "gtest/gtest.h"

TEST(Mappers_NavalBases_NavalBaseTests, foundBaseIsFalse)
{
	std::stringstream input;
	const mappers::NavalBase mapper(input);

	ASSERT_FALSE(mapper.isFound());
}

TEST(Mappers_NavalBases_NavalBaseTests, baseCanBeFound)
{
	std::stringstream input;
	input << "naval_base = something";
	const mappers::NavalBase mapper(input);

	ASSERT_TRUE(mapper.isFound());
}
