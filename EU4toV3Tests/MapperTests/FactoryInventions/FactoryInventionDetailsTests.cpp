#include "FactoryInventions/FactoryInventionDetails.h"
#include "gtest/gtest.h"

TEST(Mappers_FactoryInventionDetailsTests, nameIsEmpty)
{
	std::stringstream input;
	const mappers::FactoryInventionDetails mapper(input);

	ASSERT_TRUE(mapper.getFactoryName().empty());
}

TEST(Mappers_FactoryInventionDetailsTests, nameCanBeLoaded)
{
	std::stringstream input;
	input << "activate_building = factory1\n";
	const mappers::FactoryInventionDetails mapper(input);

	ASSERT_EQ("factory1", mapper.getFactoryName());
}
