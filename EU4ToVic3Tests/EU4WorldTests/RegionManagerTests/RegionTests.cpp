#include "RegionManager/Area.h"
#include "RegionManager/Region.h"
#include "gtest/gtest.h"

TEST(Mappers_RegionTests, blankRegionLoadsWithNoAreas)
{
	std::stringstream input;
	const EU4::Region region(input);

	EXPECT_TRUE(region.getAreas().empty());
}

TEST(Mappers_RegionTests, areaCanBeLoaded)
{
	std::stringstream input;
	input << "areas = { test_area } \n";
	const EU4::Region region(input);

	EXPECT_TRUE(region.getAreas().contains("test_area"));
}

TEST(Mappers_RegionTests, multipleAreasCanBeLoaded)
{
	std::stringstream input;
	input << "areas = { test_area area2 area3 } \n";
	const EU4::Region region(input);

	EXPECT_EQ(3, region.getAreas().size());
}

TEST(Mappers_RegionTests, regionCanBeLinkedToArea)
{
	std::stringstream input;
	input << "areas = { test_area area2 area3 } \n";
	EU4::Region region(input);

	std::stringstream input2;
	input << "{ 3 4 5 6 } \n";
	auto area2 = std::make_shared<EU4::Area>(input);

	EXPECT_FALSE(region.getAreas().at("area2"));
	region.linkArea(std::pair("area2", area2));
	EXPECT_TRUE(region.getAreas().at("area2"));
}

TEST(Mappers_RegionTests, LinkedRegionCanLocateProvince)
{
	std::stringstream input;
	input << "areas = { area2 } \n";
	EU4::Region region(input);

	std::stringstream input2;
	input2 << "{ 3 4 5 6 } \n";
	auto area2 = std::make_shared<EU4::Area>(input2);

	region.linkArea(std::pair("area2", area2));

	EXPECT_TRUE(region.regionContainsProvince(5));
}

TEST(Mappers_RegionTests, LinkedRegionReturnsFalseForProvinceMismatch)
{
	std::stringstream input;
	input << "areas = { area2 } \n";
	EU4::Region region(input);

	std::stringstream input2;
	input << "{ 3 4 5 6 } \n";
	auto area2 = std::make_shared<EU4::Area>(input);

	region.linkArea(std::pair("area2", area2));

	EXPECT_FALSE(region.regionContainsProvince(9));
}
