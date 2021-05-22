#include "NavalBases/NavalBaseMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_NavalBases_NavalBaseMapperTests, coastalProvinceCanBePinged)
{
	std::stringstream input;
	input << "1 = { building_position = { naval_base = lovely } }\n";
	input << "2 = { not a naval province }\n";
	input << "3 = { building_position = { naval_base = nice } }\n";
	const mappers::NavalBaseMapper mapper(input);

	ASSERT_TRUE(mapper.isProvinceCoastal(1));
	ASSERT_FALSE(mapper.isProvinceCoastal(2));
	ASSERT_TRUE(mapper.isProvinceCoastal(3));
}

TEST(Mappers_NavalBases_NavalBaseMapperTests, provinceMismatchesReturnFalse)
{
	std::stringstream input;
	input << "1 = { building_position = { naval_base = lovely } }\n";
	input << "2 = { not a naval province }\n";
	input << "3 = { building_position = { naval_base = nice } }\n";
	const mappers::NavalBaseMapper mapper(input);

	ASSERT_FALSE(mapper.isProvinceCoastal(4));
}
