#include "FactoryInventions/FactoryInventionMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_FactoryInventionMapperTests, inventionsCanBePinged)
{
	std::stringstream input;
	input << "empty_invention ={\n";
	input << "\teffect = {\n";
	input << "\t}";
	input << "}";
	input << "full_invention ={\n";
	input << "\teffect = {\n";
	input << "\t\tactivate_building = factory1\n";
	input << "\t}";
	input << "}";
	const mappers::FactoryInventionMapper mapper(input);

	ASSERT_EQ("full_invention", *mapper.getInventionForFactoryType("factory1"));
}

TEST(Mappers_FactoryInventionMapperTests, mismatchReturnsNullopt)
{
	std::stringstream input;
	input << "empty_invention ={\n";
	input << "\teffect = {\n";
	input << "\t}";
	input << "}";
	input << "full_invention ={\n";
	input << "\teffect = {\n";
	input << "\t\tactivate_building = factory1\n";
	input << "\t}";
	input << "}";
	const mappers::FactoryInventionMapper mapper(input);

	ASSERT_EQ(std::nullopt, mapper.getInventionForFactoryType("factory-error"));
}
