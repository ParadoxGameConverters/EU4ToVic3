#include "RegionManager/RegionManager.h"
#include "gtest/gtest.h"

// This is a collective mapper test for Area, Region, SuperRegion and RegionManager.

TEST(Mappers_RegionMapperTests, regionMapperCanBeEnabled)
{
	// We start humble, it's a machine.
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	std::stringstream regionStream;
	std::stringstream superRegionStream;

	theMapper.loadRegions(areaStream, regionStream, superRegionStream);
	EXPECT_FALSE(theMapper.provinceIsInRegion(1, "test"));
	EXPECT_FALSE(theMapper.regionNameIsValid("test"));
	EXPECT_FALSE(theMapper.getParentAreaName(1));
	EXPECT_FALSE(theMapper.getParentRegionName(1));
	EXPECT_FALSE(theMapper.getParentSuperRegionName(1));
}

TEST(Mappers_RegionMapperTests, loadingBrokenAreaWillThrowException)
{
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	areaStream << "broken_area = { 1 2 3 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }";
	try
	{
		theMapper.loadRegions(areaStream, regionStream, superRegionStream);
		FAIL();
	}
	catch (const std::runtime_error& e)
	{
		EXPECT_STREQ("Region's test_region area test_area does not exist!", e.what());
	}
}

TEST(Mappers_RegionMapperTests, loadingBrokenRegionWillThrowException)
{
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } ";
	std::stringstream regionStream;
	regionStream << "broken_region = { areas = { test_area } }";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }";
	try
	{
		theMapper.loadRegions(areaStream, regionStream, superRegionStream);
		FAIL();
	}
	catch (const std::runtime_error& e)
	{
		EXPECT_STREQ("Superregion's test_superregion region test_region does not exist!", e.what());
	}
}

TEST(Mappers_RegionMapperTests, locationServicesWork)
{
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	EXPECT_TRUE(theMapper.provinceIsInRegion(3, "test_area"));
	EXPECT_TRUE(theMapper.provinceIsInRegion(2, "test_region"));
	EXPECT_TRUE(theMapper.provinceIsInRegion(1, "test_superregion"));
}

TEST(Mappers_RegionMapperTests, locationServicesCorrectlyFail)
{
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } \n";
	areaStream << "test_area2 = { 4 5 6 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	regionStream << "test_region2 = { areas = { test_area2 } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	superRegionStream << "test_superregion2 = { test_region2 }";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	EXPECT_FALSE(theMapper.provinceIsInRegion(4, "test_area"));
	EXPECT_FALSE(theMapper.provinceIsInRegion(5, "test_region"));
	EXPECT_FALSE(theMapper.provinceIsInRegion(6, "test_superregion"));
}

TEST(Mappers_RegionMapperTests, locationServicesFailForNonsense)
{
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } \n";
	areaStream << "test_area2 = { 4 5 6 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	regionStream << "test_region2 = { areas = { test_area2 } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	superRegionStream << "test_superregion2 = { test_region2 }";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	EXPECT_FALSE(theMapper.provinceIsInRegion(1, "nonsense"));
	EXPECT_FALSE(theMapper.provinceIsInRegion(9, "test_area"));
}

TEST(Mappers_RegionMapperTests, correctParentLocationsReported)
{
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } \n";
	areaStream << "test_area2 = { 4 5 6 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	regionStream << "test_region2 = { areas = { test_area2 } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	superRegionStream << "test_superregion2 = { test_region2 }";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	EXPECT_EQ("test_area", *theMapper.getParentAreaName(1));
	EXPECT_EQ("test_region", *theMapper.getParentRegionName(1));
	EXPECT_EQ("test_superregion", *theMapper.getParentSuperRegionName(1));
	EXPECT_EQ("test_area2", *theMapper.getParentAreaName(5));
	EXPECT_EQ("test_region2", *theMapper.getParentRegionName(5));
	EXPECT_EQ("test_superregion2", *theMapper.getParentSuperRegionName(5));
}

TEST(Mappers_RegionMapperTests, wrongParentLocationsReturnNullopt)
{
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } \n";
	areaStream << "test_area2 = { 4 5 6 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	regionStream << "test_region2 = { areas = { test_area2 } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	superRegionStream << "test_superregion2 = { test_region2 }";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	EXPECT_FALSE(theMapper.getParentAreaName(7));
	EXPECT_FALSE(theMapper.getParentRegionName(7));
	EXPECT_FALSE(theMapper.getParentSuperRegionName(7));
}

TEST(Mappers_RegionMapperTests, locationNameValidationWorks)
{
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } \n";
	areaStream << "test_area2 = { 4 5 6 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	regionStream << "test_region2 = { areas = { test_area2 } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	superRegionStream << "test_superregion2 = { test_region2 }";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	EXPECT_TRUE(theMapper.regionNameIsValid("test_area"));
	EXPECT_TRUE(theMapper.regionNameIsValid("test_region2"));
	EXPECT_TRUE(theMapper.regionNameIsValid("test_superregion2"));
	EXPECT_FALSE(theMapper.regionNameIsValid("nonsense"));
}
