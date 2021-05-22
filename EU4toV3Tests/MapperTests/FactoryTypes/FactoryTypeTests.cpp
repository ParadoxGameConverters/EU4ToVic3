#include "FactoryCounts/FactoryStartingCounts.h"
#include "FactoryInventions/FactoryInventionMapper.h"
#include "FactoryTechs/FactoryTechMapper.h"
#include "FactoryTypes/FactoryType.h"
#include "gtest/gtest.h"

TEST(Mappers_FactoryTypeTests, primitivesDefaultToDefaults)
{
	std::stringstream input;
	const mappers::FactoryType mapper(input, "screw_factory");

	ASSERT_FALSE(mapper.isCoastal());
	ASSERT_EQ(0, mapper.getStartingCount());
	ASSERT_EQ("screw_factory", mapper.getName());
	ASSERT_TRUE(mapper.getInputs().empty());
	ASSERT_TRUE(mapper.getOutputs().empty());
	ASSERT_TRUE(mapper.getRequiredInvention().empty());
	ASSERT_TRUE(mapper.getRequiredTech().empty());
}

TEST(Mappers_FactoryTypeTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "is_coastal = yes\n";
	input << "input_goods = {\n";
	input << "\tiron = 0.4\n";
	input << "\tscrew_cap = 0.2\n";
	input << "}\n";
	input << "output_goods = screw\n";

	const mappers::FactoryType mapper(input, "screw_factory");

	ASSERT_TRUE(mapper.isCoastal());
	ASSERT_EQ("screw_factory", mapper.getName());
	ASSERT_EQ(2, mapper.getInputs().size());
	ASSERT_EQ("screw", mapper.getOutputs());
}

TEST(Mappers_FactoryTypeTests, techsInventionsAndCountsCanBeAssigned)
{
	std::stringstream input;
	input << "is_coastal = true\n";
	input << "input_goods = {\n";
	input << "\tiron = 0.4\n";
	input << "\tscrew_cap = 0.2\n";
	input << "}\n";
	input << "output_goods = screw\n";
	mappers::FactoryType mapper(input, "screw_factory");

	std::stringstream techInput;
	techInput << "screwdriver_technology = { activate_building = screw_factory }";
	const mappers::FactoryTechMapper techMapper(techInput);

	std::stringstream inventionInput;
	inventionInput << "screw_casting = { effect = { activate_building = screw_factory } }";
	const mappers::FactoryInventionMapper inventionMapper(inventionInput);

	std::stringstream countInput;
	countInput << "screw_factory = 3";
	const mappers::FactoryStartingCounts countMapper(countInput);

	mapper.assignTechsAndCounts(techMapper.getTechForFactoryType(mapper.getName()),
		 inventionMapper.getInventionForFactoryType(mapper.getName()),
		 countMapper.getCountForFactoryType(mapper.getName()));

	ASSERT_EQ(3, mapper.getStartingCount());
	ASSERT_EQ("screw_casting", mapper.getRequiredInvention());
	ASSERT_EQ("screwdriver_technology", mapper.getRequiredTech());
}
