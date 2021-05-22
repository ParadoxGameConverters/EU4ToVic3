#include "Factory/Factory.h"
#include "FactoryTypes/FactoryTypeMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_FactoryTypeMapperTests, factoriesCanBeBuilt)
{
	std::stringstream mainInput;
	mainInput << "screw_factory = {\n";
	mainInput << "\tis_coastal = yes\n";
	mainInput << "\tinput_goods = {\n";
	mainInput << "\t\tiron = 0.4\n";
	mainInput << "\t\tscrew_cap = 0.2\n";
	mainInput << "\t}\n";
	mainInput << "\toutput_goods = screw\n";
	mainInput << "}\n";

	std::stringstream techInput;
	techInput << "screwdriver_technology = { activate_building = screw_factory }";

	std::stringstream inventionInput;
	inventionInput << "screw_casting = { effect = { activate_building = screw_factory } }";

	std::stringstream countInput;
	countInput << "screw_factory = 3";

	mappers::FactoryTypeMapper mapper(mainInput, techInput, inventionInput, countInput);

	auto builtFactories = mapper.buildFactories();
	ASSERT_EQ(3, builtFactories.size());

	// let's grab one of those.
	const auto& aFactory = builtFactories.front(); // This is a V2::Factory now.

	ASSERT_EQ("screwdriver_technology", aFactory->getRequiredTech());
	ASSERT_EQ("screw_casting", aFactory->getRequiredInvention());
	ASSERT_EQ("screw", aFactory->getOutputs());
	ASSERT_EQ("screw_factory", aFactory->getTypeName());
	ASSERT_TRUE(aFactory->requiresCoastal());
}
