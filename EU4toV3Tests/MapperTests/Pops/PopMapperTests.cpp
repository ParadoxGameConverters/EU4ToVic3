#include "Pops/PopMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_Pops_PopMapperTests, provincesDefaultToEmpty)
{
	std::stringstream input;
	const mappers::PopMapper mapper(input);

	ASSERT_TRUE(mapper.getProvincePops().empty());
}

TEST(Mappers_Pops_PopMapperTests, provincesandPopsCanBeLoaded)
{
	std::stringstream input;
	input << "1 = {\n";
	input << "\tcapitalist = { culture = c1 religion = r1 size = 150.00 }\n";
	input << "\tslave = { culture = c2 religion = r2 size = 15000.00 }\n";
	input << "}\n";
	input << "2 = {\n";
	input << "\tintellectual = { culture = c3 religion = r3 size = 350.00 }\n";
	input << "\twageslave = { culture = c4 religion = r4 size = 35000.00 }\n";
	input << "}\n";
	const mappers::PopMapper mapper(input);

	ASSERT_EQ(2, mapper.getProvincePops().size());
	ASSERT_TRUE(mapper.getProvincePops().contains(1));
	ASSERT_TRUE(mapper.getProvincePops().contains(2));

	ASSERT_EQ(2, mapper.getProvincePops().at(1).size());
	ASSERT_EQ(150, mapper.getProvincePops().at(1)[0].size);
	ASSERT_EQ(15000, mapper.getProvincePops().at(1)[1].size);

	ASSERT_EQ(2, mapper.getProvincePops().at(2).size());
	ASSERT_EQ(350, mapper.getProvincePops().at(2)[0].size);
	ASSERT_EQ(35000, mapper.getProvincePops().at(2)[1].size);
}
