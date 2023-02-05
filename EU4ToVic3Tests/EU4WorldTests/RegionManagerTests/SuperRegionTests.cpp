#include "RegionManager/Area.h"
#include "RegionManager/Region.h"
#include "RegionManager/SuperRegion.h"
#include "gtest/gtest.h"

TEST(Mappers_SuperRegionTests, blankSuperRegionLoadsWithNoRegions)
{
	const std::vector<std::string> input;
	const EU4::SuperRegion superRegion(input);

	EXPECT_TRUE(superRegion.getRegions().empty());
}

TEST(Mappers_SuperRegionTests, RegionsCanBeLoaded)
{
	const std::vector<std::string> input = {"region1", "region2"};
	const EU4::SuperRegion superRegion(input);

	EXPECT_TRUE(superRegion.getRegions().contains("region1"));
	EXPECT_TRUE(superRegion.getRegions().contains("region2"));
}

TEST(Mappers_SuperRegionTests, superRegionCanBeLinkedToRegion)
{
	std::stringstream input;
	input << "areas = { test_area area2 area3 } \n";
	auto theRegion = std::make_shared<EU4::Region>(input);

	const std::vector<std::string> input2 = {"region1"};
	EU4::SuperRegion superRegion(input2);

	EXPECT_FALSE(superRegion.getRegions().at("region1"));
	superRegion.linkRegion(std::pair("region1", theRegion));
	EXPECT_TRUE(superRegion.getRegions().at("region1"));
}

TEST(Mappers_SuperRegionTests, LinkedRegionCanLocateProvince)
{
	std::stringstream input2;
	input2 << "{ 3 4 5 6 } \n";
	auto area2 = std::make_shared<EU4::Area>(input2);

	std::stringstream input;
	input << "areas = { area2 } \n";
	auto theRegion = std::make_shared<EU4::Region>(input);
	theRegion->linkArea(std::pair("area2", area2));

	const std::vector<std::string> input3 = {"region1"};
	EU4::SuperRegion superRegion(input3);
	superRegion.linkRegion(std::pair("region1", theRegion));

	EXPECT_TRUE(superRegion.superRegionContainsProvince(5));
}

TEST(Mappers_SuperRegionTests, LinkedRegionReturnsFalseOnProvinceMismatch)
{
	std::stringstream input2;
	input2 << "{ 3 4 5 6 } \n";
	auto area2 = std::make_shared<EU4::Area>(input2);

	std::stringstream input;
	input << "areas = { area2 } \n";
	auto theRegion = std::make_shared<EU4::Region>(input);
	theRegion->linkArea(std::pair("area2", area2));

	const std::vector<std::string> input3 = {"region1"};
	EU4::SuperRegion superRegion(input3);
	superRegion.linkRegion(std::pair("region1", theRegion));

	EXPECT_FALSE(superRegion.superRegionContainsProvince(9));
}

TEST(Mappers_SuperRegionTests, NativeCultureCanBeRegisteredAndRetrieved)
{
	const std::vector<std::string> input = {"region"};
	EU4::SuperRegion superRegion(input);

	superRegion.registerNativeCulture("culture");

	EXPECT_TRUE(superRegion.superRegionContainsNativeCulture("culture"));
	EXPECT_FALSE(superRegion.superRegionContainsNativeCulture("dummy"));
}
