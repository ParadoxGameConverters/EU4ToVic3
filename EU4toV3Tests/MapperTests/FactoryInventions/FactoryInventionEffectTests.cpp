#include "FactoryInventions/FactoryInventionEffect.h"
#include "gtest/gtest.h"

TEST(Mappers_FactoryInventionEffectTests, nameIsEmpty)
{
	std::stringstream input;
	const mappers::FactoryInventionEffect mapper(input);

	ASSERT_TRUE(mapper.getFactoryName().empty());
}

TEST(Mappers_FactoryInventionEffectTests, nameCanBeLoaded)
{
	std::stringstream input;
	input << "effect = {\n";
	input << "\tactivate_building = factory1\n";
	input << "}";
	const mappers::FactoryInventionEffect mapper(input);

	ASSERT_EQ("factory1", mapper.getFactoryName());
}
