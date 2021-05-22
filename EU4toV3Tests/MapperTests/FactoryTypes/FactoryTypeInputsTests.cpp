#include "FactoryTypes/FactoryTypeInputs.h"
#include "gtest/gtest.h"

TEST(Mappers_FactoryTypeInputsTests, inputsDefaultToEmpty)
{
	std::stringstream input;
	const mappers::FactoryTypeInputs mapper(input);

	ASSERT_TRUE(mapper.getProductionInputValues().empty());
}

TEST(Mappers_FactoryTypeInputsTests, inputsCanBeLoaded)
{
	std::stringstream input;
	input << "banana = 0.45\n";
	input << "kiwi = 0.55\n";
	const mappers::FactoryTypeInputs mapper(input);

	ASSERT_NEAR(0.45, mapper.getProductionInputValues().at("banana"), 0.001);
	ASSERT_NEAR(0.55, mapper.getProductionInputValues().at("kiwi"), 0.001);
}
