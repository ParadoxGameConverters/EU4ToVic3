#include "ProvinceMappings/ProvinceMapping.h"
#include "gtest/gtest.h"

TEST(Mappers_ProvinceMappingTests, provincesDefaultToEmpty)
{
	std::stringstream input;
	const mappers::ProvinceMapping mapper(input);

	ASSERT_TRUE(mapper.getEU4Provinces().empty());
	ASSERT_TRUE(mapper.getVic2Provinces().empty());
}

TEST(Mappers_ProvinceMappingTests, provincesCanBeAdded)
{
	std::stringstream input;
	input << "eu4 = 2 eu4 = 1 v2 = 2 v2 = 1";
	const mappers::ProvinceMapping mapper(input);

	ASSERT_EQ(2, mapper.getEU4Provinces().size());
	ASSERT_TRUE(mapper.getEU4Provinces().contains(1));
	ASSERT_TRUE(mapper.getEU4Provinces().contains(2));
	ASSERT_EQ(2, mapper.getVic2Provinces().size());
	ASSERT_TRUE(mapper.getVic2Provinces().contains(1));
	ASSERT_TRUE(mapper.getVic2Provinces().contains(2));
}

TEST(Mappers_ProvinceMappingTests, exceessProvincesAreDiscarded)
{
	std::stringstream input;
	input << "eu4 = 2 eu4 = 1 v2 = 2 v2 = 1 v2 = 2 v2 = 1"; // some repetition fo error's sake.
	const mappers::ProvinceMapping mapper(input);

	ASSERT_EQ(2, mapper.getEU4Provinces().size());
	ASSERT_TRUE(mapper.getEU4Provinces().contains(1));
	ASSERT_TRUE(mapper.getEU4Provinces().contains(2));
	ASSERT_EQ(2, mapper.getVic2Provinces().size());
	ASSERT_TRUE(mapper.getVic2Provinces().contains(1));
	ASSERT_TRUE(mapper.getVic2Provinces().contains(2));
}
