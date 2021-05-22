#include "FactoryTechs/FactoryTechDetails.h"
#include "gtest/gtest.h"

TEST(Mappers_FactoryTechDetailsTests, nameIsEmpty)
{
	std::stringstream input;
	const mappers::FactoryTechDetails mapper(input);

	ASSERT_TRUE(mapper.getFactoryName().empty());
}

TEST(Mappers_FactoryTechDetailsTests, nameCanBeLoaded)
{
	std::stringstream input;
	input << "activate_building = factory1\n";
	const mappers::FactoryTechDetails mapper(input);

	ASSERT_EQ("factory1", mapper.getFactoryName());
}
