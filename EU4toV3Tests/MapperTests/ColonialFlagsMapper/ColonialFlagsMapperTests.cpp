#include "ColonialFlags/ColonialFlagsMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_ColonialFlagsMapperTests, flagsCanBeLoadedAndRetrieved)
{
	std::stringstream input;
	input << "region_test = {\n";
	input << "\tflag = { name = test1 }\n";
	input << "\tflag = { name = test2 }\n";
	input << "\tflag = { name = test3 unique = true }\n";
	input << "}\n";
	const mappers::ColonialFlagsMapper mapper(input);

	const auto& test1 = mapper.getFlag("test1");
	const auto& test2 = mapper.getFlag("test2");
	const auto& test3 = mapper.getFlag("test3");
	
	ASSERT_EQ("test1", test1->getName());
	ASSERT_EQ("test2", test2->getName());
	ASSERT_EQ("test3", test3->getName());
}

TEST(Mappers_ColonialFlagsMapperTests, flagsMismatchReturnsNullopt)
{
	std::stringstream input;
	const mappers::ColonialFlagsMapper mapper(input);

	ASSERT_EQ(std::nullopt, mapper.getFlag("mismatch"));
}

TEST(Mappers_ColonialFlagsMapperTests, commonNamesCanBeRetrieved)
{
	std::stringstream input;
	input << "region_test = {\n";
	input << "\tflag = { name = test1 }\n";
	input << "\tflag = { name = test2 }\n";
	input << "\tflag = { name = test3 unique = true }\n";
	input << "}\n";
	const mappers::ColonialFlagsMapper mapper(input);

	const auto& names = mapper.getCommonNames();
	
	ASSERT_EQ(2, names.size());
	ASSERT_EQ("test1", names[0]);
	ASSERT_EQ("test2", names[1]);
}

TEST(Mappers_ColonialFlagsMapperTests, flagCanBeRemoved)
{
	std::stringstream input;
	input << "region_test = {\n";
	input << "\tflag = { name = test1 }\n";
	input << "}\n";
	mappers::ColonialFlagsMapper mapper(input);

	mapper.removeFlag("test1");

	ASSERT_EQ(std::nullopt, mapper.getFlag("test1"));
}
