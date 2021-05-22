#include "AfricaReset/AfricaResetMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_AfricaResetMapperTests, regionCanBeMatched)
{
	std::stringstream input;
	input << "reset_regions = { some_area some_region } ";
	const mappers::AfricaResetMapper mapper(input);

	ASSERT_TRUE(mapper.isRegionResettable("some_area"));
	ASSERT_TRUE(mapper.isRegionResettable("some_region"));
}

TEST(Mappers_AfricaResetMapperTests, regionCanBeMisMatched)
{
	std::stringstream input;
	const mappers::AfricaResetMapper mapper(input);

	ASSERT_FALSE(mapper.isRegionResettable("some_area"));
}

TEST(Mappers_AfricaResetMapperTests, westernizationLevelDefaultsToFive)
{
	std::stringstream input;
	const mappers::AfricaResetMapper mapper(input);

	ASSERT_TRUE(mapper.isTechResettable(4));
	ASSERT_FALSE(mapper.isTechResettable(5));
}

TEST(Mappers_AfricaResetMapperTests, westernizationLevelCanBeSet)
{
	std::stringstream input;
	input << "reset_below_westernization = 8";
	const mappers::AfricaResetMapper mapper(input);

	ASSERT_TRUE(mapper.isTechResettable(7));
	ASSERT_FALSE(mapper.isTechResettable(8));
}
