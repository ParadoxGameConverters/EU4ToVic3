#include "AcceptedCultureThresholds/AcceptedCultureThresholdsMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_AcceptedCultureThresholdsMapperTests, PrimitivesDefaultToZero)
{
	std::stringstream input;
	const mappers::AcceptedCultureThresholdsMapper theMapper(input);

	ASSERT_EQ(0.0, theMapper.getSameGroup());
	ASSERT_EQ(0.0, theMapper.getForeignGroup());
}

TEST(Mappers_AcceptedCultureThresholdsMapperTests, PrimitivesCanBeLoaded)
{
	std::stringstream input;
	input << "same_group = 0.15\n";
	input << "foreign_group = 0.2\n";
	const mappers::AcceptedCultureThresholdsMapper theMapper(input);

	ASSERT_EQ(0.15, theMapper.getSameGroup());
	ASSERT_EQ(0.2, theMapper.getForeignGroup());
}
