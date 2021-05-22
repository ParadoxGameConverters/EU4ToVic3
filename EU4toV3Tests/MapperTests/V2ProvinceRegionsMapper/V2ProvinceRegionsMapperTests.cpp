#include "V2ProvinceRegionsMapper/V2ProvinceRegionsMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_V2ProvinceRegionsMapperTests, provinceMismatchReturnsNullopt)
{
	std::stringstream input;
	const mappers::V2ProvinceRegionsMapper v2ProvinceRegionsMapper(input);

	const auto& match = v2ProvinceRegionsMapper.getRegionForProvince(1);

	ASSERT_EQ(std::nullopt, match);
}

TEST(Mappers_V2ProvinceRegionsMapperTests, provinceMatchReturnsRegion)
{
	std::stringstream input;
	input << "USA_1 = { 1 2 3 4 5 2599 2630 2631 }\n";
	input << "USA_78 = { 78 79 80 }\n ";

	const mappers::V2ProvinceRegionsMapper v2ProvinceRegionsMapper(input);

	const auto& match = v2ProvinceRegionsMapper.getRegionForProvince(79);

	ASSERT_EQ("USA_78", *match);
}
