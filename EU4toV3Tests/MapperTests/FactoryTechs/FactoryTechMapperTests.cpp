#include "FactoryTechs/FactoryTechMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_FactoryTechMapperTests, techsCanBePinged)
{
	std::stringstream input;
	input << "empty_tech ={\n";
	input << "}";
	input << "full_tech ={\n";
	input << "\tactivate_building = factory1\n";
	input << "}";
	const mappers::FactoryTechMapper mapper(input);

	ASSERT_EQ("full_tech", *mapper.getTechForFactoryType("factory1"));
}

TEST(Mappers_FactoryTechMapperTests, mismatchReturnsNullopt)
{
	std::stringstream input;
	input << "empty_tech ={\n";
	input << "}";
	input << "full_tech ={\n";
	input << "\tactivate_building = factory1\n";
	input << "}";
	const mappers::FactoryTechMapper mapper(input);

	ASSERT_EQ(std::nullopt, mapper.getTechForFactoryType("factory-error"));
}
