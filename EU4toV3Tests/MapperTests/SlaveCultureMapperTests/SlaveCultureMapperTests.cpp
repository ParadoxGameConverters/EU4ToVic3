#include "SlaveCultureMapper/SlaveCultureMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_SlaveCultureMapperTests, MismatchReturnsNullopt)
{
	mappers::SlaveCultureMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/slave_culture_map.txt");

	EXPECT_FALSE(mapper.getSlaveCulture("mismatch"));
}

TEST(Mappers_SlaveCultureMapperTests, TraitCanBeMatched)
{
	mappers::SlaveCultureMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/slave_culture_map.txt");

	EXPECT_EQ("afro_american", mapper.getSlaveCulture("anglophone"));
}
